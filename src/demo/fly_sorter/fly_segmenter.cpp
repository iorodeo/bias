#include "fly_segmenter.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// FlySegmenter
// ----------------------------------------------------------------------------
FlySegmenter::FlySegmenter()
{
}

FlySegmenter::FlySegmenter(FlySegmenterParam param)
{
    setParam(param);
    showDebugWindow_ = false;
    if (showDebugWindow_) 
    {
        cv::namedWindow(
                "FlySegmenter",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
    }
}

void FlySegmenter::setParam(FlySegmenterParam param)
{
    param_ = param;
    fastBinaryPredictor_.setClassifierParam(param_.classifier);
}

FlySegmenterData FlySegmenter::segment(BlobFinderData blobFinderData)
{
    FlySegmenterData flySegmenterData;
    BlobDataList blobDataList = blobFinderData.blobDataList;
    BlobDataList::iterator it;
    unsigned int cnt;

    for (it=blobDataList.begin(), cnt=0; it!=blobDataList.end(); it++, cnt++)
    {
        //std::cout << "blobData" << std::endl;

        BlobData blobData = *it;

        // Convert bounding image to LUV. Note, as I'm developing 
        // w/ mono camera may need ot convert image to BGR image. 
        // --------------------------------------------------------
        cv::Mat boundingImageBGR;
        if (blobData.boundingImage.type() != CV_8UC3)
        {
            boundingImageBGR = cv::Mat(
                    blobData.boundingImage.size(),
                    CV_8UC3,
                    cv::Scalar(0,0,0)
                    );

            cv::cvtColor(
                    blobData.boundingImage,
                    boundingImageBGR,
                    CV_GRAY2BGR
                    );
        }
        else
        {
            boundingImageBGR = blobData.boundingImage;
        }
        cv::Mat boundingImageLUV = cv::Mat(
                boundingImageBGR.size(), 
                boundingImageBGR.type(),
                cv::Scalar(0,0,0)
                );
        cv::cvtColor(boundingImageBGR,boundingImageLUV,CV_BGR2Luv);

        // Segment using fast binary predict.
        // --------------------------------------------------------
        FastBinaryPredictorData predictorData;
        predictorData = fastBinaryPredictor_.predict(boundingImageLUV);

        SegmentData segmentData;
        segmentData.blobData = blobData;
        segmentData.predictorData = predictorData;
        segmentData.boundingImageLUV = boundingImageLUV;
        // DEVELOP TEMPORARY 
        // --------------------------------------------------------
        if (true)
        {
            cv::Mat bwImage = cv::Mat(boundingImageBGR.size(),CV_8U);
            cv::Mat threshImage = cv::Mat(boundingImageBGR.size(),CV_8U);
            cv::cvtColor(boundingImageBGR,bwImage,CV_BGR2GRAY);
            cv::threshold( bwImage, threshImage, 100, 255, CV_THRESH_BINARY);
            segmentData.predictorData.label = 255 - threshImage;

            //cv::Mat testImage = cv::Mat(boundingImageBGR.size(),CV_8U,cv::Scalar(0));
            //segmentData.predictorData.label = testImage; 
        }
        // --------------------------------------------------------
        flySegmenterData.segmentDataList.push_back(segmentData);
        if (showDebugWindow_)
        {
            if (cnt==0)
            {
                //cv::imshow("FlySegmenter", segmentData.predictorData.label);
                cv::imshow("FlySegmenter", boundingImageLUV);
            }
        }
        
    }
    return flySegmenterData;
}
