#include "basic_image_proc.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

namespace bias
{

    cv::Mat bwAreaOpen(cv::Mat img, unsigned int areaThreshold)
    {
        // Roughly equivalent to Matlabs bwareaopen. Removes all connected 
        // components with fewer that areaThreshold pixels.
        cv::Mat imgModified = img.clone();
        
        // Do nothing if the image type isn't correct - maybe not 
        // the best policy ...
        if (img.channels() !=1 || img.type() != CV_8U)
        {
            return imgModified;
        }

        // Find controus  
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(
                img.clone(), 
                contours, 
                CV_RETR_EXTERNAL, 
                CV_CHAIN_APPROX_SIMPLE
                );

        // Remove contours whose area is less than the given threshold 
        for (unsigned int i=0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if ((area > 0) && (area < areaThreshold))
            {
                cv::drawContours(imgModified,contours,i,cv::Scalar(0),-1);
            }
        }
        return imgModified;
    }


    cv::Mat imCloseWithDiskElem(cv::Mat img, unsigned int radius)
    {
        // Roughly equivalent to Matlab's imclose using disk structural element 
        // with given radius
        unsigned int elemDiam = 2*radius + 1;
        cv::Size elemSize = cv::Size(elemDiam,elemDiam);
        cv::Mat structElem = cv::getStructuringElement(cv::MORPH_ELLIPSE, elemSize);
        cv::Mat imgClose = cv::Mat(img.size(),img.type());
        cv::morphologyEx(img,imgClose,cv::MORPH_CLOSE,structElem);
        return imgClose;
    }

} // namespace bias
