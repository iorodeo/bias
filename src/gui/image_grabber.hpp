#ifndef BIAS_IMAGE_GRABBER_HPP  
#define BIAS_IMAGE_GRABBER_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include "camera_fwd.hpp"
#include "lockable.hpp"

namespace bias
{

    struct StampedImage;

    class ImageGrabber : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            ImageGrabber(QObject *parent=0);

            ImageGrabber(
                    std::shared_ptr<Lockable<Camera>> cameraPtr,
                    std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
                    QObject *parent=0
                    );

            void initialize(
                    std::shared_ptr<Lockable<Camera>> cameraPtr,
                    std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr 
                    );

            void stop();

            static unsigned int NUM_STARTUP_SKIP;

        signals:
            void startCaptureError(unsigned int errorId, QString errorMsg);
            void stopCaptureError(unsigned int errorId, QString errorMsg);

        private:
            bool ready_;
            bool stopped_;
            bool capturing_;

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            void run();
    };

} // namespace bias


#endif // #ifndef BIAS_IMAGE_GRABBER_HPP
