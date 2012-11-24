#ifndef BIAS_IMAGE_GRABBER_HPP  
#define BIAS_IMAGE_GRABBER_HPP

#include <QObject>
#include "camera.hpp"

class ImageGrabber : public QObject
{
    Q_OBJECT

    public:
        ImageGrabber();
        explicit ImageGrabber(bias::CameraPtr camPtr);
        void setCameraPtr(bias::CameraPtr camPtr);

    public slots:
        void run();

    signals:
        void finished();

    private:
        bool haveCamera_;
        bias::CameraPtr cameraPtr_;
};


#endif // #ifndef BIAS_IMAGE_GRABBER_HPP
