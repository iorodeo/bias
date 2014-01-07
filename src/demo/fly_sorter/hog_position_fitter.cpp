#include "hog_position_fitter.hpp"
#include "basic_image_proc.hpp"
#include "fast_binary_predictor.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <cfloat>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// PositionData
// ----------------------------------------------------------------------------
const bool PositionData::DEFAULT_IS_FLY = false;
const bool PositionData::DEFAULT_IS_MULTIPLE_FLIES = false;

PositionData::PositionData()
{
    isFly = DEFAULT_IS_FLY;
    isMultipleFlies = DEFAULT_IS_MULTIPLE_FLIES;
    flipped = false;
    success = false;
    bodyArea = 0; 
    meanXRel = 0.0;
    meanYRel = 0.0;
    meanXAbs = 0.0;
    meanYAbs = 0.0;
    ellipseMajorAxis = 0.0;
    ellipseMinorAxis = 0.0;
    ellipseAngle = 0.0;
    covarianceMatrix = cv::Mat(2,2,CV_64FC1,cv::Scalar(0.0));
}


std::string PositionData::toStdString(unsigned int indent)
{
    std::stringstream ss;
    std::string indentStr0 = getIndentString(indent);
    std::string indentStr1 = getIndentString(indent+1);
    std::string indentStr2 = getIndentString(indent+2);
    ss << indentStr0 << "PositionData:" << std::endl;
    ss << indentStr1 << "success: " << success << std::endl;
    ss << indentStr1 << "isFly: " << isFly << std::endl;
    ss << indentStr1 << "flipped: " << flipped << std::endl;
    ss << indentStr1 << "bodyArea: " << bodyArea << std::endl;
    ss << indentStr1 << "meanXRel: " << meanXRel << std::endl;
    ss << indentStr1 << "meanYRel: " << meanYRel << std::endl;
    ss << indentStr1 << "meanXAbs: " << meanXAbs << std::endl;
    ss << indentStr1 << "meanYAbs: " << meanYAbs << std::endl;
    ss << indentStr1 << "ellipseMajorAxis: " << ellipseMajorAxis << std::endl;
    ss << indentStr1 << "ellipseMinorAxis: " << ellipseMinorAxis << std::endl;
    ss << indentStr1 << "ellipseAngle: " << ellipseAngle << std::endl;
    ss << indentStr1 << "covarianceMatrix: " << std::endl;
    ss << indentStr1 << "frameCount: " << frameCount << std::endl;
    for (int i=0; i<2; i++)
    {
        ss << indentStr2; 
        for (int j=0; j<2; j++)
        {
            ss << covarianceMatrix.at<double>(i,j) << " ";
        }
        ss << std::endl;
    }
    ss << indentStr1 << "rotBoundingImageLUV: (not shown)" << std::endl;
    ss << indentStr1 << "pixelFeatureVector: (not shown)" << std::endl;
    ss << segmentData.toStdString(indent+1);
    return ss.str();
}


void PositionData::print(unsigned int indent)
{
    std::cout << toStdString(indent);
}


// HogPositionFitterData
// ----------------------------------------------------------------------------
HogPositionFitterData::HogPositionFitterData() {};

//  HogPositionFitter
//  ---------------------------------------------------------------------------

HogPositionFitter::HogPositionFitter() 
{ 
    showDebugWindow_ = false; 
    writeTrainingData_ = false;
    trainingFileNamePrefix_ = std::string("none");
};


