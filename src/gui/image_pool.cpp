#include "image_pool.hpp"

namespace bias
{

    unsigned int ImagePool::numberOfNewImages()
    {
        return newImages_.size();
    }

    unsigned int ImagePool::numberOfOldImages()
    {
        return oldImages_.size();
    }

    void ImagePool::enqueueOldImage(cv::Mat img)
    {
        oldImages_.push(img);
    }

    void ImagePool::enqueueNewImage(cv::Mat img)
    {
        newImages_.push(img);
    }

    cv::Mat ImagePool::dequeueOldImage()
    {
        cv::Mat img;
        if (oldImages_.size() > 0)
        {
            img = oldImages_.front();
            oldImages_.pop();
        }
        return img;
    }

    cv::Mat ImagePool::dequeueNewImage()
    {
        cv::Mat img;
        if (newImages_.size() > 0)
        {
            img = newImages_.front();
            newImages_.pop();
        }
        return img;
    }

    void ImagePool::acquireNewImageLock()
    {
        newImageMutex_.lock();
    }

    void ImagePool::acquireOldImageLock()
    {
        oldImageMutex_.lock();
    }

    void ImagePool::releaseNewImageLock()
    {
        newImageMutex_.unlock();
    }

    void ImagePool::releaseOldImageLock()
    {
        oldImageMutex_.unlock();
    }

    void ImagePool::emptyNewImages()
    {
        while (!newImages_.empty()) 
        { 
            newImages_.pop(); 
        }
    }

    void ImagePool::emptyOldImages()
    {
        while (!oldImages_.empty())
        { 
            oldImages_.pop();
        }
    }

    void ImagePool::empty()
    {
        emptyNewImages();
        emptyOldImages();
    }

}
