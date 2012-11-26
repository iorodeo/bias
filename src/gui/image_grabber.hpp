#ifndef BIAS_IMAGE_GRABBER_HPP  
#define BIAS_IMAGE_GRABBER_HPP

#include <QRunnable>
#include "camera.hpp"
#include "image_pool.hpp"

class ImageGrabber : public QRunnable
{
    public:
        ImageGrabber();
        ImageGrabber(bias::CameraPtr camPtr, bias::ImagePoolPtr imgPoolPtr);
        void initialize(bias::CameraPtr camPtr, bias::ImagePoolPtr imgPoolPtr);
        void stop();

    private:
        unsigned int count_;
        bool ready_;
        bool stopped_;
        bias::CameraPtr cameraPtr_;
        bias::ImagePoolPtr imagePoolPtr_;
        void run();
};


#endif // #ifndef BIAS_IMAGE_GRABBER_HPP