HogPositionFitter::HogPositionFitter(HogPositionFitterParam param) : HogPositionFitter()
{
    setParam(param);
    if (showDebugWindow_)
    {
        //cv::namedWindow(
        //        "hogPosMaxComp",
        //        CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
        //        );
        //cv::namedWindow(
        //        "boundingImageLUV",
        //        CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
        //        );
        cv::namedWindow(
                "rotBoundingImageLUV",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
    }
};


void HogPositionFitter::setParam(HogPositionFitterParam param)
{
    param_ = param;
}


void HogPositionFitter::trainingDataWriteEnable(std::string fileNamePrefix)
{
    writeTrainingData_ = true;
    trainingFileNamePrefix_ = fileNamePrefix;
}


void HogPositionFitter::trainingDataWriteDisable()
{
    writeTrainingData_ = false;
}


HogPositionFitterData HogPositionFitter::fit(
        FlySegmenterData flySegmenterData, 
        unsigned long frameCount,
        cv::Mat img
        )
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    HogPositionFitterData fitterData;
    SegmentDataList segmentDataList = flySegmenterData.segmentDataList;
    SegmentDataList::iterator it;
    unsigned int cnt;

    for (it=segmentDataList.begin(), cnt=0; it!=segmentDataList.end(); it++, cnt++)
    {
        PositionData posData;
        posData.segmentData = *it;
        posData.frameCount = frameCount;

        // Detect Body pixels 
        cv::Mat closeMat = imCloseWithDiskElem(
                posData.segmentData.predictorData.label,
                param_.closeRadius
                );

        cv::Mat isBodyMat = bwAreaOpen(closeMat,param_.openArea);
        posData.bodyArea = cv::countNonZero(isBodyMat);

        // Ensure not on boarder and that bodyArea is above minimum for fly
        bool onBorder = posData.segmentData.blobData.isOnBorder();
        if (onBorder || (posData.bodyArea < param_.openArea))
        {
            // Note,  with the current implementation of bwAreaOpen you can
            // degenerate cases where bodyArea > 0 but less than openArea. For
            // example when the image is all 255. I'm not worrying about this
            // right now, but you fix this to make is more is line with
            // matlab's function. 
            posData.isFly = false;
            posData.success = false;
            fitterData.positionDataList.push_back(posData);
            continue;
        }
        else
        {
            // This is big enough to be a fly - get the largest connected component
            posData.isFly = true;
            cv::Mat maxCompMat = findMaxConnectedComponent(isBodyMat);

            // Write images to file
            // ----------------------------------------------------------------------
            //QString imgFileName = QString("maxCompMat_%1_%2.bmp").arg(frameCount).arg(cnt);
            //cv::imwrite(imgFileName.toStdString(), posData.segmentData.predictorData.label);
            //cv::imwrite(imgFileName.toStdString(),maxCompMat);
            // ----------------------------------------------------------------------

            // Find pixel nonzero pixel locations of maximum connected component.
            cv::Mat maxCompPointMat;
            cv::findNonZero(maxCompMat, maxCompPointMat);

            // Recompute body area for maximum connected component. 
            // Note, double check this w/ Kristin
            posData.bodyArea = cv::countNonZero(maxCompMat);

            // Check if area is too big for one fly
            if (posData.bodyArea > param_.maxBodyArea)
            {
                // Too big to be single fly
                posData.isMultipleFlies = true;
                cv::Scalar meanPos = cv::mean(maxCompPointMat);
                posData.meanXRel = meanPos.val[0];
                posData.meanYRel = meanPos.val[1];
                posData.meanXAbs = posData.meanXRel + posData.segmentData.blobData.boundingRect.x;
                posData.meanYAbs = posData.meanYRel + posData.segmentData.blobData.boundingRect.y;
                fitterData.positionDataList.push_back(posData);
                posData.success = false;
                continue;
            }
            else 
            {
                posData.isMultipleFlies = false;
            }

            // Convert 2D Nx1 Point2i matrix to Nx2 samples matrix - precursor to computing covariance
            cv::Mat_<double> samplesXY = cv::Mat_<double>(2,maxCompPointMat.rows);
            for (int i=0; i<maxCompPointMat.rows; i++)
            {
                cv::Point2i p = maxCompPointMat.at<cv::Point2i>(i);
                samplesXY.at<double>(0,i) = double(p.x);
                samplesXY.at<double>(1,i) = double(p.y);
            }

            // Compute covariance matrix and mean values of x and y coordinates
            cv::Mat covMat; 
            cv::Mat meanMat;
            int covarFlags = CV_COVAR_NORMAL | CV_COVAR_SCALE | CV_COVAR_COLS;
            cv::calcCovarMatrix(samplesXY,covMat,meanMat,covarFlags);
            posData.meanXRel = meanMat.at<double>(0,0);
            posData.meanYRel = meanMat.at<double>(1,0);
            posData.meanXAbs = posData.meanXRel + posData.segmentData.blobData.boundingRect.x;
            posData.meanYAbs = posData.meanYRel + posData.segmentData.blobData.boundingRect.y;
            posData.covarianceMatrix = covMat;

            // Fit ellipse using covariance matrix 
            cv::Mat eigenVal; 
            cv::Mat eigenVec;
            cv::eigen(covMat, eigenVal, eigenVec);
            posData.ellipseMajorAxis = 2.0*std::sqrt(eigenVal.at<double>(0,0));
            posData.ellipseMinorAxis = 2.0*std::sqrt(eigenVal.at<double>(1,0));
            posData.ellipseAngle = std::atan2(eigenVec.at<double>(0,1),eigenVec.at<double>(0,0));

            // Rotate fly image using affine transform
            double angleTemp = std::fmod(posData.ellipseAngle + 0.5*M_PI,M_PI) - 0.5*M_PI;
            double rotAngDeg = (angleTemp + 0.5*M_PI)*180.0/M_PI;
            //double rotAngDeg = (posData.ellipseAngle + 0.5*M_PI)*180.0/M_PI;
            cv::Point2f rotCenter = cv::Point2f(posData.meanXRel, posData.meanYRel);
            cv::Mat rotMat = cv::getRotationMatrix2D(rotCenter, rotAngDeg, 1.0);

            double shiftX = -rotCenter.x + posData.ellipseMinorAxis + param_.padBorder;
            double shiftY = -rotCenter.y + posData.ellipseMajorAxis + param_.padBorder;
            rotMat.at<double>(0,2) = rotMat.at<double>(0,2) + shiftX;
            rotMat.at<double>(1,2) = rotMat.at<double>(1,2) + shiftY;

            cv::Size imageSize =cv::Size(
                    2*(posData.ellipseMinorAxis + param_.padBorder),
                    2*(posData.ellipseMajorAxis + param_.padBorder)
                    );
           
            int imageType = posData.segmentData.boundingImageLUV.type();
            cv::Mat rotBoundingImageLUV = cv::Mat(imageSize,imageType,param_.fillValuesLUV);
            cv::warpAffine(
                    posData.segmentData.boundingImageLUV,
                    rotBoundingImageLUV,
                    rotMat,
                    imageSize,
                    cv::INTER_LINEAR,
                    cv::BORDER_TRANSPARENT
                    );

            // Get pixel feature vector use to classify orientation
            posData.pixelFeatureVector = getPixelFeatureVector(rotBoundingImageLUV);

            FastBinaryPredictor orientPred = FastBinaryPredictor(param_.orientClassifier);
            FastBinaryPredictorData<double> orientData = orientPred.predict(posData.pixelFeatureVector);

            // Flip pixel feature vector and rotate LUV bounding image  - if required
            if (orientData.fit < 0.0)
            {
                posData.flipped = true;
                posData.ellipseAngle = modRange(posData.ellipseAngle+M_PI,-M_PI, M_PI);
                cv::Mat rotFlippedBoundingImageLUV;
                cv::flip(rotBoundingImageLUV, rotFlippedBoundingImageLUV, -1);
                posData.pixelFeatureVector = getPixelFeatureVector(rotFlippedBoundingImageLUV);
                posData.rotBoundingImageLUV = rotFlippedBoundingImageLUV;
            }
            else
            {
                posData.flipped = false;
                posData.rotBoundingImageLUV = rotBoundingImageLUV;
            }

            posData.success = true;
            fitterData.positionDataList.push_back(posData); 

            if (writeTrainingData_)
            {
                createTrainingData(frameCount, posData, rotBoundingImageLUV);
            }
           
            // DEBUG - Write pixel feature vector to file
            // ------------------------------------------------------------------------------------
            //if (0) 
            //{

            //    std::ofstream pVecStream;
            //    QStringList nameList;
            //    QString pVecFileName = QString("pVec_frm_%1_cnt_%2.txt").arg(frameCount).arg(cnt);
            //    pVecStream.open(pVecFileName.toStdString());
            //    for (int i=0; i<posData.pixelFeatureVector.size();i++)
            //    {
            //        pVecStream << posData.pixelFeatureVector[i] << std::endl;
            //    }
            //    pVecStream.close();
            //}
            if (showDebugWindow_)
            {
                if (cnt==0)
                {
                    //cv::imshow("hogPosMaxComp", maxCompMat);
                    //cv::imshow("boundingImageLUV", posData.segmentData.boundingImageLUV);
                    cv::imshow("rotBoundingImageLUV", posData.rotBoundingImageLUV);
                }
            }
            // ------------------------------------------------------------------------------------
        }
       
    } // for (it=segementDataList.begin() 

    return fitterData;
}


