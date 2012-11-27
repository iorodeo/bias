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
    }

    bool ImageDispatcher::tryDisplayImageLock()
    {
        return displayImageMutex_.tryLock();
    }

    void ImageDispatcher::acquireDisplayImageLock() 
    {
        displayImageMutex_.lock();
    }

    void ImageDispatcher::releaseDisplayImageLock()
    {
        displayImageMutex_.unlock();
    }

    cv::Mat ImageDispatcher::getDisplayImage()
    {
        return displayImage_;
    }

    void ImageDispatcher::stop()
    {
        stopped_ = true;
    }


    void ImageDispatcher::run()
    {
        StampedImage newStampImage;

        bool haveNewImage = false;

        stopped_ = false;

        while (!stopped_) 
        {
            newImageQueuePtr_ -> acquireLock();
            if (!(newImageQueuePtr_ -> empty()))
            {
                newStampImage = newImageQueuePtr_ -> front();
                newImageQueuePtr_ -> pop();
                haveNewImage = true;

            }
            newImageQueuePtr_ -> releaseLock();

            if (haveNewImage) 
            {
                if (tryDisplayImageLock())
                {
                    displayImage_ = newStampImage.image;
                    releaseDisplayImageLock();
                }
                haveNewImage = false;
            }
        }
    }

} // namespace bias


