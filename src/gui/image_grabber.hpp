#ifndef BIAS_IMAGE_GRABBER_HPP  
#define BIAS_IMAGE_GRABBER_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include "camera_fwd.hpp"

namespace bias
{

    struct StampedImage;
    template <class T> class Lockable;
    template <class T> class LockableQueue;

    class ImageGrabber : public QObject, public QRunnable
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

            bool tryLock();
            void acquireLock();
            void releaseLock();

            void stop();

        private:
            bool ready_;
            bool stopped_;

            QMutex mutex_;
            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            void run();
    };

} // namespace bias


#endif // #ifndef BIAS_IMAGE_GRABBER_HPP