std::vector<double> HogPositionFitter::getPixelFeatureVector(cv::Mat image)
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    // Get Gradient data, magintude, orientation, etc.
    GradientData gradData = getGradientData(
            image,
            param_.pixelFeatureVector.gradNormRadius,
            param_.pixelFeatureVector.gradNormConst, 
            GRAD_METHOD_SCHARR
            //GRAD_METHOD_SOBEL
            );

    // Get mask filled (due rotation) from true image data
    cv::Mat fillMask = getFillMask(image);

    // Sub-vectors for storing pixel feature vector data
    std::vector<double> meanGradMagVector; // Done
    std::vector<double> histGradMagVector; // Done
    std::vector<double> histGradOriVector; // Done
    std::vector<double> meanColorVector;
    std::vector<double> histColorVector;

    for (int i=0; i<param_.pixelFeatureVector.binParam.size(); i++)
    {
        unsigned int numX = param_.pixelFeatureVector.binParam[i].numX;
        unsigned int numY = param_.pixelFeatureVector.binParam[i].numY;
        double binWidth = double(image.cols)/double(numX);
        double binHeight = double(image.rows)/double(numY);

        // Loop over spacial bins
        for (int indX=0; indX < numX; indX++)
        //for (int indX=numX-1; indX >= 0; indX--)
        {
            for (int indY=0; indY < numY; indY++) 
            //for (int indY=numY-1; indY >=0; indY--) // Reverse order to match matlab
            {
                int x = int(std::round(indX*binWidth));
                int y = int(std::round(indY*binHeight));

                // Create ROI and get views of image and fill mask 
                cv::Rect roiRect = cv::Rect(x,y,binWidth,binHeight);
                cv::Mat subImage = image(roiRect);
                cv::Mat subFillMask = fillMask(roiRect);

                // Find mean of max normalized gradient magnitude
                cv::Mat normGradMag = gradData.normMagMax(roiRect);
                cv::Scalar meanGradMag = cv::mean(normGradMag,subFillMask);
                meanGradMagVector.push_back(meanGradMag.val[0]);

                // Calculate histogram of the normalized gradient magnitude
                std::vector<double> histGradMagSubVector = getHistGradMag(
                        normGradMag,
                        subFillMask
                        );

                histGradMagVector.insert(
                        histGradMagVector.end(), 
                        histGradMagSubVector.begin(), 
                        histGradMagSubVector.end()
                        );

                // Calculate histogram of gradient orientation
                cv::Mat gradOri = gradData.oriOfNormMagMax(roiRect);
                std::vector<double> histGradOriSubVector = getHistGradOri(
                        gradOri, 
                        normGradMag, 
                        subFillMask
                        );

                histGradOriVector.insert(
                        histGradOriVector.end(),
                        histGradOriSubVector.begin(),
                        histGradOriSubVector.end()
                        );

                // Find mean of color values
                cv::Scalar meanColor = cv::mean(subImage,subFillMask);
                for (int k=0; k<subImage.channels(); k++)
                {
                    meanColorVector.push_back(meanColor.val[k]);
                }

                // Calculate histograms of color values.
                std::vector<double> histColorSubVector = getHistColor(
                        subImage,
                        subFillMask
                        );
               
                histColorVector.insert(
                        histColorVector.end(),
                        histColorSubVector.begin(),
                        histColorSubVector.end()
                        );

            } // for (int indY

        }  // for (int indX

    } // for( int i

    // Create pixel feature vector
    std::vector<double> pixelFeatureVector;

    pixelFeatureVector.insert(
            pixelFeatureVector.end(),
            meanGradMagVector.begin(),
            meanGradMagVector.end()
            );

    pixelFeatureVector.insert(
            pixelFeatureVector.end(),
            meanColorVector.begin(),
            meanColorVector.end()
            );

    pixelFeatureVector.insert(
            pixelFeatureVector.end(),
            histColorVector.begin(),
            histColorVector.end()
            );

    pixelFeatureVector.insert(
            pixelFeatureVector.end(),
            histGradMagVector.begin(),
            histGradMagVector.end()
            );

    pixelFeatureVector.insert(
            pixelFeatureVector.end(),
            histGradOriVector.begin(),
            histGradOriVector.end()
            );


    return pixelFeatureVector;
}


