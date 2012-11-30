#ifndef BIAS_GUI_CAMERA_WINDOW_HPP
#define BIAS_GUI_CAMERA_WINDOW_HPP

#include <memory>
#include <QDir>
#include <QPointer>
#include <QMainWindow>
#include "ui_camera_window.h"
#include "camera_facade_fwd.hpp"


// Qt forward declarations
class QTimer;
class QThreadPool;

namespace bias 
{
    // BIAS forward declarations
    class ImageGrabber;
    class ImageDispatcher;
    struct StampedImage;
    template <class T> class Lockable;
    template <class T> class LockableQueue;


    class CameraWindow : public QMainWindow, private Ui::CameraWindow
    {
        Q_OBJECT

        const double DEFAULT_IMAGE_DISPLAY_FREQ = 15.0; // Hz 

        public:

            CameraWindow(Guid cameraGuid, QWidget *parent=0);

        protected:

            void resizeEvent(QResizeEvent *event);
            void closeEvent(QCloseEvent *event);

        private slots:
           
            // Button callbacks
            void connectButtonClicked();
            void startButtonClicked();
            void startImageCaptureError(unsigned int errorId, QString errorMsg);
            void stopImageCaptureError(unsigned int errorId, QString errorMsg);

            // Display Timer update
            void updateImageDisplay();

            // Menu actions
            void actionCameraInfoTriggered();
            void actionCameraFormat7SettingsTriggered();
            void actionLoggingEnabledTriggered();
            void actionLoggingVideoFileTriggered();
            void actionLoggingSettingsTriggered();
            void actionLoggingFormatTriggered();
            void actionTimerEnabledTriggered();
            void actionTimerSettingsTriggered();
            void actionDisplayUpdateFreqTriggered();
            void actionDisplayFlipVertTriggered();
            void actionDisplayFlipHorzTriggered();
            void actionDisplayRotTriggered();
            void actionHelpUserManualTriggered();
            void actionHelpAboutTriggered();

        private:

            bool connected_;
            bool capturing_;
            bool havePixmap_;
            bool logging_;
            bool timer_;
            bool flipVert_;
            bool flipHorz_;
            QDir defaultSaveDir_;
            double imageDisplayFreq_;
            unsigned long frameCount_;

            QPixmap pixmapOriginal_;
            QPointer<QActionGroup> cameraTriggerActionGroupPtr_;
            QPointer<QActionGroup> rotationActionGroupPtr_;
            QPointer<QActionGroup> loggingFormatActionGroupPtr_;

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;

            QPointer<QThreadPool> threadPoolPtr_;
            QPointer<QTimer> imageDisplayTimerPtr_;

            QPointer<ImageGrabber> imageGrabberPtr_;
            QPointer<ImageDispatcher> imageDispatcherPtr_;

            void connectWidgets();
            void initialize(Guid guid);
            void setDefaultSaveDir();
            void setupImageDisplayTimer();
            void setupCameraMenu();
            void setupLoggingMenu();
            void setupDisplayMenu();
            void setupDisplayOrientMenu();
            void setupDisplayRotMenu();

            void updatePixmap();
            void updateImageLabel();

            void connectCamera();
            void disconnectCamera();

            void startImageCapture();
            void stopImageCapture();

            void updateCameraInfoMessage();
            void updateCameraMenu();
            void populateVideoModeMenu();

            void deleteMenuActions(QMenu *menuPtr);
            void setCameraInfoMessage(QString vendorName, QString modelName);
            void setMenuChildrenEnabled(QWidget *parentWidgetPtr, bool value);
            void setCaptureTimeLabel(double timeStamp);

    }; // class CameraWindow


    // Utilitiy functions
    QString boolToOnOffQString(bool value);
    QString timeStampToQString(double timeStamp);
}

#endif // #ifndef BIAS_GUI_CAMERA_WINDOW_HPP
