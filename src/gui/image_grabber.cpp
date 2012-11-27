#include "image_grabber.hpp"
#include "camera.hpp"
#include "stamped_image.hpp"
#include "lockable_queue.hpp"
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

    ImageGrabber::ImageGrabber (
            CameraPtr cameraPtr, 
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(cameraPtr, newImageQueuePtr);
    }

    void ImageGrabber::initialize( 
            CameraPtr cameraPtr, 
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr 
            ) 
    {
        cameraPtr_ = cameraPtr;
        newImageQueuePtr_ = newImageQueuePtr;
        ready_ = true;
        stopped_ = true;
    }


    void ImageGrabber::stop()
    {
        stopped_ = true;
    }

    void ImageGrabber::run()
    { 
        StampedImage stampImg;

        // TO DO ... Temporary timestamp
        // ------------------------------------
        QTime timer;
        float timeLast = 0.0;
        float timeCurr = 0.0;
        float timeDiff = 0.0;
        float frameRateEst = 0.0;
        // ------------------------------------

        count_ = 0;
        stopped_ = false;

        if (!ready_) { return; }

        cameraPtr_ -> startCapture();
        timer.start();


        while (!stopped_)
        {
            stampImg.image = cameraPtr_ -> grabImage();

            // TO Do ... temporary get current time for time stamp 
            // ---------------------------------------------------
            timeCurr = timer.elapsed()*0.001;
            timeDiff = timeCurr - timeLast;
            timeLast = timeCurr;
            frameRateEst = 0.95*frameRateEst + 0.05/timeDiff; 
            //std::cout << frameRateEst << std::endl << std::flush;
            // ---------------------------------------------------

            stampImg.timeStamp = timeCurr;

            newImageQueuePtr_ -> acquireLock();
            newImageQueuePtr_ -> push(stampImg);
            newImageQueuePtr_ -> releaseLock();
        }

        cameraPtr_ -> stopCapture();
    }

} // namespace bias