std::vector<double> HogPositionFitter::getHistGradOri(
        cv::Mat gradOri, 
        cv::Mat normGradMag, 
        cv::Mat mask
        )
{
    std::vector<float> centVect = param_.pixelFeatureVector.gradOriCentVector;

    // Create histogram bins
    std::vector<float> bins;
    double minValGradOri,  maxValGradOri;
    cv::minMaxLoc(gradOri,&minValGradOri,&maxValGradOri,0,0,mask);
    if (centVect.size() == 1)
    {
        bins.push_back(minValGradOri);
        bins.push_back(centVect[0]);
        bins.push_back(maxValGradOri);
    }
    else
    {
        for (int i=0; i<centVect.size()-1; i++)
        {
            float posLower = centVect[i];
            float posUpper = centVect[i+1];
            float binWidth = std::fabs(posUpper - posLower);
            float binValue;
            if (i ==0 )
            {
                binValue = std::min(posLower - 0.5*binWidth + FLT_EPSILON, minValGradOri);
                bins.push_back(binValue);
            }

            binValue = posLower + 0.5*binWidth + FLT_EPSILON; 
            bins.push_back(binValue);

            if(i == centVect.size()-2)
            {
                binValue =std::max(posUpper + 0.5*binWidth + FLT_EPSILON, maxValGradOri);
                bins.push_back(binValue);
            }
        }
    }

    // Initialize histogram values and compute histogram
    double totalCount = 0.0;
    std::vector<double> histValues;
    for (int i=0; i<bins.size()-1; i++)
    {
        histValues.push_back(0.0);
    }

    for (int i=0; i<gradOri.rows; i++)
    {
        for (int j=0; j<gradOri.cols; j++)
        {
            for (int k=0; k<bins.size()-1; k++)
            {
                float oriValue = gradOri.at<float>(i,j);
                float weight = normGradMag.at<float>(i,j); 
                if ((oriValue >= bins[k]) && (oriValue < bins[k+1]))
                {
                    histValues[k] = double(histValues[k] + weight);
                    totalCount += weight;
                }
            } 
        }
    }

    // Normalize histogram values
    for (int i=0; i<histValues.size(); i++)
    {
        histValues[i] = histValues[i]/totalCount;
    }

    return histValues;
}


