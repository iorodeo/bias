#include "blob_finder.hpp"
#include "camera_facade.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace bias
{

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
        cv::Mat image8UC1;

        // Convert image to 8UC1 - for use with find contours
        if ((image.channels() > 1) || (image.depth() != CV_8U))
        {
            image8UC1 = cv::Mat(image.size(), CV_8UC1, cv::Scalar(0));
            cvtColor(image,image8UC1,CV_BGR2GRAY);
        }
        else
        {
            image8UC1 = image;
        }

        // Threshold and find contours
        cv::threshold(
                image8UC1,
                data.thresholdImage, 
                param_.thresholdLax, 
                param_.thresholdMaxVal, 
                CV_THRESH_BINARY
                );
        data.thresholdImage = param_.thresholdMaxVal - data.thresholdImage;
        cv::Mat imageTemp = data.thresholdImage.clone();
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(imageTemp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // Filter blob data and draw contours on image
        data.blobDataImage = cv::Mat(image8UC1.size(), CV_8UC3, cv::Scalar(0,0,0));
        cvtColor(image8UC1,data.blobDataImage,CV_GRAY2BGR);

        if (image.channels() > 1)
        {
            data.blobDataImage = cv::Mat(image);
        }
        else
        {
            data.blobDataImage = cv::Mat(image8UC1.size(), CV_8UC3, cv::Scalar(0,0,0));
            cvtColor(image8UC1,data.blobDataImage,CV_GRAY2BGR);
        }


        for (size_t index=0; index < contours.size(); index++)
        {
            cv::Moments contourMoments = cv::moments(contours[index]);
            BlobData blobData = BlobData(contours[index],image);
            if (withinAreaBounds(blobData) && containsStrictThreshold(blobData,image8UC1))
            { 
                data.blobDataList.push_back(blobData); 
                blobData.draw(data.blobDataImage);
            }
        }

        data.success = true;
        return data;
    }


    bool BlobFinder::withinAreaBounds(BlobData blobData)
    { 
        bool  lessThanMax = blobData.area <= param_.maximumArea;
        bool  greaterThanMin = blobData.area >= param_.minimumArea;
        return  greaterThanMin && lessThanMax;
    }


    bool BlobFinder::containsStrictThreshold(BlobData blobData, cv::Mat image)
    { 
        int xMin = blobData.boundingRect.x;
        int yMin = blobData.boundingRect.y; 
        int xMax = blobData.boundingRect.x + blobData.boundingRect.width;
        int yMax = blobData.boundingRect.y + blobData.boundingRect.height;

        bool containsUpperThresh = false;
        for (int i=xMin; i<xMax; i++)
        { 
            for (int j=yMin; j<yMax; j++)
            {
                if (int(image.at<uchar>(j,i)) <= param_.thresholdStrict)
                {
                    float testResult = cv::pointPolygonTest(
                            blobData.contourVector,
                            cv::Point2f(i,j), 
                            false
                            );
                    if ((testResult == 1) || (testResult == 0))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }


} // namespace bias

