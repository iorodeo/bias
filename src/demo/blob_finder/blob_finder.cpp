#include "blob_finder.hpp"
#include "camera_facade.hpp"
#include "blob_data.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

BlobFinder::BlobFinder() {}


BlobFinder::BlobFinder(BlobFinderParam param)
{
    param_ = param;
}


bool BlobFinder::run()
{
    bool success = true;

    std::cout << std::endl;
    std::cout << "* finding blobs - press 'q' to exit" << std::endl;

    success = startCapture();
    if (!success)
    {
        return false;
    }
    std::cout << std::endl;

    cv::namedWindow("Threshold Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED); 
    cv::namedWindow("Contour Image",   CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED); 

    unsigned long cnt = 0;

    while (true)
    {
        cv::Mat image;
        cv::Mat imageTemp;
        cv::Mat imageThresh;
        cv::Mat imageBlobs;
        std::vector<std::vector<cv::Point>> contours;
        BlobDataList blobDataList;

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
        std::cout << std::endl << std::endl; 

        // Threshold image, invert and find external contours 
        cv::threshold(image,imageThresh, param_.threshold, param_.thresholdMaxVal, CV_THRESH_BINARY);
        imageThresh = param_.thresholdMaxVal - imageThresh;
        imageTemp = imageThresh.clone();
        cv::findContours(imageTemp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // Copy gray image to RGB and draw contours on image
        imageBlobs = cv::Mat(image.size(), CV_8UC3, cv::Scalar(0,0,0));
        cvtColor(image,imageBlobs,CV_GRAY2BGR);

        // Filter contours by area and compute descriptive data
        for (size_t index=0; index < contours.size(); index++)
        {
            cv::Moments contourMoments = cv::moments(contours[index]);
            BlobData blobData = BlobData(contours[index]);

            if ((blobData.area >= param_.minimumArea) && (blobData.area <= param_.maximumArea))
            {
                blobCnt++;
                blobDataList.push_back(blobData);

                std::cout << "    blob #: " << blobDataList.size() << std::endl;
                blobData.print(2);
                std::cout << std::endl;
                blobData.draw(imageBlobs);
            }
        }
        std::cout << std::endl;


        cv::resize(imageThresh,imageTemp, cv::Size(0,0), param_.displayScale, param_.displayScale);
        cv::imshow("Threshold Image", imageTemp);

        cv::resize(imageBlobs, imageTemp, cv::Size(0,0), param_.displayScale, param_.displayScale); 
        cv::imshow("Contour Image", imageTemp);

        // Look for 'q' key press as signal to quit
        int key = cv::waitKey(1);
        if ((key == 'q') || (key == 'Q'))
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
    std::cout << "  setting framerate to " << param_.frameRate << " fps ... ";

    try
    {
        PropertyInfo  frameRateInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_FRAME_RATE);
        Property frameRateProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_FRAME_RATE);
        frameRateProp.absoluteControl = true;
        frameRateProp.absoluteValue = param_.frameRate;
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