std::vector<double> HogPositionFitter::getHistGradMag(cv::Mat normGradMag, cv::Mat mask)
{
    cv::Mat histNormGradMagMat;
    std::vector<double> histNormGradMagVec;

    // Parameters for creating the normalized gradient magnitude histogram - used in calcHist 
    int histNumImages = 1;
    int histChannels[] = {0};
    int histNumDim = 1;
    int histSize[] = {int(param_.pixelFeatureVector.gradMagEdgeVector.size())-1};
    float *histEdgeArray = &param_.pixelFeatureVector.gradMagEdgeVector[0];
    const float *histRanges[] = {histEdgeArray};
    bool histUniform = false;
    bool histAccumulate = false;

    // Calculate histogram for normalized gradient magnitudes
    cv::calcHist(
            &normGradMag,
            histNumImages,
            histChannels,
            mask,
            histNormGradMagMat,
            histNumDim,
            histSize,
            histRanges,
            histUniform,
            histAccumulate
            );

    // Normalize histogram 
    double histSum = cv::sum(histNormGradMagMat)[0];
    for (int i=0; i<histNormGradMagMat.rows; i++)
    {
        double frac = histNormGradMagMat.at<float>(i,0)/histSum;
        histNormGradMagVec.push_back(frac);
    }
    return histNormGradMagVec;
}


