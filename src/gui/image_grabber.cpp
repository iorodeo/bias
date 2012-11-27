#include "image_grabber.hpp"
#include "camera.hpp"
#include "image_pool.hpp"
#include <iostream>
#include <QTime>
#include <opencv2/core/core.hpp>

namespace bias {

    ImageGrabber::ImageGrabber(QObject *parent) : QObject(parent) 
    {
        count_ = 0;
        ready_ = false;
        stopped_ = true;
    }

    ImageGrabber::ImageGrabber(CameraPtr camPtr, ImagePoolPtr imgPoolPtr, QObject *parent) : QObject(parent)
    {
        initialize(camPtr, imgPoolPtr);
    }

    void ImageGrabber::initialize(CameraPtr camPtr, ImagePoolPtr imgPoolPtr)
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
        QTime timer;
        float timeLast = 0.0;
        float timeCurr = 0.0;
        float timeDiff = 0.0;
        float frameRateEst = 0.0;

        count_ = 0;
        stopped_ = false;

        if (!ready_) { return; }

        cameraPtr_ -> startCapture();
        timer.start();


        while (!stopped_)
        {
            //// Remove old image from image pool
            //imagePoolPtr_ -> acquireOldImageLock();
            //img = imagePoolPtr_ -> dequeueOldImage();
            //imagePoolPtr_ -> releaseOldImageLock();

            // Capture new image
            img = cameraPtr_ -> grabImage();

            timeCurr = timer.elapsed()*0.001;
            timeDiff = timeCurr - timeLast;
            timeLast = timeCurr;
            frameRateEst = 0.95*frameRateEst + 0.05/timeDiff; 

            //std::cout << frameRateEst << std::endl << std::flush;
            std::cout << timeCurr << " " << timeDiff << std::endl << std::flush;

            // Place new image into image pool
            imagePoolPtr_ -> acquireNewImageLock();
            imagePoolPtr_ -> enqueueNewImage(img);
            imagePoolPtr_ -> releaseNewImageLock();

        }

        cameraPtr_ -> stopCapture();
    }

} // namespace bias


