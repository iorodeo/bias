#include "image_dispatcher.hpp"
#include "stamped_image.hpp"
#include <iostream>
#include <QThread>

namespace bias
{

    ImageDispatcher::ImageDispatcher(QObject *parent) : QObject(parent)
    {
        initialize(false,NULL,NULL);
    }

    ImageDispatcher::ImageDispatcher( 
            bool logging,
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(logging,newImageQueuePtr,logImageQueuePtr);
    }

    void ImageDispatcher::initialize(
            bool logging,
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr 
            ) 
    {
        newImageQueuePtr_ = newImageQueuePtr;
        logImageQueuePtr_ = logImageQueuePtr;
        if ((newImageQueuePtr_ != NULL) && (logImageQueuePtr_ !=NULL)) 
        {
            ready_ = true;
        }
        else 
        {
            ready_ = false;
        }
        stopped_ = true;
        logging_ = logging;
        frameCount_ = 0;
        currentTimeStamp_ = 0.0;
    }

    cv::Mat ImageDispatcher::getImage() const
    {
        return currentImage_;
    }

    double ImageDispatcher::getTimeStamp() const
    {
        return currentTimeStamp_;
    }

    double ImageDispatcher::getFPS() const
    {
        return fpsEstimator_.getValue();
    }

    unsigned long ImageDispatcher::getFrameCount() const
    {
        return frameCount_;
    }

    void ImageDispatcher::stop()
    {
        stopped_ = true;
    }


    void ImageDispatcher::run()
    {
        bool done = false;
        StampedImage newStampImage;

        if (!ready_) 
        { 
            return; 
        }

        // Set thread priority to normal
        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::NormalPriority);

        // Initiaiize values
        acquireLock();
        frameCount_ = 0;
        stopped_ = false;
        fpsEstimator_.reset();
        releaseLock();

        while (!done) 
        {

            newImageQueuePtr_ -> acquireLock();
            newImageQueuePtr_ -> waitIfEmpty();
            if (newImageQueuePtr_ -> empty())
            {
                newImageQueuePtr_ -> releaseLock();
                break;
            }
            newStampImage = newImageQueuePtr_ -> front();
            newImageQueuePtr_ -> pop();
            newImageQueuePtr_ -> releaseLock();

            if (logging_ )
            {
                logImageQueuePtr_ -> acquireLock();
                logImageQueuePtr_ -> push(newStampImage);
                logImageQueuePtr_ -> signalNotEmpty();
                logImageQueuePtr_ -> releaseLock();
            }

            acquireLock();
            currentImage_ = newStampImage.image;
            currentTimeStamp_ = newStampImage.timeStamp;
            frameCount_ = newStampImage.frameCount;
            fpsEstimator_.update(newStampImage.timeStamp);
            releaseLock();

            acquireLock();
            done = stopped_;
            releaseLock();
        }
    }

} // namespace bias