std::vector<double> HogPositionFitter::getHistColor(cv::Mat subImage, cv::Mat mask)
{
    cv::vector<cv::Scalar> colorEdgeVector = param_.pixelFeatureVector.colorEdgeVector;
    cv::vector<double> histColorVec;

    for (int k=0; k<subImage.channels(); k++)
    {
        cv::Mat histColorMat;

        // Parameters for createing color histogram
        int histNumImages = 1;
        int histChannels[] = {k};
        int histNumDim = 1;
        int histSize[] = {int(colorEdgeVector.size())-1};
        std::vector<float> histEdgeVector;
        for (int i=0; i<colorEdgeVector.size(); i++)
        {
            histEdgeVector.push_back(colorEdgeVector[i].val[k]);
        }
        const float *histRanges[] = {&histEdgeVector[0]};
        bool histUniform = false;
        bool histAccumulate = false;

        // Calculate histogram
        cv::calcHist(
                &subImage,
                histNumImages,
                histChannels,
                mask,
                histColorMat,
                histNumDim,
                histSize,
                histRanges,
                histUniform,
                histAccumulate
                );

        // Normalize histogram and add to histogram vector
        double histSum = cv::sum(histColorMat)[0];
        for (int i=0; i<histColorMat.rows; i++)
        {
            double frac = histColorMat.at<float>(i,0)/histSum;
            histColorVec.push_back(frac);
        }
    }
    return histColorVec;
}


cv::Mat HogPositionFitter::getFillMask(cv::Mat image)
{
    // Create mask which is 0 on filled in values (due to rotation)
    cv::Mat fillMask = cv::Mat(image.size(), CV_8UC1, cv::Scalar(255));

    for (int i=0; i<image.rows; i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            cv::Vec3f pixVec = image.at<cv::Vec3f>(i,j);

            bool isEqual = true; 
            for (int k=0; k<3; k++)
            {
                float val = pixVec[k];
                float fillVal = param_.fillValuesLUV.val[k]; 

                if (val != fillVal)
                { 
                    isEqual = false;
                }
            }
            if (isEqual)
            {
                fillMask.at<uchar>(i,j) = 0;
            }
        }
    }

    // Erode mask slightly to get rid of any gradient at fill bndry 
    unsigned int elemDiam = 2*param_.pixelFeatureVector.fillBndryErodeRadius+1;
    cv::Size elemSize = cv::Size(elemDiam,elemDiam);
    cv::Mat structElem = cv::getStructuringElement(cv::MORPH_ELLIPSE, elemSize);
    cv::erode(fillMask,fillMask,structElem);

    return fillMask;
}


