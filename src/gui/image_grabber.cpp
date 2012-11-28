#include "image_grabber.hpp"
#include "camera.hpp"
#include "exception.hpp"
#include "stamped_image.hpp"
#include "lockable.hpp"
#include "lockable_queue.hpp"
#include <iostream>
#include <QTime>
#include <opencv2/core/core.hpp>

namespace bias {

    ImageGrabber::ImageGrabber(QObject *parent) : QObject(parent) 
    {
        ready_ = false;
        stopped_ = true;
    }

    ImageGrabber::ImageGrabber (
            std::shared_ptr<Lockable<Camera>> cameraPtr,
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(cameraPtr, newImageQueuePtr);
    }

    void ImageGrabber::initialize( 
            std::shared_ptr<Lockable<Camera>> cameraPtr,
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr 
            ) 
    {
        cameraPtr_ = cameraPtr;
        newImageQueuePtr_ = newImageQueuePtr;
        ready_ = true;
        stopped_ = true;
    }

    bool ImageGrabber::tryLock()
    {
        return mutex_.tryLock();
    }

    void ImageGrabber::acquireLock()
    {
        mutex_.lock();
    }

    void ImageGrabber::releaseLock()
    {
        mutex_.unlock();
    }

    void ImageGrabber::stop()
    {
        stopped_ = true;
    }

    void ImageGrabber::run()
    { 
        bool done = false;
        bool grabError = false;
        StampedImage stampImg;
        QTime timer; // TO DO ... temporary for timestamp generation 

        if (!ready_) 
        { 
            return; 
        }

        acquireLock();
        stopped_ = false;
        releaseLock();

        cameraPtr_ -> acquireLock();
        cameraPtr_ -> startCapture();
        cameraPtr_ -> releaseLock();

        timer.start(); // TO DO ... temporary 

        while (!done)
        {
            // Grab image from camera
            cameraPtr_ -> acquireLock();
            try
            {
                stampImg.image = cameraPtr_ -> grabImage();
                grabError = false;
            }
            catch (RuntimeError &runtimeError)
            {
                std::cout << "Frame grab error: id = ";
                std::cout << runtimeError.id() << ", what = "; 
                std::cout << runtimeError.what() << std::endl;
                grabError = true;
            }
            cameraPtr_ -> releaseLock();

            if (!grabError) 
            {
                stampImg.timeStamp = timer.elapsed()*0.001; // TO DO ... temporary

                newImageQueuePtr_ -> acquireLock();
                newImageQueuePtr_ -> push(stampImg);
                newImageQueuePtr_ -> releaseLock();

                acquireLock();
                done = stopped_;
                releaseLock();
            }
        }

        cameraPtr_ -> acquireLock();
        cameraPtr_ -> stopCapture();
        cameraPtr_ -> releaseLock();
    }

} // namespace bias


