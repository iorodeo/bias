#ifndef BIAS_IMAGE_GRABBER_HPP  
#define BIAS_IMAGE_GRABBER_HPP

#include <memory>
#include <QObject>
#include <QRunnable>
#include "camera_fwd.hpp"

namespace bias
{

    class ImagePool;
    typedef std::shared_ptr<ImagePool> ImagePoolPtr;

    class ImageGrabber : public QObject, public QRunnable
    {
        Q_OBJECT

        public:
            ImageGrabber(QObject *parent=0);
            ImageGrabber(CameraPtr camPtr, ImagePoolPtr imgPoolPtr, QObject *parent=0);
            void initialize(CameraPtr camPtr, ImagePoolPtr imgPoolPtr);
            void stop();

        private:
            unsigned int count_;
            bool ready_;
            bool stopped_;
            CameraPtr cameraPtr_;
            ImagePoolPtr imagePoolPtr_;

            void run();
    };

} // namespace bias


#endif // #ifndef BIAS_IMAGE_GRABBER_HPP
