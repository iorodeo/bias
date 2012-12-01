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
        capturing_ = false;
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
        capturing_ = false;
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
        bool error = false;
        unsigned int errorId = 0;
        StampedImage stampImg;
        QString errorMsg("no message");
        QTime clock; // TO DO ... temporary timestamp generation 

        if (!ready_) 
        { 
            return; 
        }

        // Start image capture
        cameraPtr_ -> acquireLock();
        try
        {
            cameraPtr_ -> startCapture();
        }
        catch (RuntimeError &runtimeError)
        {
            error = true;
            errorId = runtimeError.id();
            errorMsg = QString::fromStdString(runtimeError.what());
        }
        cameraPtr_ -> releaseLock();

        if (error)
        {
            emit startCaptureError(errorId, errorMsg);
            return;
        } 

        acquireLock();
        stopped_ = false;
        releaseLock();

        clock.start(); // TO DO ... temporary timestamp generation 

        // Grab images from camera until the done signal is given
        while (!done)
        {
            // Grab an image
            cameraPtr_ -> acquireLock();
            try
            {
                stampImg.image = cameraPtr_ -> grabImage();
                error = false;
            }
            catch (RuntimeError &runtimeError)
            {
                std::cout << "Frame grab error: id = ";
                std::cout << runtimeError.id() << ", what = "; 
                std::cout << runtimeError.what() << std::endl;
                error = true;
            }
            cameraPtr_ -> releaseLock();

            // Push image into new image queue
            if (!error) 
            {
                stampImg.timeStamp = clock.elapsed()*0.001; // TO DO ... temporary

                newImageQueuePtr_ -> acquireLock();
                newImageQueuePtr_ -> push(stampImg);
                newImageQueuePtr_ -> releaseLock();

                acquireLock();
                done = stopped_;
                releaseLock();
            }
        }

        // Stop image capture
        cameraPtr_ -> acquireLock();
        try
        {
            cameraPtr_ -> stopCapture();
        }
        catch (RuntimeError &runtimeError)
        {
            error = true;
            errorId = runtimeError.id();
            errorMsg = QString::fromStdString(runtimeError.what());
        }
        cameraPtr_ -> releaseLock();

        if (error)
        {
            emit stopCaptureError(errorId, errorMsg);
        }
    }

} // namespace bias


