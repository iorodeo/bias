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

        // Convert image to 8UC1 - for use with find contours
        cv::Mat image8UC1;
        if (image.channels() > 1)
        {
            image8UC1 = cv::Mat(image.size(), CV_8UC1, cv::Scalar(0));
            cvtColor(image,image8UC1,CV_BGR2GRAY);
        }
        else
        {
            image8UC1 = image;
        }

        //// Perform simple morphological image reconstruction
        //cv::Mat reconImage = reconstruct(
        //        image8UC1,
        //        param_.thresholdUpper, 
        //        param_.thresholdLower, 
        //        param_.thresholdMaxVal
        //        );

        //data.thresholdImage = reconImage;

        // Threshold and find contours
        cv::threshold(
                image8UC1,
                data.thresholdImage, 
                param_.threshold, 
                param_.thresholdMaxVal, 
                CV_THRESH_BINARY
                );
        data.thresholdImage = param_.thresholdMaxVal - data.thresholdImage;
        
        cv::Mat imageTemp = data.thresholdImage.clone();
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(imageTemp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // Draw contours on image
        data.blobDataImage = cv::Mat(image8UC1.size(), CV_8UC3, cv::Scalar(0,0,0));
        cvtColor(image8UC1,data.blobDataImage,CV_GRAY2BGR);
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

    // Utility functions
    // ----------------------------------------------------------------------------
    cv::Mat reconstruct(
            cv::Mat image, 
            double thresholdUpper, 
            double thresholdLower, 
            double thresholdMaxVal
            )
    { 
        // Get upper and lower threshold images
        cv::Mat imageUpper;
        cv::threshold(
                image, 
                imageUpper,
                thresholdUpper,
                thresholdMaxVal,
                CV_THRESH_BINARY
                );
        imageUpper = thresholdMaxVal - imageUpper;

        cv::Mat imageLower;
        cv::threshold(
                image,
                imageLower,
                thresholdLower,
                thresholdMaxVal,
                CV_THRESH_BINARY
                ); 
        imageLower = thresholdMaxVal - imageLower;

        // Finder contours based on upper thresholds
        std::vector<std::vector<cv::Point>> contoursUpper;
        cv::Mat scratchImageUpper = imageUpper.clone();
        cv::findContours(
                scratchImageUpper, 
                contoursUpper, 
                CV_RETR_EXTERNAL, 
                CV_CHAIN_APPROX_NONE
                );


        // Create reconstructed image
        cv::Mat reconImage = cv::Mat(image.size(), CV_8UC1, cv::Scalar(0));
        cv::Size imageSize = image.size();
        cv::Size maskSize(imageSize.width + 2, imageSize.height + 2);
        cv::Mat imageMask = cv::Mat(maskSize, CV_8UC1, cv::Scalar(0));
        cv::Rect roi(1,1,imageSize.width+1, imageSize.height+1);
        //imageLower.copyTo(imageMask(roi));

        for (size_t index=0; index < contoursUpper.size(); index++)
        {
            std::cout << index << std::endl;
            std::vector<cv::Point> contour = contoursUpper[index];
            if ( contour.empty() )
            {
                continue;
            }
            cv::Point seed = contour[0];
            //cv::floodFill(reconImage, imageMask, seed, cv::Scalar(thresholdMaxVal)); 
        }
        std::cout << std::endl;


        //return reconImage;
        return imageUpper;
    }

} // namespace bias

