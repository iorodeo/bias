#include "image_grabber.hpp"
#include <iostream>
#include <QTimer>
#include <opencv2/core/core.hpp>

ImageGrabber::ImageGrabber() 
{
    count_ = 0;
    ready_ = false;
    stopped_ = true;
}

ImageGrabber::ImageGrabber(bias::CameraPtr camPtr, bias::ImagePoolPtr imgPoolPtr)
{
    initialize(camPtr, imgPoolPtr);
}

void ImageGrabber::initialize(bias::CameraPtr camPtr, bias::ImagePoolPtr imgPoolPtr)
{
    cameraPtr_ = camPtr;
    imagePoolPtr_ = imgPoolPtr;
    ready_ = true;
    stopped_ = true;
}

void ImageGrabber::stop()
{
    stopped_ = true;
}

void ImageGrabber::run()
{ 
    cv::Mat img;
    count_ = 0;
    stopped_ = false;

    if (!ready_) { return; }

    cameraPtr_ -> startCapture();

    while (!stopped_)
    {
        std::cout << "image grabber -> new image" << std::endl << std::flush;

        // Remove old image from image pool
        imagePoolPtr_ -> acquireOldImageLock();
        img = imagePoolPtr_ -> dequeueOldImage();
        imagePoolPtr_ -> releaseOldImageLock();
        
        // Capture new image
        cameraPtr_ -> grabImage(img);

        // Place new image into image pool
        imagePoolPtr_ -> acquireNewImageLock();
        imagePoolPtr_ -> enqueueNewImage(img);
        imagePoolPtr_ -> releaseNewImageLock();
        
    }

    cameraPtr_ -> stopCapture();
}


