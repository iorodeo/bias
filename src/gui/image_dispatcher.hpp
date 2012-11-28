#ifndef BIAS_IMAGE_PROCESSOR_HPP
#define BIAS_IMAGE_PROCESSOR_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <opencv2/core/core.hpp>
#include "fps_estimator.hpp"

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

            
            bool tryLock();
            void acquireLock();
            void releaseLock();

            // Use lock when calling these methods
            // ----------------------------------
            void stop();
            cv::Mat getImage();
            double getTimeStamp();
            double getFPS();
            // -----------------------------------

        private:
            bool ready_;
            QMutex mutex_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            // use lock when setting these values
            // -----------------------------------
            bool stopped_;
            cv::Mat currentImage_;
            double currentTimeStamp_;
            FPS_Estimator fpsEstimator_;
            // ------------------------------------

            void run();
    };

} // namespace bias


#endif // #ifndef BIAS_IMAGE_PROCESSOR_HPP
