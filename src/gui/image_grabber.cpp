#include "image_grabber.hpp"
#include "camera.hpp"
#include "exception.hpp"
#include "stamped_image.hpp"
#include "affinity.hpp"
#include <iostream>
#include <QTime>
#include <QThread>
#include <opencv2/core/core.hpp>

namespace bias {

    unsigned int ImageGrabber::NUM_STARTUP_SKIP = 15;

    ImageGrabber::ImageGrabber(QObject *parent) : QObject(parent) 
    {
        initialize(NULL,NULL);
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
        capturing_ = false;
        stopped_ = true;
        cameraPtr_ = cameraPtr;
        newImageQueuePtr_ = newImageQueuePtr;
        if ((cameraPtr_ != NULL) && (newImageQueuePtr_ != NULL))
        {
            ready_ = true;
        }
        else
        {
            ready_ = false;
        }
    }

    void ImageGrabber::stop()
    {
        stopped_ = true;
    }

    void ImageGrabber::run()
    { 
        //double tLast = 0.0;

        bool isFirst = true;
        bool done = false;
        bool error = false;
        unsigned int errorId = 0;
        unsigned long frameCount = 0;
        unsigned long startUpCount = 0;
            

        StampedImage stampImg;

        TimeStamp timeStamp;
        TimeStamp timeStampInit; 

        double timeStampDbl = 0.0;
        double timeStampDblLast = 0.0;

        QString errorMsg("no message");

        if (!ready_) 
        { 
            return; 
        }

        // Set thread priority to "time critical" and assign cpu affinity
        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::TimeCriticalPriority);
        assignThreadAffinity(true,1,0);


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

        // Grab images from camera until the done signal is given
        while (!done)
        {

            // Grab an image
            cameraPtr_ -> acquireLock();
            try
            {
                stampImg.image = cameraPtr_ -> grabImage();
                timeStamp = cameraPtr_ -> getImageTimeStamp();
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

            // Skip some number of frames on startup 
            if (startUpCount < NUM_STARTUP_SKIP)
            {
                startUpCount++;
                continue;
            }

            // Push image into new image queue
            if (!error) 
            {
                // Save initial time stamp
                if (isFirst)
                {
                    timeStampInit = timeStamp;
                    isFirst = false;
                }
                timeStampDbl  = double(timeStamp.seconds);
                timeStampDbl -= double(timeStampInit.seconds);
                timeStampDbl += (1.0e-6)*double(timeStamp.microSeconds);
                timeStampDbl -= (1.0e-6)*double(timeStampInit.microSeconds);
                stampImg.timeStamp = timeStampDbl;
                stampImg.frameCount = frameCount;
                frameCount++;

                newImageQueuePtr_ -> acquireLock();
                newImageQueuePtr_ -> push(stampImg);
                newImageQueuePtr_ -> signalNotEmpty(); 
                newImageQueuePtr_ -> releaseLock();

                acquireLock();
                done = stopped_;
                releaseLock();

                // ------------------------------------------------------------------
                //std::cout << "dt = " << timeStampDbl - timeStampDblLast << std::endl;
                //timeStampDblLast = timeStampDbl;
                // ------------------------------------------------------------------

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


