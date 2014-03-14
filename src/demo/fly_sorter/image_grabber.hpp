#ifndef IMAGE_GRABBER_HPP  
#define IMAGE_GRABBER_HPP

#include "camera_facade_fwd.hpp"
#include "parameters.hpp"
#include <QDateTime>
#include <QObject>
#include <QRunnable>
#include <QThread>
#include <memory>
#include <opencv2/core/core.hpp>

using namespace bias;

class CameraInfo
{
    public:
        QString vendor;
        QString model;
        QString guid;
        CameraInfo();
};

class ImageData
{
    public:
        cv::Mat mat;
        unsigned long frameCount;
        double dateTime;
        ImageData();
        void copy(ImageData);
};


class ImageGrabber : public QObject, public QRunnable
{
    Q_OBJECT

    public:

        ImageGrabber(ImageGrabberParam param, QObject *parent=0);

    public slots:

        void stopCapture();
        void dumpCameraProperties();


    signals:

        void cameraSetupError(QString errorMsg);
        void fileReadError(QString errorMsg);
        void newCameraInfo(CameraInfo info);
        void newImage(ImageData imageData);
        void stopped();

    private:

        bool stopped_;
        bool capturing_;
        bool dumpCamPropsFlag_;
        std::shared_ptr<Camera> cameraPtr_;
        ImageGrabberParam param_;

        void run();
        void runCaptureFromCamera();
        void runCaptureFromFile();
        void runCaptureFromDir();
        bool setupCamera();

};


class ThreadHelper: public QThread
{
    // Helper to make protected msleep method available in QRunnable
    
    public:

        static void msleep(int ms)
        {
            QThread::msleep(ms);
        }
};





#endif // #ifndef BIAS_IMAGE_GRABBER_HPP
