#ifndef BIAS_IMAGE_PROCESSOR_HPP
#define BIAS_IMAGE_PROCESSOR_HPP

#include <QMutex>
#include <QRunnable>
#include <opencv2/core/core.hpp>
#include "image_pool.hpp"

class ImageProcessor : public QRunnable
{
    public:
        ImageProcessor();
        explicit ImageProcessor(bias::ImagePoolPtr imagePoolPtr);
        bool tryDisplayImageLock();
        void acquireDisplayImageLock();
        void releaseDisplayImageLock();
        cv::Mat getDisplayImage();
        void stop();

    private:
        bool ready_;
        bool stopped_;

        QMutex displayImageMutex_;
        cv::Mat displayImage_;

        bias::ImagePoolPtr imagePoolPtr_;

        void run();
};


#endif // #ifndef BIAS_IMAGE_PROCESSOR_HPP
