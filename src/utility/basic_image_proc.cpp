#include "basic_image_proc.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

namespace bias
{

    cv::Mat bwAreaOpen(cv::Mat img, unsigned int areaThreshold)
    {
        // Roughly equivalent to Matlabs bwareaopen. Removes small objects 
        // from image. The returned image has pixel value 255 within connected
        // component objects > areaThreshold and 0 everywhere else.  
        //  
        // Note, even with bwAreaOpen you can degenerate cases where bodyArea > 0
        // but less than openArea. For example when the image is all 255. I'm not 
        // worrying about this right now, but you fix this to make is more is line
        // with matlab's function. 
        cv::Mat imgModified = img.clone();
        
        if ((img.channels() !=1) || (img.type() != CV_8U))
        {
            // Do nothing if the image type isn't correct - maybe not 
            // the best policy we can fix this later if needed.
            return imgModified;
        }

        // Find contours  
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(img.clone(),contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

        // Remove contours whose area is less than the given threshold 
        for (unsigned int i=0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if (area < areaThreshold)
            {
                cv::drawContours(imgModified,contours,i,cv::Scalar(0), CV_FILLED);
            }

        }
        return imgModified;
    }


    cv::Mat imCloseWithDiskElem(cv::Mat img, unsigned int radius)
    {
        // Roughly equivalent to Matlab's imclose using disk structural element 
        // with given radius
        unsigned int elemDiam = 2*radius + 1;
        std::cout << "elemDiam: " << elemDiam << std::endl;
        cv::Size elemSize = cv::Size(elemDiam,elemDiam);
        cv::Mat structElem = cv::getStructuringElement(cv::MORPH_ELLIPSE, elemSize);
        cv::Mat imgClose = cv::Mat(img.size(),img.type());
        cv::morphologyEx(img,imgClose,cv::MORPH_CLOSE,structElem);
        return imgClose;
    }

    cv::Mat findMaxConnectedComponent(cv::Mat img)
    {
        cv::Mat imgModified = cv::Mat(img.size(), CV_8U, cv::Scalar(0));

        // Returns and image with only the maximum connected component.
        if ((img.channels() !=1) || (img.type() != CV_8U)) 
        {
            // Do nothing if the image type isn't correct - maybe not 
            // the best policy but we can fix this later if needed. 
            return imgModified;
        }

        // Find contours  
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(img.clone(),contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

        // Find contour w/ maximum area
        unsigned int maxArea = 0;
        unsigned int maxAreaInd = 0;
        for (unsigned int i=0; i<contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea)
            {
                maxArea = area;
                maxAreaInd = i;
            }
        }

        // If found, draw filled version of maximum area contour
        if (maxArea > 0)
        {
            cv::drawContours(imgModified,contours,maxAreaInd,cv::Scalar(255), CV_FILLED);
        }
        return imgModified;
    }

} // namespace bias
