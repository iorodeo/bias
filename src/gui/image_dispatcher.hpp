#ifndef BIAS_IMAGE_PROCESSOR_HPP
#define BIAS_IMAGE_PROCESSOR_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <opencv2/core/core.hpp>

namespace bias
{

    struct StampedImage;
    template <class T> class LockableQueue;

    class ImageDispatcher : public QObject, public QRunnable
    {
        Q_OBJECT

        public:
            ImageDispatcher(QObject *parent=0);

            ImageDispatcher( 
                    std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
                    QObject *parent = 0
                    );

            void initialize( 
                    std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr 
                    );

            
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
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            void run();
    };

} // namespace bias


#endif // #ifndef BIAS_IMAGE_PROCESSOR_HPP
