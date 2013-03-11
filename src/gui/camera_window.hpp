#ifndef BIAS_GUI_CAMERA_WINDOW_HPP
#define BIAS_GUI_CAMERA_WINDOW_HPP

#include <memory>
#include <QDir>
#include <QSize>
#include <QMap>
#include <QPointer>
#include <QDateTime>
#include <QMainWindow>
#include <QByteArray>
#include "ui_camera_window.h"
#include "camera_facade_fwd.hpp"
#include "video_writer_params.hpp"


// External lib forward declarations
class QTimer;
class QThreadPool;
class QSignalMapper;

namespace cv   { class Mat; }

namespace bias 
{
    // BIAS forward declarations
    struct StampedImage;
    class ImageGrabber;
    class ImageDispatcher;
    class ImageLogger; 
    class TimerSettingsDialog;
    class LoggingSettingsDialog;
    template <class T> class Lockable;
    template <class T> class LockableQueue;

    class CameraWindow : public QMainWindow, private Ui::CameraWindow
    {
        Q_OBJECT

        static const unsigned int MAX_THREAD_COUNT=10;

        public:

            CameraWindow(Guid cameraGuid, QWidget *parent=0);
            void saveConfiguration(QString filename);
            void loadConfiguration(QString fileName);
            QByteArray getConfiguration();
            void setConfiguration(QByteArray jsonConfig);

        signals:

            void imageCaptureStarted();
            void imageCaptureStopped();

        protected:

            void resizeEvent(QResizeEvent *event);
            void closeEvent(QCloseEvent *event);
            void showEvent(QShowEvent *event);

        private slots:
           
            // Button callbacks
            void connectButtonClicked();
            void startButtonClicked();

            // Errors
            void startImageCaptureError(unsigned int errorId, QString errorMsg);
            void stopImageCaptureError(unsigned int errorId, QString errorMsg);
            void imageCaptureError(unsigned int errorId, QString errorMsg);
            void imageLoggingError(unsigned int errorId, QString errorMsg);

            // Display update and duration check timers
            void updateDisplayOnTimer();
            void checkDurationOnTimer();

            // Tab changed event
            void tabWidgetChanged(int index);

            // Menu actions
            void actionFileLoadConfigTriggered();
            void actionFileSaveconfigTriggered();
            void actionFileHideWindowTriggered();
            void actionCameraInfoTriggered();
            void actionCameraFormat7SettingsTriggered();
            void actionCameraTriggerExternalTriggered();
            void actionCameraTriggerInternalTriggered();
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
            void actionPluginsSettingsTriggered();

            // Signal mappers for videomode, framerate and properties
            void actionVideoModeTriggered(int vidModeInt);
            void actionFrameRateTriggered(int frmRateInt);
            void actionPropertyTriggered(int propTypeInt);

            // Dialog slots
            void timerDurationChanged(unsigned long duration);
            void loggingSettingsChanged(VideoWriterParams params);

        private:

            bool connected_;
            bool capturing_;
            bool logging_;
            bool flipVert_;
            bool flipHorz_;
            bool haveDefaultVideoFileDir_;
            bool haveDefaultConfigFileDir_;

            QDir defaultVideoFileDir_;
            QDir currentVideoFileDir_;
            QString currentVideoFileName_;

            QDir defaultConfigFileDir_;
            QDir currentConfigFileDir_;
            QString currentConfigFileName_;

            double imageDisplayFreq_;
            ImageRotationType imageRotation_;
            VideoFileFormat videoFileFormat_;
            unsigned long frameCount_;
            unsigned long captureDurationSec_;

            QPixmap previewPixmapOriginal_;
            QPixmap pluginPixmapOriginal_;
            QPixmap histogramPixmapOriginal_;

            QPointer<QActionGroup> videoModeActionGroupPtr_; 
            QPointer<QActionGroup> frameRateActionGroupPtr_; 
            QPointer<QActionGroup> cameraTriggerActionGroupPtr_;
            QPointer<QActionGroup> loggingFormatActionGroupPtr_;
            QPointer<QActionGroup> rotationActionGroupPtr_;

            QPointer<QSignalMapper> videoModeSignalMapperPtr_;
            QPointer<QSignalMapper> frameRateSignalMapperPtr_;
            QPointer<QSignalMapper> propertiesSignalMapperPtr_;

            QMap<QAction*, ImageRotationType> actionToRotationMap_;
            QMap<QAction*, VideoFileFormat> actionToVideoFileFormatMap_;

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr_;

            QPointer<QThreadPool> threadPoolPtr_;

            QPointer<ImageGrabber> imageGrabberPtr_;
            QPointer<ImageDispatcher> imageDispatcherPtr_;
            QPointer<ImageLogger> imageLoggerPtr_;

            QPointer<QTimer> imageDisplayTimerPtr_;
            QPointer<QTimer> captureDurationTimerPtr_;
            QDateTime captureStartDateTime_;
            QDateTime captureStopDateTime_;

            QPointer<TimerSettingsDialog> timerSettingsDialogPtr_;
            QPointer<LoggingSettingsDialog> loggingSettingsDialogPtr_;

            VideoWriterParams videoWriterParams_;

            void connectWidgets();
            void initialize(Guid guid);

            void connectCamera();
            void disconnectCamera();

            void startImageCapture();
            void stopImageCapture();

            void setDefaultFileDirs();
            void setupImageDisplayTimer();
            void setupCaptureDurationTimer();
            
            // Menu setup methods
            void setupCameraMenu();
            void setupLoggingMenu();
            void setupDisplayMenu();
            void setupDisplayOrientMenu();
            void setupDisplayRotMenu();

            // Menu update methods
            void updateAllMenus();
            void updateFileMenu();
            void updateCameraMenu();
            void updateLoggingMenu();
            void updateTimerMenu();

            void updateCameraVideoModeMenu();
            void updateCameraFrameRateMenu();
            void updateCameraPropertiesMenu();
            void updateCameraTriggerMenu();

            void updatePreviewImageLabel();
            void updateCameraInfoMessage();

            void setupImageLabels();
            void updateImageLabel(
                    QLabel *imageLabelPtr, 
                    QPixmap &pixmapOriginal, 
                    bool flipAndRotate=true,
                    bool addFrameCount=true
                    );
            void updateAllImageLabels();

            void resizeImageLabel(
                    QLabel *imageLabelPtr, 
                    QPixmap &pixmapOriginal, 
                    bool flipAndRotate=true,
                    bool addFrameCount=true
                    );
            void resizeAllImageLabels();
            void updateHistogramPixmap(cv::Mat hist);

            void deleteMenuActions(QMenu *menuPtr, QActionGroup *actionGroupPtr=NULL);
            void setCameraInfoMessage(QString vendorName, QString modelName);
            void setMenuChildrenEnabled(QWidget *parentWidgetPtr, bool value);
            void setCaptureTimeLabel(double timeStamp);

            QString getVideoFileFullPath();
            QString getVideoFileFullPathWithGuid();
            QString getConfigFileFullPath();

            cv::Mat calcHistogram(cv::Mat mat);

    }; // class CameraWindow


    // Utilitiy functions
    QString boolToOnOffQString(bool value);
    QString timeStampToQString(double timeStamp);

}

#endif // #ifndef BIAS_GUI_CAMERA_WINDOW_HPP