GradientData getGradientData(
        cv::Mat image, 
        unsigned int normRadius, 
        double normConst, 
        GradientMethod method
        )
{
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    
    GradientData gradData;

    // Find image gradient 
    double scale = 1.0;
    double delta = 0.0;

    if (method == GRAD_METHOD_SCHARR)
    {
        scale = 1.0/32.0;
        cv::Scharr(image,gradData.dx,CV_32F,1,0,scale,delta,cv::BORDER_REPLICATE);
        cv::Scharr(image,gradData.dy,CV_32F,0,1,scale,delta,cv::BORDER_REPLICATE);
    }
    else 
    {
        int ksize = 3;
        scale = 1.0/8.0;
        cv::Sobel(image,gradData.dx,CV_32F,1,0,ksize,scale,delta,cv::BORDER_REPLICATE);
        cv::Sobel(image,gradData.dy,CV_32F,0,1,ksize,scale,delta,cv::BORDER_REPLICATE);
    } 

    // Find magnitude and orientation of gradient
    cv::cartToPolar(gradData.dx, gradData.dy, gradData.mag, gradData.ori,false);


    // Reduce orientation so that is is modulo pi 
    for (int i=0; i<gradData.ori.rows; i++) 
    {
        for (int j=0; j<gradData.ori.cols; j++)
        {
            cv::Vec3f oriVec = gradData.ori.at<cv::Vec3f>(i,j);
            cv::Vec3f oriVecMod;

            for (int k=0; k<3; k++)
            {
                oriVecMod[k] = std::fmod(oriVec[k],M_PI);
            }
            gradData.ori.at<cv::Vec3f>(i,j) = oriVecMod;
        }
    }

    // Find normalized maginitude and gradient. 
    cv::Mat smoothMag;
    if (normRadius == 0)
    {
        smoothMag = gradData.mag;
    }
    else
    {
        cv::Mat triFilter = getTriangleFilter2D(normRadius);
        cv::filter2D(
                gradData.mag, 
                smoothMag,
                -1,
                triFilter,
                cv::Point(-1,-1),
                0.0,
                cv::BORDER_REPLICATE
                );

    }

    //gradData.normMag = gradData.mag/(smoothMag+normConst);
    gradData.normMag = cv::Mat(gradData.mag.size(),CV_32FC3,cv::Scalar(0.0));


    // Loop over image  get maxium of gradient magnitudes (w.r.t. color) for each pixel.  
    // Also get the orientation of the gradient with the largest magniude. .  
    gradData.normMagMax = cv::Mat(gradData.normMag.size(), CV_32FC1, cv::Scalar(0.0));
    gradData.oriOfNormMagMax = cv::Mat(gradData.ori.size(), CV_32FC1, cv::Scalar(0.0));
    for (int i=0; i<image.rows; i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            cv::Vec3f magVec = gradData.mag.at<cv::Vec3f>(i,j);
            cv::Vec3f smoothMagVec = smoothMag.at<cv::Vec3f>(i,j);
            cv::Vec3f normMagVec = gradData.normMag.at<cv::Vec3f>(i,j);
            cv::Vec3f oriVec = gradData.ori.at<cv::Vec3f>(i,j);

            double normMagMax = 0.0;
            double oriOfNormMagMax = 0.0;
            for (int k=0; k<3; k++)
            {
                normMagVec[k] = magVec[k]/(smoothMagVec[k] + normConst);
                if (normMagVec[k] >= normMagMax)
                {
                    normMagMax = normMagVec[k];
                    oriOfNormMagMax = oriVec[k];
                }
            }
            gradData.normMag.at<cv::Vec3f>(i,j) = normMagVec;
            gradData.normMagMax.at<float>(i,j) = normMagMax;
            gradData.oriOfNormMagMax.at<float>(i,j) = oriOfNormMagMax; 
        }
    }

    // DEBUG
    // --------------------------------------------------------------------------
    static int writeCnt = 0;
    if ( (0) && (writeCnt < 1))
    {
        for (int k=0; k<3; k++)
        {
            std::ofstream magStream;
            std::ofstream smoStream;
            std::ofstream imgStream;
            std::ofstream nrmStream;
            std::ofstream maxStream;

            QString magFileName = QString("mag_%1_%2.dat").arg(k).arg(writeCnt);
            QString smoFileName = QString("smo_%1_%2.dat").arg(k).arg(writeCnt);
            QString imgFileName = QString("img_%1_%2.dat").arg(k).arg(writeCnt);
            QString nrmFileName = QString("nrm_%1_%2.dat").arg(k).arg(writeCnt);
            magStream.open(magFileName.toStdString());
            smoStream.open(smoFileName.toStdString());
            imgStream.open(imgFileName.toStdString());
            nrmStream.open(nrmFileName.toStdString());

            if (k==0)
            {
                QString maxFileName = QString("max_%1.dat").arg(writeCnt);
                maxStream.open(maxFileName.toStdString());
            }

            for (int i=0; i<gradData.mag.rows; i++)
            {
                for (int j=0; j<gradData.mag.cols; j++)
                {
                    cv::Vec3f magVec = gradData.mag.at<cv::Vec3f>(i,j);
                    cv::Vec3f smoVec = smoothMag.at<cv::Vec3f>(i,j);
                    cv::Vec3f imgVec = image.at<cv::Vec3f>(i,j);
                    cv::Vec3f nrmVec = gradData.normMag.at<cv::Vec3f>(i,j);
                    magStream << magVec[k] << " ";
                    smoStream << smoVec[k] << " ";
                    imgStream << imgVec[k] << " ";
                    nrmStream << nrmVec[k] << " ";
                    if (k==0)
                    {
                        float maxVec = gradData.normMagMax.at<float>(i,j);
                        maxStream << maxVec  << " ";
                    }
                }
                magStream << std::endl;
                smoStream << std::endl;
                imgStream << std::endl;
                nrmStream << std::endl;
                if  (k==0)
                {
                    maxStream << std::endl;
                }
            }

            magStream.close();
            smoStream.close();
            imgStream.close();
            nrmStream.close();
            if (k==0) 
            {
                maxStream.close();
            }
        }
        writeCnt++;
    }
    // --------------------------------------------------------------------------
    return gradData;
}


