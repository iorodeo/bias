#include "image_dispatcher.hpp"
#include "stamped_image.hpp"
#include "lockable_queue.hpp"
#include <iostream>

namespace bias
{

    ImageDispatcher::ImageDispatcher(QObject *parent) : QObject(parent)
    {
        ready_ = false;
        stopped_ = true;
        currentTimeStamp_ = 0.0;
    }

    ImageDispatcher::ImageDispatcher( 
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(newImageQueuePtr);
    }

    void ImageDispatcher::initialize(
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr 
            ) 
    {
        newImageQueuePtr_ = newImageQueuePtr;
        ready_ = true;
        stopped_ = true;
        currentTimeStamp_ = 0.0;
    }

    bool ImageDispatcher::tryLock()
    {
        return mutex_.tryLock();
    }

    void ImageDispatcher::acquireLock() 
    {
        mutex_.lock();
    }

    void ImageDispatcher::releaseLock()
    {
        mutex_.unlock();
    }

    cv::Mat ImageDispatcher::getImage()
    {
        return currentImage_;
    }

    double ImageDispatcher::getTimeStamp()
    {
        return currentTimeStamp_;
    }

    double ImageDispatcher::getFPS()
    {
        return fpsEstimator_.getValue();
    }

    void ImageDispatcher::stop()
    {
        stopped_ = true;
    }


    void ImageDispatcher::run()
    {
        StampedImage newStampImage;
        bool haveNewImage = false;
        bool done = false;

        stopped_ = false;
        fpsEstimator_.reset();

        while (!done) 
        {
            newImageQueuePtr_ -> acquireLock();
            if (!(newImageQueuePtr_ -> empty()))
            {
                newStampImage = newImageQueuePtr_ -> front();
                newImageQueuePtr_ -> pop();
                haveNewImage = true;

            }
            newImageQueuePtr_ -> releaseLock();

            acquireLock();
            if (haveNewImage) 
            {
                currentImage_ = newStampImage.image;
                currentTimeStamp_ = newStampImage.timeStamp;
                fpsEstimator_.update(newStampImage.timeStamp);
                haveNewImage = false;
            }
            done = stopped_;
            releaseLock();
        }
    }

} // namespace bias


