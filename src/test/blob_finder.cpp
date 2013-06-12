#include "blob_finder.hpp"
#include "camera_facade.hpp"
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

const float BlobFinder::DEFAULT_FRAMERATE = 10.0;
const float BlobFinder::DEFAULT_DISPLAY_SCALE = 0.5;
const double BlobFinder::DEFAULT_THRESHOLD = 100.0;
const double BlobFinder::DEFAULT_MINIMUM_AREA = 50.0;
const double BlobFinder::DEFAULT_MAXIMUM_AREA = 640.0*480.0;
const double BlobFinder::THRESHOLD_MAXVAL = 255.0;


BlobFinder::BlobFinder()
{
    frameRate_ = DEFAULT_FRAMERATE;
    displayScale_ = DEFAULT_DISPLAY_SCALE;
    threshold_ = DEFAULT_THRESHOLD;
    minimumArea_ = DEFAULT_MINIMUM_AREA;
    maximumArea_ = DEFAULT_MAXIMUM_AREA; 
}


BlobFinder::BlobFinder(float frameRate) : BlobFinder()
{
    frameRate_ = frameRate;
}


bool BlobFinder::run()
{
    bool success = true;

    std::cout << std::endl;
    std::cout << "* finding blobs - press 'x' to exit" << std::endl;

    success = startCapture();
    if (!success)
    {
        return false;
    }
    std::cout << std::endl;

    cv::namedWindow("Raw Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    cv::namedWindow("Threshold Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED); 
    cv::namedWindow("Contour Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED); 

    unsigned long cnt = 0;

    while (true)
    {
        cv::Mat image;
        cv::Mat imageTemp;
        cv::Mat imageThresh;
        cv::Mat imageContour;
        std::vector<std::vector<cv::Point>> contours;
        std::vector<std::vector<cv::Point>> contoursFiltered;

        // Grab frame from camera
        std::cout << "  frame: " << cnt;
        try
        { 
            image = cameraPtr_ -> grabImage();
        }
        catch (RuntimeError &runtimeError)
        {
            std::cout << " - error " << runtimeError.what() << std::endl;
            continue;
        }
        std::cout << std::endl; 

        // Threshold image, invert and find external contours 
        cv::threshold(image,imageThresh, threshold_, THRESHOLD_MAXVAL, CV_THRESH_BINARY);
        imageThresh = THRESHOLD_MAXVAL - imageThresh;
        imageTemp = imageThresh.clone();
        cv::findContours(imageTemp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // Filter contours by area
        for (size_t index=0; index < contours.size(); index++)
        {
            cv::Moments contourMoments = cv::moments(contours[index]);
            if ((contourMoments.m00 >= minimumArea_) && (contourMoments.m00 <= maximumArea_))
            {
                contoursFiltered.push_back(contours[index]);

                double centroidX = contourMoments.m10/contourMoments.m00;
                double centroidY = contourMoments.m01/contourMoments.m00;
                int numBlob = contoursFiltered.size() - 1;
                std::cout << "    blob: " << numBlob; 
                std::cout << ", (" << centroidX << ", " << centroidY << ") " << std::endl;
            }
        }
        std::cout << std::endl;

        // Draw contours on image
        imageContour = cv::Mat(image.size(), CV_8UC3, cv::Scalar(0,0,0));
        for (size_t index=0; index < contoursFiltered.size(); index++)
        {
            cv::drawContours(imageContour, contoursFiltered, index, cv::Scalar(0,0,255));
        }

        // Display images
        cv::resize(image,imageTemp, cv::Size(0,0), displayScale_, displayScale_);
        cv::imshow("Raw Image", imageTemp);

        cv::resize(imageThresh,imageTemp, cv::Size(0,0), displayScale_, displayScale_);
        cv::imshow("Threshold Image", imageTemp);

        cv::resize(imageContour, imageTemp, cv::Size(0,0), displayScale_, displayScale_); 
        cv::imshow("Contour Image", imageTemp);

        // Look for 'x' key press as signal to quit
        int key = cv::waitKey(1);
        if (key == 120)
        {
            break;
        }

        cnt++;
    }

    success = stopCapture();
    if (!success)
    {
        return false;
    }

    return true;
}

bool BlobFinder::setupCamera()
{
    std::cout << std::endl;
    std::cout << "* setup camera" << std::endl << std::endl;

    std::cout << "  searching for cameras ... ";
    CameraFinder cameraFinder;
    CameraPtrList cameraPtrList = cameraFinder.createCameraPtrList();
    if (cameraPtrList.empty())
    {
        std::cout << "no cameras found." << std::endl;
        return false;
    }

    std::cout << cameraPtrList.size() << " cameras" << std::endl;
    for (CameraPtrList::iterator it=cameraPtrList.begin(); it!=cameraPtrList.end(); it++)
    {
        CameraPtr cameraPtr_ = *it;
        std::cout << "  guid: " << cameraPtr_ -> getGuid().toString() << std::endl;
    }

    std::cout << "  connecting to first camera ... ";
    cameraPtr_ = cameraPtrList.front();

    if (cameraPtr_ -> isConnected()) 
    {
        std::cout << "error: camera already connected" << std::endl;
        return false;
    }

    try
    {
        cameraPtr_ -> connect();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl;
        return false;
    }
    std::cout << "done" << std::endl;

    std::cout << "  model:  " << cameraPtr_ -> getModelName() << std::endl;
    std::cout << "  vendor: " << cameraPtr_ -> getVendorName() << std::endl;

    std::cout << "  setting videoMode=Format7, trigger=internal ... ";
    try
    {
        cameraPtr_ -> setVideoMode(VIDEOMODE_FORMAT7);
        cameraPtr_ -> setTriggerInternal();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl;
        return false;
    }
    std::cout << "done" << std::endl;

    std::cout << "  setting framerate to " << frameRate_ << " fps ... ";
    try
    {
        PropertyInfo  frameRateInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_FRAME_RATE);
        Property frameRateProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_FRAME_RATE);
        frameRateProp.absoluteControl = true;
        frameRateProp.absoluteValue = frameRate_;
        frameRateProp.autoActive = false;
        cameraPtr_ -> setProperty(frameRateProp);
        frameRateProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_FRAME_RATE);
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl;
        return false;
    }
    std::cout << "done" << std::endl;
    std::cout << std::endl;

    return true;
}


bool BlobFinder::cleanUp()
{

    std::cout << std::endl;
    std::cout << "* clean up" << std::endl << std::endl;
    std::cout << "  disconnecting camera ... ";

    if (!(cameraPtr_ -> isConnected()))
    {
        std::cout << "already disconnected" << std::endl;
        return false;
    }

    try
    {
        cameraPtr_ -> disconnect();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl;
        return false;
    }
    std::cout << "done" << std::endl;
    std::cout << std::endl;
    return true;
}



bool BlobFinder::startCapture()
{
    std::cout << std::endl;
    std::cout << "* start capture ... ";
    try
    {
        cameraPtr_ -> startCapture();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl;
        return false;
    }
    std::cout << "done" << std::endl;
    return true;
}


bool BlobFinder::stopCapture()
{
    std::cout << std::endl;
    std::cout << "* stop capture ... ";
    try
    {
        cameraPtr_ -> stopCapture();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return false;
    }
    std::cout << "done" << std::endl;
    return true;
}
