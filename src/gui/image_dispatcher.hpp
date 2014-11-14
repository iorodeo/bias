#ifndef BIAS_IMAGE_PROCESSOR_HPP
#define BIAS_IMAGE_PROCESSOR_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <opencv2/core/core.hpp>
#include "fps_estimator.hpp"
#include "lockable.hpp"

namespace bias
{

    struct StampedImage;

    class ImageDispatcher : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            ImageDispatcher(QObject *parent=0);

            ImageDispatcher( 
                    bool logging,
                    bool pluginEnabled,
                    unsigned int cameraNumber,
                    std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
                    std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
                    std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr,
                    QObject *parent = 0
                    );

            void initialize( 
                    bool logging,
                    bool pluginEnabled,
                    unsigned int cameraNumber,
                    std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr,
                    std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr ,
                    std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr
                    );

            // Use lock when calling these methods
            // ----------------------------------
            void stop();
            cv::Mat getImage() const;     // Note, might want to change so that we return 
            double getTimeStamp() const;  // the stampedImage.
            double getFPS() const;
            unsigned long getFrameCount() const;
            // -----------------------------------

        private:
            bool ready_;
            bool logging_;
            bool pluginEnabled_;
            unsigned int cameraNumber_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr_;
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr_;

            // use lock when setting these values
            // -----------------------------------
            bool stopped_;
            cv::Mat currentImage_;        // Note, might want to change so that we store
            double currentTimeStamp_;     // the stampedImage. 
            FPS_Estimator fpsEstimator_;
            unsigned long frameCount_;
            // ------------------------------------

            void run();
    };

} // namespace bias


#endif // #ifndef BIAS_IMAGE_PROCESSOR_HPP
