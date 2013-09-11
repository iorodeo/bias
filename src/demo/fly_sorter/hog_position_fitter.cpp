#include "hog_position_fitter.hpp"
#include "basic_image_proc.hpp"
#include "fast_binary_predictor.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
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
    success = false;
    bodyArea = 0; 
    meanX = 0.0;
    meanY = 0.0;
    ellipseMajorAxis = 0.0;
    ellipseMinorAxis = 0.0;
    ellipseAngle = 0.0;
    covarianceMatrix = cv::Mat(2,2,CV_64FC1,cv::Scalar(0.0));
}

// HogPositionFitterData
// ----------------------------------------------------------------------------
HogPositionFitterData::HogPositionFitterData() {};

//  HogPositionFitter
//  ---------------------------------------------------------------------------

HogPositionFitter::HogPositionFitter() {};

HogPositionFitter::HogPositionFitter(HogPositionFitterParam param)
{
    setParam(param);
    showDebugWindow_ = false;
    if (showDebugWindow_)
    {
        //cv::namedWindow(
        //        "hogPosLabel",
        //        CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
        //        );
        //cv::namedWindow(
        //        "hogPosClose",
        //        CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
        //        );
        //cv::namedWindow(
        //        "hogPosIsBody",
        //        CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
        //        );
        cv::namedWindow(
                "hogPosMaxComp",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
        cv::namedWindow(
                "boundingImageLUV",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
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


HogPositionFitterData HogPositionFitter::fit(FlySegmenterData flySegmenterData)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    HogPositionFitterData fitterData;
    SegmentDataList segmentDataList = flySegmenterData.segmentDataList;
    SegmentDataList::iterator it;
    unsigned int cnt;

    for (it=segmentDataList.begin(), cnt=0; it!=segmentDataList.end(); it++, cnt++)
    {
        SegmentData segmentData = *it;
        PositionData posData;
        std::cout << "  processing segment " << cnt << " data " << std::endl;

        // Detect Body pixels 
        cv::Mat closeMat = imCloseWithDiskElem(
                segmentData.predictorData.label,
                param_.closeRadius
                );

        cv::Mat isBodyMat = bwAreaOpen(closeMat,param_.openArea);
        posData.bodyArea = cv::countNonZero(isBodyMat);

        // Ensure that bodyArea is above minimum for fly
        if (posData.bodyArea < param_.openArea)
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

            // Find pixel nonzero pixel locations of maximum connected component.
            cv::Mat maxCompPointMat;
            cv::findNonZero(maxCompMat, maxCompPointMat);

            // Recompute body area for maximum connected component. /// Note, double check this w/ Kristin ///
            posData.bodyArea = cv::countNonZero(maxCompMat);

            // Check if area is too big for one fly
            if (posData.bodyArea > param_.maxBodyArea)
            {
                // Too big to be single fly
                posData.isMultipleFlies = true;
                cv::Scalar meanPos = cv::mean(maxCompPointMat);
                posData.meanX = meanPos.val[0];
                posData.meanY = meanPos.val[1];
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
            posData.meanX = meanMat.at<double>(0,0);
            posData.meanY = meanMat.at<double>(1,0);
            posData.covarianceMatrix = covMat;

            // Fit ellipse using covariance matrix 
            cv::Mat eigenVal; 
            cv::Mat eigenVec;
            cv::eigen(covMat, eigenVal, eigenVec);
            posData.ellipseMajorAxis = 2.0*std::sqrt(eigenVal.at<double>(0,0));
            posData.ellipseMinorAxis = 2.0*std::sqrt(eigenVal.at<double>(1,0));
            posData.ellipseAngle = std::atan2(eigenVec.at<double>(0,1),eigenVec.at<double>(0,0));
            posData.ellipseAngle = std::fmod(posData.ellipseAngle + 0.5*M_PI,M_PI) - 0.5*M_PI;

             
            // Rotate fly image using affine transform
            double rotAngDeg = (posData.ellipseAngle + 0.5*M_PI)*180.0/M_PI;
            cv::Point2f rotCenter = cv::Point2f(posData.meanX, posData.meanY);
            cv::Mat rotMat = cv::getRotationMatrix2D(rotCenter, rotAngDeg, 1.0);

            double shiftX = -rotCenter.x + posData.ellipseMinorAxis + param_.padBorder;
            double shiftY = -rotCenter.y + posData.ellipseMajorAxis + param_.padBorder;
            rotMat.at<double>(0,2) = rotMat.at<double>(0,2) + shiftX;
            rotMat.at<double>(1,2) = rotMat.at<double>(1,2) + shiftY;

            cv::Size imageSize =cv::Size(
                    2*(posData.ellipseMinorAxis + param_.padBorder),
                    2*(posData.ellipseMajorAxis + param_.padBorder)
                    );
           
            int imageType = segmentData.boundingImageLUV.type();
            cv::Mat rotBoundingImageLUV = cv::Mat(imageSize,imageType);
            cv::warpAffine(
                    segmentData.boundingImageLUV,
                    rotBoundingImageLUV,
                    rotMat,
                    imageSize,
                    cv::INTER_LINEAR,
                    cv::BORDER_CONSTANT,
                    param_.fillValuesLUV*255.0
                    );

            // Get pixel feature vector
            cv::Mat pixelFeatureVector = getPixelFeatureVector(rotBoundingImageLUV);

            // Classify orientation
            //FastBinaryPredictor predictory = FastBinaryPredictor();
            //FastBinaryPredictorData predictorData = F


            // Flip pixel feature vector and rotated LUV bounding image if required.




            posData.success = true;

            // Temporary
            // ---------------------------------------------------------------------
            if (showDebugWindow_)
            {
                if (cnt==0)
                {
                    //cv::imshow("hogPosLabel", segmentData.predictorData.label);
                    //cv::imshow("hogPosClose", closeMat);
                    //cv::imshow("hogPosIsBody", isBodyMat);
                    cv::imshow("hogPosMaxComp", maxCompMat);
                    cv::imshow("boundingImageLUV", segmentData.boundingImageLUV);
                    cv::imshow("rotBoundingImageLUV", rotBoundingImageLUV);
                }
            }
            // ----------------------------------------------------------------------
        }



        std::cout << "    bodyArea:       " << posData.bodyArea << std::endl;
        std::cout << "    isBodyMat.cols: " << isBodyMat.cols << std::endl;
        std::cout << "    isBodyMat.rows: " << isBodyMat.rows << std::endl;
        std::cout << "    isFly:          " << posData.isFly << std::endl;
       
    } // for (it=segementDataList.begin() 

    return fitterData;

}


cv::Mat HogPositionFitter::getPixelFeatureVector(cv::Mat image)
{
    cv::Mat pixelFeatureVector;
    return pixelFeatureVector;
}





