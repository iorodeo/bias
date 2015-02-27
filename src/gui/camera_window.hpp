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
#include <QVariantMap>
#include "ui_camera_window.h"
#include "camera_facade_fwd.hpp"
#include "video_writer_params.hpp"
#include "alignment_settings.hpp"
#include "auto_naming_options.hpp"
#include "rtn_status.hpp"
#include "bias_plugin.hpp"


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
    class PluginHandler;
    class TimerSettingsDialog;
    class LoggingSettingsDialog;
    class AutoNamingDialog;
    class Format7SettingsDialog;
    class AlignmentSettingsDialog;
    class ExtCtlHttpServer;
    template <class T> class Lockable;
    template <class T> class LockableQueue;

    class CameraWindow : public QMainWindow, private Ui::CameraWindow
    {
        Q_OBJECT

        public:

            CameraWindow(
                    Guid cameraGuid, 
                    unsigned int cameraNumber, 
                    unsigned int numberOfCameras, 
                    QWidget *parent=0
                    );
            RtnStatus connectCamera(bool showErrorDlg=true);
            RtnStatus disconnectCamera(bool showErrorDlg=true);
            RtnStatus startImageCapture(bool showErrorDlg=true);
            RtnStatus stopImageCapture(bool showErrorDlg=true);

            RtnStatus setConfigurationFromJson(
                    QByteArray jsonConfig, 
                    bool showErrorDlg=true
                    );
            RtnStatus setConfigurationFromMap(
                    QVariantMap configMap, 
                    bool showErrorDlg=true
                    );
            RtnStatus setConfigurationFromMap_Lenient(
                    QVariantMap configMap, 
		    QVariantMap oldConfigMap, 
                    bool showErrorDlg=true
                    );

            QByteArray getConfigurationJson(
                    RtnStatus &rtnStatus, 
                    bool showErrorDlg=true
                    );
            QVariantMap getConfigurationMap(
                    RtnStatus &rtnStatus, 
                    bool showErrorDlg=true
                    );

            RtnStatus enableLogging(bool showErrorDlg=true);
            RtnStatus disableLogging(bool showErrorDlg=true);

            RtnStatus saveConfiguration(
                    QString filename, 
                    bool showErrorDlg=true
                    );

            RtnStatus loadConfiguration(
                    QString fileName, 
                    bool showErrorDlg=true
                    );

            RtnStatus setVideoFile(QString videoFileString);
            void setUserCameraName(QString cameraName);
            RtnStatus setWindowGeometry(QRect windowGeom);
            RtnStatus setWindowGeometryFromJson(QByteArray jsonGeomArray);
            QRect getWindowGeometry();
            QVariantMap getWindowGeometryMap();

            QString getCameraGuidString(RtnStatus &rtnStatus);
            QString getVideoFileFullPath();
            QDir getVideoFileDir();

            bool isConnected();
            bool isCapturing();
            bool isLoggingEnabled();
            double getTimeStamp();
            double getFramesPerSec();
            unsigned long getFrameCount();
            float getFormat7PercentSpeed();

        signals:

            void imageCaptureStarted(bool logging);
            void imageCaptureStopped();
            void format7SettingsChanged();

        protected:

            void resizeEvent(QResizeEvent *event);
            void closeEvent(QCloseEvent *event);
            void showEvent(QShowEvent *event);

        private slots:

            // Start timer on first frame
            void startCaptureDurationTimer();
           
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
            void actionFileSaveConfigTriggered();
            void actionFileHideWindowTriggered();
            void actionCameraInfoTriggered();
            void actionCameraFormat7SettingsTriggered();
            void actionCameraTriggerExternalTriggered();
            void actionCameraTriggerInternalTriggered();
            void actionLoggingEnabledTriggered();
            void actionLoggingVideoFileTriggered();
            void actionLoggingSettingsTriggered();
            void actionLoggingFormatTriggered();
            void actionLoggingAutoNamingTriggered();
            void actionTimerEnabledTriggered();
            void actionTimerSettingsTriggered();
            void actionDisplayUpdateFreqTriggered();
            void actionDisplayFlipVertTriggered();
            void actionDisplayFlipHorzTriggered();
            void actionDisplayRotTriggered();
            void actionDisplayAlignToolsTriggered();
            void actionHelpUserManualTriggered();
            void actionPluginsEnabledTriggered();
            void actionPluginsSettingsTriggered();
            void actionServerEnabledTriggered();
            void actionServerPortTriggered();
            void actionServerCommandsTriggered();
            void actionHelpAboutTriggered();
            void actionDumpCameraPropsTriggered();

            // Signal mappers for menu items e.g. videomode, framerate, properties and colormaps
            void actionVideoModeTriggered(int vidModeInt);
            void actionFrameRateTriggered(int frmRateInt);
            void actionPropertyTriggered(int propTypeInt);
            void actionColorMapTriggered(int colorMapInt);


            // Dialog slots
            void timerDurationChanged(unsigned long duration);
            void loggingSettingsChanged(VideoWriterParams params);
            void format7RoiEnableStateChanged();
            void alignmentSettingsChanged(AlignmentSettings);
            void autoNamingOptionsChanged(AutoNamingOptions options);

        private:

            bool connected_;
            bool capturing_;
            bool haveImagePixmap_;
            bool logging_;
            bool flipVert_;
            bool flipHorz_;
            bool haveDefaultVideoFileDir_;
            bool haveDefaultConfigFileDir_;
            bool showCameraLockFailMsg_;
            bool pluginEnabled_;
            unsigned int cameraNumber_;
            unsigned int numberOfCameras_;
            unsigned int format7PercentSpeed_;
            int colorMapNumber_;

            QDir defaultVideoFileDir_;
            QDir currentVideoFileDir_;
            QString currentVideoFileName_;

            QDir defaultConfigFileDir_;
            QDir currentConfigFileDir_;
            QString currentConfigFileName_;

            QString userCameraName_;

            double timeStamp_;
            double framesPerSec_;
            double imageDisplayFreq_;
            ImageRotationType imageRotation_;
            VideoFileFormat videoFileFormat_;
            unsigned long frameCount_;
            unsigned long captureDurationSec_;
            AutoNamingOptions autoNamingOptions_;

            QPointer<QLabel> statusLabelPtr_;

            QPixmap previewPixmapOriginal_;
            QPixmap pluginPixmapOriginal_;
            QPixmap histogramPixmapOriginal_;

            QPointer<QActionGroup> videoModeActionGroupPtr_; 
            QPointer<QActionGroup> frameRateActionGroupPtr_; 
            QPointer<QActionGroup> cameraTriggerActionGroupPtr_;
            QPointer<QActionGroup> loggingFormatActionGroupPtr_;
            QPointer<QActionGroup> rotationActionGroupPtr_;
            QPointer<QActionGroup> colorMapActionGroupPtr_;
            QPointer<QActionGroup> pluginActionGroupPtr_;

            QPointer<QSignalMapper> videoModeSignalMapperPtr_;
            QPointer<QSignalMapper> frameRateSignalMapperPtr_;
            QPointer<QSignalMapper> propertiesSignalMapperPtr_;
            QPointer<QSignalMapper> colorMapSignalMapperPtr_;

            QMap<QAction*, ImageRotationType> actionToRotationMap_;
            QMap<QAction*, VideoFileFormat> actionToVideoFileFormatMap_;
            QMap<QString, QPointer<BiasPlugin>> pluginMap_;

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr_;
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr_;
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr_;

            QPointer<QThreadPool> threadPoolPtr_;

            QPointer<ImageGrabber> imageGrabberPtr_;
            QPointer<ImageDispatcher> imageDispatcherPtr_;
            QPointer<ImageLogger> imageLoggerPtr_;
            QPointer<PluginHandler> pluginHandlerPtr_;

            QPointer<QTimer> imageDisplayTimerPtr_;
            QPointer<QTimer> captureDurationTimerPtr_;
            QDateTime captureStartDateTime_;
            QDateTime captureStopDateTime_;

            QPointer<TimerSettingsDialog> timerSettingsDialogPtr_;
            QPointer<LoggingSettingsDialog> loggingSettingsDialogPtr_;
            QPointer<AutoNamingDialog> autoNamingDialogPtr_;
            QPointer<Format7SettingsDialog> format7SettingsDialogPtr_;
            AlignmentSettings alignmentSettings_;
            QPointer<AlignmentSettingsDialog> alignmentSettingsDialogPtr_;

            VideoWriterParams videoWriterParams_;

            QPointer<ExtCtlHttpServer> httpServerPtr_;
            unsigned int httpServerPort_;

            void connectWidgets();
            void initialize(
                    Guid guid, 
                    unsigned int cameraNumber, 
                    unsigned int numberOfCameras
                    );


            void setDefaultFileDirs();
            void setupImageDisplayTimer();
            void setupCaptureDurationTimer();
            void updateWindowTitle();
            
            // Menu and statusbar setup methods
            void setupCameraMenu();
            void setupLoggingMenu();
            void setupDisplayMenu();
            void setupDisplayOrientMenu();
            void setupDisplayRotMenu();
            void setupPluginMenu();
            void setupDisplayColorMapMenu();
            void setupStatusLabel();

            // Menu update methods
            void updateAllMenus();
            void updateFileMenu();
            void updateCameraMenu();
            void updateLoggingMenu();
            void updateTimerMenu();
            void updateDisplayMenu();
            void updateDebugMenu();

            void updateCameraVideoModeMenu();
            void updateCameraFrameRateMenu();
            void updateCameraPropertiesMenu();
            void updateCameraTriggerMenu();

            void updateDisplayOrientMenu();
            void updateDisplayRotMenu();
            void updateDisplayColorMapMenu();

            void updatePreviewImageLabel();
            void updateCameraInfoMessage();

            void setupImageLabels(
                    bool cameraPreview=true, 
                    bool histogram=true, 
                    bool pluginPreview=true
                    );
            void updateImageLabel(
                    QLabel *imageLabelPtr, 
                    QPixmap &pixmapOriginal, 
                    bool flipAndRotate=true,
                    bool addFrameCount=true,
                    bool addRoiBoundary=true,
                    bool addAlignmentObjs=true
                    );
            void updateAllImageLabels();

            void resizeImageLabel(
                    QLabel *imageLabelPtr, 
                    QPixmap &pixmapOriginal, 
                    bool flipAndRotate=true,
                    bool addFrameCount=true,
                    bool addRoiBoundary=true,
                    bool addAlignmentObjs=true
                    );

            void resizeAllImageLabels();
            void updateHistogramPixmap(cv::Mat hist);

            void updateStatusLabel(QString msg="");

            void deleteMenuActions(QMenu *menuPtr, QActionGroup *actionGroupPtr=NULL);
            void setCameraInfoMessage(QString vendorName, QString modelName);
            void setMenuChildrenEnabled(QWidget *parentWidgetPtr, bool value);
            void setCaptureTimeLabel(double timeStamp);
            void setServerPortText();

            QString getVideoFileFullPathWithAutoNaming();
            QString getConfigFileFullPath();

            RtnStatus setCameraFromMap(QVariantMap cameraMap, bool showErrorDlg);
            RtnStatus setLoggingFromMap(QVariantMap loggingMap, bool showErrorDlg);
            RtnStatus setCameraPropertyFromMap(
                    QVariantMap propValueMap, 
                    PropertyInfo propInfo, 
                    bool showErrorDlg
                    );
            RtnStatus setFormat7SettingsFromMap(
                    QVariantMap settingsMap,
                    Format7Info format7Info,
                    bool showErrorDlg
                    );
            RtnStatus setLoggingFormatFromMap(
                    QVariantMap formatMap, 
                    bool showErrorDlg
                    );
            RtnStatus setAutoNamingOptionsFromMap(
                    QVariantMap autoNamingOptionsMap,
                    bool showErrorDlg
                    );
            RtnStatus setTimerFromMap(QVariantMap timerMap, bool showErrorDlg);
            RtnStatus setDisplayFromMap(QVariantMap displayMap, bool showErrorDlg);
            RtnStatus setServerFromMap(QVariantMap serverMap, bool showErrorDlg);
            RtnStatus setConfigFileFromMap(QVariantMap configFileMap, bool showErrorDlg);

            cv::Mat calcHistogram(cv::Mat mat);
            RtnStatus onError(QString message, QString title, bool showErrorDlg);

    }; // class CameraWindow

    // Utilitiy functions
    QString boolToOnOffQString(bool value);
    QString timeStampToQString(double timeStamp);

    VideoMode convertStringToVideoMode(QString videoModeString);
    FrameRate convertStringToFrameRate(QString frameRateString);
    TriggerType convertStringToTriggerType(QString trigTypeString);
    VideoFileFormat convertStringToVideoFileFormat(QString formatString);
    ImageMode convertStringToImageMode(QString imageModeString);
    PixelFormat convertStringToPixelFormat(QString pixelFormatString);

    QMap<QString, VideoMode> getStringToVideoModeMap();
    QMap<QString, FrameRate> getStringToFrameRateMap();
    QMap<QString, TriggerType> getStringToTriggerTypeMap();
    QMap<QString, ImageMode> getStringToImageModeMap();
    QMap<QString, PixelFormat> getStringToPixelFormatMap();

    QString propNameToCamelCase(QString propName);

} // namespace bias



#endif // #ifndef BIAS_GUI_CAMERA_WINDOW_HPP
