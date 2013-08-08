#include "fly_segmenter.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// FlySegmenterData
// ----------------------------------------------------------------------------

FlySegmenterData::FlySegmenterData() {};

void FlySegmenterData::setPredictorData(FastBinaryPredictorData predictorData)
{
    fit = predictorData.fit;
    label = predictorData.label;
}


// FlySegmenter
// ----------------------------------------------------------------------------
FlySegmenter::FlySegmenter()
{
}

FlySegmenter::FlySegmenter(FlySegmenterParam param)
{
    setParam(param);

    //// Develop
    //// ----------------------------------------------------------
    //cv::namedWindow(
    //        "segmenter",
    //        CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
    //        );
    //// ----------------------------------------------------------
}

void FlySegmenter::setParam(FlySegmenterParam param)
{
    param_ = param;
    fastBinaryPredictor_.setClassifierParam(param_.classifier);
}

FlySegmenterData FlySegmenter::segment(BlobDataList blobDataList)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    BlobDataList::iterator it;

    for ( it=blobDataList.begin(); it!=blobDataList.end(); it++)
    {
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

        //// Segment using fast binary predict.
        //// --------------------------------------------------------
        FastBinaryPredictorData predictorData;
        predictorData = fastBinaryPredictor_.predict(boundingImageLUV);

        FlySegmenterData segmenterData;
        //segmenterData.setPredictorData(predictorData);

        //// Develop
        //// ---------------------------------------------------------
        //cv::imshow("segmenter", segmenterData.label);
        //// ---------------------------------------------------------
        
        return segmenterData;
    }
}