void HogPositionFitter::createTrainingData(
        unsigned long frameCount,
        PositionData posData,
        cv::Mat img
        )
{
    // Create base file name
    std::stringstream baseNameStream;
    baseNameStream << trainingFileNamePrefix_;
    baseNameStream << "_frame_" << (frameCount+1);
    baseNameStream << "_posx_"  << int(posData.meanXAbs);
    baseNameStream << "_posy_"  << int(posData.meanYAbs);
    baseNameStream << "_id_"    << posData.segmentData.blobData.id;
    std::string baseName = baseNameStream.str();

    // Write unflipped pixel feature vector
    std::stringstream fileNameStream;
    fileNameStream << baseName << "_flipxy_00.txt";
    std::vector<double> vector = getPixelFeatureVector(img);
    writePixelFeatureVector(fileNameStream.str(), vector);

    // Write pixel feature vector from image flipped about x
    fileNameStream.str(std::string());
    fileNameStream << baseName << "_flipxy_10.txt";
    cv::Mat imgFlipX;
    cv::flip(img, imgFlipX, 0);
    std::vector<double> vectorFlipX = getPixelFeatureVector(imgFlipX);
    writePixelFeatureVector(fileNameStream.str(), vectorFlipX);

    // Write pixel feature vector from image flipped about y
    fileNameStream.str(std::string());
    fileNameStream << baseName << "_flipxy_01.txt";
    cv::Mat imgFlipY;
    cv::flip(img, imgFlipY, 1);
    std::vector<double> vectorFlipY = getPixelFeatureVector(imgFlipY);
    writePixelFeatureVector(fileNameStream.str(), vectorFlipY);

    // Write pixel feature vector from image flipped about x and y
    fileNameStream.str(std::string());
    fileNameStream << baseName << "_flipxy_11.txt";
    cv::Mat imgFlipXY;
    cv::flip(img, imgFlipXY, -1);
    std::vector<double> vectorFlipXY = getPixelFeatureVector(imgFlipXY);
    writePixelFeatureVector(fileNameStream.str(), vectorFlipXY);

    // Create 
    fileNameStream.str(std::string());
    fileNameStream << baseName << "_image.png";
    cv::Mat imgScaled;
    img.convertTo(imgScaled, CV_16UC3, 65535);
    cv::imwrite(fileNameStream.str(), imgScaled);

}


void HogPositionFitter::writePixelFeatureVector(
        std::string fileName, 
        std::vector<double> pixVector
        )
{
    std::ofstream outStream; 
    int numElements = pixVector.size();
    outStream.open(fileName);
    outStream << std::scientific;
    outStream << std::setprecision(10);
    outStream << 1 << ",";
    outStream << numElements << ",";
    for (int i=0; i<numElements; i++)
    {
        outStream << pixVector[i];
        if (i < numElements)
        {
            outStream << ","; 
        }
    }
    outStream.close();
}



cv::Mat getTriangleFilter1D(unsigned int normRadius)
{ 
    float normConst = std::pow(float(normRadius)+1,2);
    cv::Mat f = cv::Mat(1,2*normRadius+1,CV_32FC1);

    for (unsigned int i=0; i<normRadius+1; i++)
    {
        f.at<float>(0,i) = float(i+1)/normConst;
    }
    for (unsigned int i=normRadius+1; i<2*normRadius+1;i++)
    {
        f.at<float>(0,i) = float(2*normRadius+1-i)/normConst;
    }
    return f;
}

cv::Mat getTriangleFilter2D(unsigned int normRadius)
{
    cv::Mat f1D = getTriangleFilter1D(normRadius);
    cv::Mat f2D = f1D.t()*f1D;
    return f2D;
}

double modRange(double input, double lower, double upper)
{
    return std::fmod(input - lower, upper - lower) + lower;
}
