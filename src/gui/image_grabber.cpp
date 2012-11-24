#include "image_grabber.hpp"
#include <iostream>
#include <QTimer>
#include <opencv2/core/core.hpp>

ImageGrabber::ImageGrabber() 
{
    haveCamera_ = false;
}

ImageGrabber::ImageGrabber(bias::CameraPtr cameraPtr) 
{
    setCameraPtr(cameraPtr);
}

void ImageGrabber::setCameraPtr(bias::CameraPtr cameraPtr)
{
    cameraPtr_ = cameraPtr;
    haveCamera_ = true;
}


void ImageGrabber::run()
{
    if (!haveCamera_)
    {
        while (true)
        {
            std::cout << "hello" << std::endl;

        }

    }
    emit finished();
}

