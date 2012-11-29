#ifndef BIAS_GUI_CAMERA_WINDOW_HPP
#define BIAS_GUI_CAMERA_WINDOW_HPP

#include <memory>
#include <QPointer>
#include <QMainWindow>
#include "ui_camera_window.h"
#include "camera_facade_fwd.hpp"

class QTimer;
class QThreadPool;

namespace bias 
{
    class ImageGrabber;
    class ImageDispatcher;
    struct StampedImage;
    template <class T> class Lockable;
    template <class T> class LockableQueue;

    class CameraWindow : public QMainWindow, private Ui::CameraWindow
    {
        Q_OBJECT

        const unsigned int DEFAULT_IMAGE_DISPLAY_DT = 66; // ms

        public:

            CameraWindow(Guid cameraGuid, QWidget *parent=0);

        protected:

            void resizeEvent(QResizeEvent *event);
            void closeEvent(QCloseEvent *event);

        private slots:
           
            void connectButtonClicked();
            void startButtonClicked();
            void updateImageDisplay();
            void startImageCaptureError(unsigned int errorId, QString errorMsg);
            void stopImageCaptureError(unsigned int errorId, QString errorMsg);

        private:

            bool connected_;
            bool capturing_;
            bool havePixmap_;
            unsigned long frameCount_;
            unsigned int imageDisplayDt_;

            QPixmap pixmapOriginal_;

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            QPointer<QThreadPool> threadPoolPtr_;
            QPointer<QTimer> imageDisplayTimerPtr_;

            QPointer<ImageGrabber> imageGrabberPtr_;
            QPointer<ImageDispatcher> imageDispatcherPtr_;

            void connectWidgets();
            void initialize(Guid guid);
            void setupImageDisplayTimer();

            void updatePixmap();
            void updateImageLabel();

            void connectCamera();
            void disconnectCamera();

            void startImageCapture();
            void stopImageCapture();

            void setCameraInfoMessage(QString vendorName, QString modelName);

            void setMenuChildrenEnabled(QWidget *parentWidgetPtr, bool value);


    }; // class CameraWindow

}

#endif // #ifndef BIAS_GUI_CAMERA_WINDOW_HPP
