#include "blob_finder.hpp"
#include "camera_facade.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// BlobFinderData
// ----------------------------------------------------------------------------

BlobFinderData::BlobFinderData()
{
    success = false;
}



// BlobFinder
// ----------------------------------------------------------------------------

BlobFinder::BlobFinder() {}


BlobFinder::BlobFinder(BlobFinderParam param)
{
    setParam(param);
}

void BlobFinder::setParam(BlobFinderParam param)
{
    param_ = param;
}


BlobFinderData BlobFinder::findBlobs(cv::Mat image)
{
    BlobFinderData data;
    std::vector<std::vector<cv::Point>> contours;

    cv::threshold(
            image,
            data.thresholdImage, 
            param_.threshold, 
            param_.thresholdMaxVal, 
            CV_THRESH_BINARY
            );

    data.thresholdImage = param_.thresholdMaxVal - data.thresholdImage;
    cv::Mat imageTemp = data.thresholdImage.clone();
    cv::findContours(imageTemp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    data.blobDataImage = cv::Mat(image.size(), CV_8UC3, cv::Scalar(0,0,0));
    cvtColor(image,data.blobDataImage,CV_GRAY2BGR);

    for (size_t index=0; index < contours.size(); index++)
    {
        cv::Moments contourMoments = cv::moments(contours[index]);
        BlobData blobData = BlobData(contours[index]);
        if ((blobData.area >= param_.minimumArea) && (blobData.area <= param_.maximumArea))
        {
            data.blobDataList.push_back(blobData);
            blobData.draw(data.blobDataImage);
        }
    }

    data.success = true;
    return data;
}

