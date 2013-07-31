#include "fly_segmenter.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

FlySegmenter::FlySegmenter()
{
}

FlySegmenter::FlySegmenter(FlySegmenterParam param)
{
    setParam(param);
}

void FlySegmenter::setParam(FlySegmenterParam param)
{
    param_ = param;
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

        // Segment using fast binary predict.
        // --------------------------------------------------------
        //int numChan = 3;
        //int numRows = 1;
        //cv::Mat boudningImageLUV1D = boundingImageLUV.reshape(numChan,numRows);

    }
}
