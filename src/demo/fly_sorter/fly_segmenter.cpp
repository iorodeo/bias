#include "fly_segmenter.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "bgr_to_luv_converter.hpp"


// FlySegmenter
// ----------------------------------------------------------------------------
FlySegmenter::FlySegmenter() {}

FlySegmenter::FlySegmenter(FlySegmenterParam param)
{
    setParam(param);
    showDebugWindow_ = true; 
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

        BlobData blobData = *it;

        // Convert bounding image to LUV. Note, as I'm developing 
        // w/ mono camera may need ot convert image to BGR image. 
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

        // Convert BGR image to LUV image - use custom converter instead of
        // cvtColor as Matlab training used Piotr Dollar's rgbConvert which
        // gives different results than cvtColor.
        cv::Mat boundingImageLUV = BgrToLuvConverter::convert(boundingImageBGR);

        // Segment using fast binary predict.
        FastBinaryPredictorData<cv::Mat> predictorData;
        predictorData = fastBinaryPredictor_.predict(boundingImageLUV);

        SegmentData segmentData;
        segmentData.blobData = blobData;
        segmentData.predictorData = predictorData;
        segmentData.boundingImageLUV = boundingImageLUV;
        flySegmenterData.segmentDataList.push_back(segmentData);

        // DEVELOP TEMPORARY    
        // --------------------------------------------------------
        if (showDebugWindow_)
        {
            if (cnt==0)
            {
                cv::imshow("FlySegmenter", segmentData.predictorData.label);
                //cv::imshow("FlySegmenter", boundingImageLUV);
            }
        }
        // ---------------------------------------------------------
        
    }
    return flySegmenterData;
}
