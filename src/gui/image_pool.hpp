#ifndef BIAS_IMAGE_POOL_HPP
#define BIAS_IMAGE_POOL_HPP

#include <queue>
#include <QMutex>
#include <memory>
#include <opencv2/core/core.hpp>

namespace bias 
{
    class ImagePool
    {
        public:

            ImagePool() {};

            unsigned int numberOfNewImages();
            unsigned int numberOfOldImages();

            void enqueueOldImage(cv::Mat img);
            void enqueueNewImage(cv::Mat img);

            cv::Mat dequeueOldImage();
            cv::Mat dequeueNewImage();

            void acquireNewImageLock();
            void acquireOldImageLock();

            void releaseNewImageLock();
            void releaseOldImageLock();

            void emptyNewImages();
            void emptyOldImages();
            void empty();

        private:

            std::queue<cv::Mat> newImages_; 
            std::queue<cv::Mat> oldImages_;

            QMutex newImageMutex_;
            QMutex oldImageMutex_;
    };

}


#endif // #ifndef BIAS_IMAGE_POOL
