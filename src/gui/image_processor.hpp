#ifndef BIAS_IMAGE_PROCESSOR_HPP
#define BIAS_IMAGE_PROCESSOR_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <opencv2/core/core.hpp>

namespace bias
{

    class ImagePool;
    typedef std::shared_ptr<ImagePool> ImagePoolPtr;

    class ImageProcessor : public QObject, public QRunnable
    {
        Q_OBJECT

        public:
            ImageProcessor(QObject *parent=0);
            explicit ImageProcessor(ImagePoolPtr imagePoolPtr, QObject *parent=0);
            void initialize(ImagePoolPtr imagePoolPtr);
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

} // namespace bias


#endif // #ifndef BIAS_IMAGE_PROCESSOR_HPP
