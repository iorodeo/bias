#include "camera_window.hpp"
#include "camera_facade.hpp"
#include "mat_to_qimage.hpp"
#include "stamped_image.hpp"
#include "lockable.hpp"
#include "image_label.hpp"
#include "image_grabber.hpp"
#include "image_dispatcher.hpp"
#include "image_logger.hpp"
#include "video_writer.hpp"
#include "video_writer_bmp.hpp"
#include "video_writer_avi.hpp"
#include "video_writer_fmf.hpp"
#include "video_writer_ufmf.hpp"
#include "affinity.hpp"
#include "property_dialog.hpp"
#include "timer_settings_dialog.hpp"
#include "logging_settings_dialog.hpp"
#include "auto_naming_dialog.hpp"
#include "format7_settings_dialog.hpp"
#include "alignment_settings_dialog.hpp"
#include "background_histogram_ufmf.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include "ext_ctl_http_server.hpp"
#include "plugin_handler.hpp"

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <QtGui>
#include <QTimer>
#include <QThreadPool>
#include <QSignalMapper>
#include <QFile>
#include <QApplication>
#include <QtDebug>

// Added for qt5 build
// ---------------------------
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
// ---------------------------

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>


// Development
// ------------------------------------
#include "stampede_plugin.hpp"
#include "grab_detector_plugin.hpp"
// -------------------------------------

namespace bias
{
    // Constants
    // ----------------------------------------------------------------------------------
    
    const unsigned int DURATION_TIMER_INTERVAL = 1000; // msec
    const QSize PREVIEW_DUMMY_IMAGE_SIZE = QSize(320,256);
    const unsigned int MAX_THREAD_COUNT=10;

    // Default settings
    const unsigned long DEFAULT_CAPTURE_DURATION = 300; // sec
    const double DEFAULT_IMAGE_DISPLAY_FREQ = 15.0;     // Hz
    const double MAX_IMAGE_DISPLAY_FREQ = 60.0;         // Hz
    const double MIN_IMAGE_DISPLAY_FREQ = 1.0;          // Hz 
    const QSize DEFAULT_HISTOGRAM_IMAGE_SIZE = QSize(256,204);
    const QString DEFAULT_VIDEO_FILE_NAME = QString("bias_video");
    const QString DEFAULT_CONFIG_FILE_NAME = QString("bias_config");
    const QString CONFIG_FILE_EXTENSION = QString("json");
    const float DEFAULT_FORMAT7_PERCENT_SPEED = 100.0;
    const int CAMERA_LOCK_TRY_DT = 100;                  // mSec
    const int IMAGE_DISPLAY_CAMERA_LOCK_TRY_DT = int(0.5*1000.0/MAX_IMAGE_DISPLAY_FREQ);

    const unsigned int HTTP_SERVER_PORT_BEGIN = 5000;
    const unsigned int HTTP_SERVER_PORT_END = 20000;
    const unsigned int HTTP_SERVER_PORT_STEP = 10;
    const bool DEFAULT_HTTP_SERVER_ENABLED = true;

    const unsigned int ROI_BOUNDARY_LINE_WIDTH = 5;
    const QColor ROI_BOUNDARY_COLOR = QColor(255,0,0);

    QMap<VideoFileFormat, QString> createExtensionMap()
    {
        QMap<VideoFileFormat, QString> map;
        map.insert(VIDEOFILE_FORMAT_BMP,  QString("bmp"));
        map.insert(VIDEOFILE_FORMAT_AVI,  QString("avi"));
        map.insert(VIDEOFILE_FORMAT_FMF,  QString("fmf"));
        map.insert(VIDEOFILE_FORMAT_UFMF, QString("ufmf"));
        return map;
    };
    const QMap<VideoFileFormat, QString> VIDEOFILE_EXTENSION_MAP = createExtensionMap();

    const int COLORMAP_NONE = -1;
    QMap<int, QString>  createColorMapIntToStringMap()
    {
        QMap<int, QString> map;
        map.insert(COLORMAP_NONE, QString("None"));
        map.insert(cv::COLORMAP_AUTUMN, QString("Autumn"));
        map.insert(cv::COLORMAP_BONE, QString("Bone"));
        map.insert(cv::COLORMAP_JET, QString("Jet"));
        map.insert(cv::COLORMAP_WINTER, QString("Winter"));
        map.insert(cv::COLORMAP_RAINBOW, QString("Rainbow"));
        map.insert(cv::COLORMAP_OCEAN, QString("Ocean"));
        map.insert(cv::COLORMAP_SUMMER, QString("Summer"));
        map.insert(cv::COLORMAP_SPRING, QString("Spring"));
        map.insert(cv::COLORMAP_COOL, QString("Cool"));
        map.insert(cv::COLORMAP_HSV, QString("Hsv"));
        map.insert(cv::COLORMAP_PINK, QString("Pink"));
        map.insert(cv::COLORMAP_HOT, QString("Hot"));
        return map;
    }
    const QMap<int, QString> COLORMAP_INT_TO_STRING_MAP = createColorMapIntToStringMap();
    const int DEFAULT_COLORMAP_NUMBER = COLORMAP_NONE;

    // Debug files
    const QString DEBUG_DUMP_CAMERA_PROPS_FILE_NAME("bias_camera_props_dump.txt");

    // Public methods
    // ----------------------------------------------------------------------------------

    CameraWindow::CameraWindow(
            Guid cameraGuid, 
            unsigned int cameraNumber, 
            unsigned int numberOfCameras, 
            QWidget *parent
            ) : QMainWindow(parent)
    {
        setupUi(this);
        connectWidgets();
        initialize(cameraGuid, cameraNumber, numberOfCameras);
    }


    RtnStatus CameraWindow::connectCamera(bool showErrorDlg) 
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg;
        RtnStatus rtnStatus;

        if (connected_)
        {
            rtnStatus.message = QString("Camera already connected");
            return rtnStatus;
        }

        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                cameraPtr_ -> connect();

#ifdef WITH_FC2
                // WBD DEVEL TEMP
                // ------------------------------------------------------------
                //  TEMPORARY - set camera to known videomode and trigger type
                cameraPtr_ -> setVideoMode(VIDEOMODE_FORMAT7);
                cameraPtr_ -> setTriggerInternal();
                // ------------------------------------------------------------
#endif
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable to acquire camera lock");
            return rtnStatus; 
        }

        if (error)
        {
            QString msgTitle("Connection Error");
            QString msgText("Failed to connect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            if (showErrorDlg) 
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return rtnStatus;
        }
        connected_ = true;
        connectButtonPtr_ -> setText(QString("Disconnect"));

        updateStatusLabel();

        startButtonPtr_ -> setEnabled(true);
        menuCameraPtr_ -> setEnabled(true);

        updateCameraInfoMessage();

        updateAllMenus();

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus; 
    }


    RtnStatus CameraWindow::disconnectCamera(bool showErrorDlg)
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg; 
        RtnStatus rtnStatus;

        if (capturing_) 
        {
            stopImageCapture();
        }

        if (!connected_)
        {
            rtnStatus.success = true;
            rtnStatus.message = QString("Camera already disconnected");
            return rtnStatus;
        }

        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                cameraPtr_ -> disconnect();
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable to acquire camea lock");
            return rtnStatus;
        }

        if (error)
        {
            QString msgTitle("Disconnection Error");
            QString msgText("Failed to disconnect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            rtnStatus.success=false;
            rtnStatus.message=msgText;
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return rtnStatus;
        }
        connected_ = false;
        connectButtonPtr_ -> setText(QString("Connect"));

        //statusLabelPtr_ -> setText("Disconnected");
        updateStatusLabel();

        startButtonPtr_ -> setEnabled(false);
        menuCameraPtr_ -> setEnabled(false);
        updateCameraInfoMessage();
        setCaptureTimeLabel(0.0);
        updateAllMenus();

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::startImageCapture(bool showErrorDlg) 
    {
        RtnStatus rtnStatus;

        if (!connected_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to start image capture: not connected");
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return rtnStatus;
        }

        if (capturing_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to start image capture: capture already in progress");
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = true;
            rtnStatus.message = msgText;
            return rtnStatus;
        }

        frameCount_ = 0;
        timeStamp_ = 0.0;
        framesPerSec_ = 0.0;

        newImageQueuePtr_ -> clear();
        logImageQueuePtr_ -> clear();
        pluginImageQueuePtr_ -> clear();

        imageGrabberPtr_ = new ImageGrabber(
                cameraNumber_, 
                cameraPtr_, 
                newImageQueuePtr_, 
                this
                );
        imageGrabberPtr_ -> setAutoDelete(false);

        imageDispatcherPtr_ = new ImageDispatcher(
                logging_, 
                BiasPlugin::pluginsEnabled(),
                cameraNumber_,
                newImageQueuePtr_,
                logImageQueuePtr_,
                pluginImageQueuePtr_,
                this
                );
        imageDispatcherPtr_ -> setAutoDelete(false);

        connect(
                imageGrabberPtr_, 
                SIGNAL(startCaptureError(unsigned int, QString)),
                this,
                SLOT(startImageCaptureError(unsigned int, QString))
               );

        connect(
                imageGrabberPtr_,
                SIGNAL(captureError(unsigned int, QString)),
                this,
                SLOT(imageCaptureError(unsigned int, QString))
               );

        connect(
                imageGrabberPtr_,
                SIGNAL(stopCaptureError(unsigned int, QString)),
                this,
                SLOT(stopImageCaptureError(unsigned int, QString))
               );

        if (actionTimerEnabledPtr_ -> isChecked())
        {
            connect(
                    imageGrabberPtr_,
                    SIGNAL(startTimer()),
                    this,
                    SLOT(startCaptureDurationTimer())
                   );
        }


        threadPoolPtr_ -> start(imageGrabberPtr_);
        threadPoolPtr_ -> start(imageDispatcherPtr_);

        if (logging_)
        {
            // Create video writer based on video file format type
            std::shared_ptr<VideoWriter> videoWriterPtr; 
            QString videoFileFullPath = getVideoFileFullPathWithAutoNaming();

            // DEBUG
            // -------------------------------------------------------------------------------
            //std::cout << "videoFileFullPath: " << videoFileFullPath.toStdString() << std::endl;
            // --------------------------------------------------------------------------------

            switch (videoFileFormat_)
            {
                case VIDEOFILE_FORMAT_BMP:
                    videoWriterPtr = std::make_shared<VideoWriter_bmp>(
                            videoWriterParams_.bmp,
                            videoFileFullPath,
                            cameraNumber_
                            );
                    break;

                case VIDEOFILE_FORMAT_AVI:  
                    videoWriterPtr = std::make_shared<VideoWriter_avi>(
                            videoWriterParams_.avi,
                            videoFileFullPath,
                            cameraNumber_
                            );
                    break;

                case VIDEOFILE_FORMAT_FMF:
                    videoWriterPtr = std::make_shared<VideoWriter_fmf>(
                            videoWriterParams_.fmf,
                            videoFileFullPath,
                            cameraNumber_
                            );
                    break;

                case VIDEOFILE_FORMAT_UFMF:
                    videoWriterPtr = std::make_shared<VideoWriter_ufmf>(
                            videoWriterParams_.ufmf,
                            videoFileFullPath,
                            cameraNumber_
                            );
                    break;

                default:
                    videoWriterPtr = std::make_shared<VideoWriter>(
                            videoFileFullPath,
                            cameraNumber_
                            );
                    break;

            } // switch (videoFileFormat) 

            // Set output file
            videoWriterPtr -> setFileName(videoFileFullPath);
            videoWriterPtr -> setVersioning(autoNamingOptions_.includeVersionNumber);

            imageLoggerPtr_ = new ImageLogger(
                    cameraNumber_,
                    videoWriterPtr, 
                    logImageQueuePtr_, 
                    this
                    );
            imageLoggerPtr_ -> setAutoDelete(false);

            // Connect image logger error signals
            connect(
                    imageLoggerPtr_,
                    SIGNAL(imageLoggingError(unsigned int, QString)),
                    this,
                    SLOT(imageLoggingError(unsigned int, QString))
                   );

            connect(
                    videoWriterPtr.get(),
                    SIGNAL(imageLoggingError(unsigned int, QString)),
                    this,
                    SLOT(imageLoggingError(unsigned int, QString))
                   );

            threadPoolPtr_ -> start(imageLoggerPtr_);

        } // if (logging_)

        if (BiasPlugin::pluginsEnabled())
        {
            pluginHandlerPtr_ -> setCameraNumber(cameraNumber_);
            pluginHandlerPtr_ -> setImageQueue(pluginImageQueuePtr_);
            pluginHandlerPtr_ -> setPlugin(getCurrentPlugin());
            pluginHandlerPtr_ -> setAutoDelete(false);
            threadPoolPtr_ -> start(pluginHandlerPtr_);
        } 
        actionPluginsEnabledPtr_ -> setEnabled(false);

        // Set Capture start and stop time
        captureStartDateTime_ = QDateTime::currentDateTime();
        captureStopDateTime_ = captureStartDateTime_.addSecs(captureDurationSec_);

        // Update GUI widget for capturing state
        startButtonPtr_ -> setText(QString("Stop"));
        connectButtonPtr_ -> setEnabled(false);
        pluginActionGroupPtr_ -> setEnabled(false);
        updateStatusLabel();

        capturing_ = true;
        showCameraLockFailMsg_ = false;

        updateAllMenus();

        showCameraLockFailMsg_ = true;

        emit imageCaptureStarted(logging_);

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::stopImageCapture(bool showErrorDlg)
    {
        RtnStatus rtnStatus;

        if (!connected_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to stop image capture: camera not connected");
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return rtnStatus;
        }

        capturing_ = false;

        if (actionTimerEnabledPtr_ -> isChecked())
        {
            captureDurationTimerPtr_ -> stop();
        }

        // Note, image grabber and dispatcher are destroyed by the 
        // threadPool when their run methods exit.

        // Send stop singals to threads
        if (!imageGrabberPtr_.isNull())
        {
            imageGrabberPtr_ -> acquireLock();
            imageGrabberPtr_ -> stop();
            imageGrabberPtr_ -> releaseLock();
        }

        if (!imageDispatcherPtr_.isNull())
        {
            imageDispatcherPtr_ -> acquireLock();
            frameCount_ = imageDispatcherPtr_ -> getFrameCount();
            imageDispatcherPtr_ -> stop();
            imageDispatcherPtr_ -> releaseLock();

            newImageQueuePtr_ -> acquireLock();
            newImageQueuePtr_ -> signalNotEmpty();
            newImageQueuePtr_ -> releaseLock();
        }

        if (!imageLoggerPtr_.isNull())
        {
            imageLoggerPtr_ -> acquireLock();
            imageLoggerPtr_ -> stop();
            imageLoggerPtr_ -> releaseLock();

            logImageQueuePtr_ -> acquireLock();
            logImageQueuePtr_ -> signalNotEmpty();
            logImageQueuePtr_ -> releaseLock();
        }

        if (!pluginHandlerPtr_.isNull())
        {
            pluginHandlerPtr_ -> acquireLock();
            pluginHandlerPtr_ -> stop();
            pluginHandlerPtr_ -> releaseLock();

            pluginImageQueuePtr_ -> acquireLock();
            pluginImageQueuePtr_ -> signalNotEmpty();
            pluginImageQueuePtr_ -> releaseLock();
        }

        // Wait until threads are finished
        threadPoolPtr_ -> waitForDone();

        // Clear any stale data out of existing queues
        newImageQueuePtr_ -> acquireLock();
        newImageQueuePtr_ -> clear();
        newImageQueuePtr_ -> releaseLock();

        logImageQueuePtr_ -> acquireLock();
        logImageQueuePtr_ -> clear();
        logImageQueuePtr_ -> releaseLock();

        pluginImageQueuePtr_ -> acquireLock();
        pluginImageQueuePtr_ -> clear();
        pluginImageQueuePtr_ -> releaseLock();


        // Update data GUI information
        startButtonPtr_ -> setText(QString("Start"));
        connectButtonPtr_ -> setEnabled(true);
        actionPluginsEnabledPtr_ -> setEnabled(true);
        pluginActionGroupPtr_ -> setEnabled(true);

        updateStatusLabel();
        framesPerSec_ = 0.0;
        updateAllImageLabels();

        updateAllMenus();

        emit imageCaptureStopped();

        // Manually delete grabber, dispatcher and logger threads   
        // required as autoDelete is set to false
        delete imageGrabberPtr_;
        delete imageDispatcherPtr_;
        delete imageLoggerPtr_;

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::saveConfiguration(QString fileName, bool showErrorDlg)
    {
        // --------------------------------------------------------------------
        // TO DO  ... need to work on error checking. 
        // --------------------------------------------------------------------
        RtnStatus rtnStatus;
        QString msgTitle("Save Configuration Error");

        if (!connected_) 
        { 
            QString msgText("Unable to determine configuration: camera not connected");
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return rtnStatus; 
        }

        if (fileName.isEmpty() || fileName.isNull())
        {
            QString msgText("Unable to open file: no filename given");
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return rtnStatus;
        }

        QByteArray jsonConfig = getConfigurationJson(rtnStatus);
        if ( (!rtnStatus.success) || (jsonConfig.isEmpty()) ) 
        { 
            return rtnStatus; 
        }

        QByteArray jsonConfigPretty = prettyIndentJson(jsonConfig); 
        QFile configFile(fileName);

        bool ok = configFile.open(QIODevice::WriteOnly);
        if (!ok)
        {
            QString msgText = QString("Unable to open file %1").arg(fileName);
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return rtnStatus;
        }
        configFile.write(jsonConfigPretty);
        configFile.close(); 
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    } 
    

    RtnStatus CameraWindow::loadConfiguration(QString fileName, bool showErrorDlg) 
    {
        // --------------------------------------------------------------------
        // TO DO  ... need to add error checking. 
        // --------------------------------------------------------------------
        RtnStatus rtnStatus;
        QString msgTitle("Load Configuration Error");
        QFile configFile(fileName);

        if (!configFile.exists())
        {
            QString msgText = QString("Configuration file, %1, does not exist").arg(fileName);
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return rtnStatus;
        }

        bool ok = configFile.open(QIODevice::ReadOnly);
        if (!ok)
        {
            QString msgText = QString("Unable to open configuration file %1").arg(fileName);
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return rtnStatus;
        }
        QByteArray jsonConfig = configFile.readAll();
        configFile.close();
        rtnStatus = setConfigurationFromJson(jsonConfig,showErrorDlg);
        return rtnStatus;
    }


    QByteArray CameraWindow::getConfigurationJson(RtnStatus &rtnStatus, bool showErrorDlg)
    {
        // Get configuration map
        QVariantMap configurationMap = getConfigurationMap(rtnStatus, showErrorDlg);
        if (configurationMap.isEmpty())
        {
            QByteArray emptyByteArray = QByteArray();
            return emptyByteArray;
        }

        // Serialize configuration
        bool ok;
        QByteArray jsonConfig = QtJson::serialize(configurationMap,ok);
        if (!ok)
        {
            QString errMsgTitle("Save Configuration Error");
            QString errMsgText("Error serializing configuration");
            QMessageBox::critical(this, errMsgTitle, errMsgText);
            QByteArray emptyByteArray = QByteArray();
            return emptyByteArray;
        }
        return jsonConfig;
    }
      

    QVariantMap CameraWindow::getConfigurationMap(RtnStatus &rtnStatus, bool showErrorDlg)
    {

        if (!connected_) { 
            QVariantMap emptyMap = QVariantMap();
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to get configuration: camera is not connected");
            return emptyMap; 
        }

        QVariantMap configurationMap;

        // Get configuration values from camera
        QString vendorName;
        QString modelName;
        QString guidString;
        PropertyList propList;
        VideoMode videoMode;
        FrameRate frameRate;
        TriggerType trigType;
        Format7Settings format7Settings;
        QString errorMsg;
        bool error = false;
        unsigned int errorId;

        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            { 
                vendorName = QString::fromStdString(cameraPtr_ -> getVendorName());
                modelName = QString::fromStdString(cameraPtr_ -> getModelName());
                guidString = QString::fromStdString((cameraPtr_ -> getGuid()).toString());
                propList = cameraPtr_ -> getListOfProperties();
                videoMode = cameraPtr_ -> getVideoMode();
                frameRate = cameraPtr_ -> getFrameRate();
                trigType = cameraPtr_ -> getTriggerType();
                format7Settings = cameraPtr_ -> getFormat7Settings();
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable acquire camera lock");
            return configurationMap;
        }

        if (error)
        {
            QString msgTitle("Camera Query Error");
            QString msgText("Error retrieving values from camera.\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            if (showErrorDlg) 
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            QVariantMap emptyMap = QVariantMap();
            rtnStatus.success = false;
            rtnStatus.message = msgText;
            return emptyMap;
        }

        QVariantMap cameraMap;
        cameraMap.insert("vendor", vendorName); 
        cameraMap.insert("model", modelName);
        cameraMap.insert("guid", guidString);

        // Add camera properties
        QVariantMap cameraPropMap;
        for (
                PropertyList::iterator it = propList.begin();
                it != propList.end();
                it++
            )
        {
            Property prop = *it;
            QString propName = QString::fromStdString(getPropertyTypeString(prop.type));
            QVariantMap valueMap;
            valueMap.insert("present", prop.present);
            valueMap.insert("absoluteControl", prop.absoluteControl);
            valueMap.insert("onePush", prop.onePush);
            valueMap.insert("on", prop.on);
            valueMap.insert("autoActive", prop.autoActive);
            if (prop.type == PROPERTY_TYPE_WHITE_BALANCE)
            {
                valueMap.insert("valueRed", prop.valueA);
                valueMap.insert("valueBlue", prop.valueB);
            }
            else
            {
                valueMap.insert("value", prop.value);
            }
            valueMap.insert("absoluteValue", double(prop.absoluteValue));
            QString camelCaseName = propNameToCamelCase(propName);
            cameraPropMap.insert(camelCaseName, valueMap);
        }
        cameraMap.insert("properties", cameraPropMap);

        // Add videomode, framerate and trigger information
        QString videoModeString = QString::fromStdString(getVideoModeString(videoMode));
        cameraMap.insert("videoMode", videoModeString);
        QString frameRateString = QString::fromStdString(getFrameRateString(frameRate));
        cameraMap.insert("frameRate", frameRateString);
        QString trigTypeString = QString::fromStdString(getTriggerTypeString(trigType));
        cameraMap.insert("triggerType", trigTypeString);

        // Create format7 settings map
        QVariantMap format7SettingsMap;
        std::string imageModeStdString = getImageModeString(format7Settings.mode);
        QString imageModeString = QString::fromStdString(imageModeStdString);
        format7SettingsMap.insert("mode", imageModeString);
        std::string pixFormatStdString = getPixelFormatString(format7Settings.pixelFormat);
        QString pixFormatString = QString::fromStdString(pixFormatStdString);
        format7SettingsMap.insert("pixelFormat",pixFormatString);

        QVariantMap roiMap;
        roiMap.insert("offsetX", format7Settings.offsetX);
        roiMap.insert("offsetY", format7Settings.offsetY);
        roiMap.insert("width", format7Settings.width);
        roiMap.insert("height", format7Settings.height);
        format7SettingsMap.insert("roi", roiMap);
        cameraMap.insert("format7Settings", format7SettingsMap);
        configurationMap.insert("camera", cameraMap);

        // Add logging information
        QVariantMap loggingMap;
        loggingMap.insert("enabled", logging_);
        loggingMap.insert("format", VIDEOFILE_EXTENSION_MAP[videoFileFormat_]);
        loggingMap.insert("directory", currentVideoFileDir_.canonicalPath());
        loggingMap.insert("fileName", currentVideoFileName_);
        
        // Add logging configuration 
        QVariantMap loggingSettingsMap;
        
        QVariantMap bmpSettingsMap;
        bmpSettingsMap.insert("frameSkip", videoWriterParams_.bmp.frameSkip);
        loggingSettingsMap.insert("bmp", bmpSettingsMap);

        QVariantMap aviSettingsMap;
        aviSettingsMap.insert("frameSkip", videoWriterParams_.avi.frameSkip);
        aviSettingsMap.insert("codec", videoWriterParams_.avi.codec);
        loggingSettingsMap.insert("avi", aviSettingsMap);

        QVariantMap fmfSettingsMap;
        fmfSettingsMap.insert("frameSkip", videoWriterParams_.fmf.frameSkip);
        loggingSettingsMap.insert("fmf", fmfSettingsMap);

        QVariantMap ufmfSettingsMap;
        ufmfSettingsMap.insert("frameSkip", videoWriterParams_.ufmf.frameSkip);
        ufmfSettingsMap.insert("backgroundThreshold", videoWriterParams_.ufmf.backgroundThreshold);
        ufmfSettingsMap.insert("boxLength", videoWriterParams_.ufmf.boxLength);
        ufmfSettingsMap.insert("medianUpdateCount", videoWriterParams_.ufmf.medianUpdateCount);
        ufmfSettingsMap.insert("medianUpdateInterval", videoWriterParams_.ufmf.medianUpdateInterval);
        ufmfSettingsMap.insert("compressionThreads", videoWriterParams_.ufmf.numberOfCompressors);

        QVariantMap ufmfDilateMap;
        ufmfDilateMap.insert("on", videoWriterParams_.ufmf.dilateState);
        ufmfDilateMap.insert("windowSize", videoWriterParams_.ufmf.dilateWindowSize);
        ufmfSettingsMap.insert("dilate", ufmfDilateMap);
        
        loggingSettingsMap.insert("ufmf", ufmfSettingsMap);
        loggingMap.insert("settings", loggingSettingsMap);

        // Add logging auto-naming options
        QVariantMap autoNamingOptionsMap;
        autoNamingOptionsMap.insert("includeCameraIdentifier", autoNamingOptions_.includeCameraIdentifier);
        autoNamingOptionsMap.insert("includeTimeAndDate", autoNamingOptions_.includeTimeAndDate);
        autoNamingOptionsMap.insert("includeVersionNumber", autoNamingOptions_.includeVersionNumber);
        autoNamingOptionsMap.insert("cameraIdentifier", autoNamingOptions_.getCameraIdentifierString());
        autoNamingOptionsMap.insert("timeAndDateFormat", autoNamingOptions_.timeAndDateFormat);
        loggingMap.insert("autoNamingOptions", autoNamingOptionsMap);
        configurationMap.insert("logging", loggingMap);

        // Add Timer configuration
        QVariantMap timerMap;
        timerMap.insert("enabled", actionTimerEnabledPtr_ -> isChecked());
        QVariantMap timerSettingsMap;
        timerSettingsMap.insert("duration", qulonglong(captureDurationSec_));
        timerMap.insert("settings", timerSettingsMap);
        configurationMap.insert("timer", timerMap);

        // Add display configuration
        QVariantMap displayMap;
        QVariantMap orientationMap;
        orientationMap.insert("flipVertical", flipVert_ );
        orientationMap.insert("flipHorizontal", flipHorz_);
        displayMap.insert("orientation", orientationMap);
        displayMap.insert("rotation", (unsigned int)(imageRotation_));
        displayMap.insert("updateFrequency", imageDisplayFreq_);
        displayMap.insert("colorMap",COLORMAP_INT_TO_STRING_MAP[colorMapNumber_]);
        configurationMap.insert("display", displayMap);

        // Add server configuration
        QVariantMap serverMap;
        serverMap.insert("enabled",actionServerEnabledPtr_ -> isChecked());
        serverMap.insert("port", httpServerPort_);
        configurationMap.insert("server", serverMap);

        // Add configuration configuration
        QVariantMap configFileMap;
        configFileMap.insert("directory", currentConfigFileDir_.canonicalPath());
        configFileMap.insert("fileName", currentConfigFileName_);
        configurationMap.insert("configuration", configFileMap);

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return configurationMap;
    }


    RtnStatus CameraWindow::setConfigurationFromJson(
            QByteArray jsonConfig, 
            bool showErrorDlg
            )
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error");

        bool ok;
        QVariantMap configMap = QtJson::parse(QString(jsonConfig), ok).toMap();
        if (!ok)
        {
            QString errMsgText("Error loading configuration - "); 
            errMsgText += "unable to parse json.";
            if (showErrorDlg) 
            {
                QMessageBox::critical(this, errMsgTitle, errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        QVariantMap oldConfigMap = getConfigurationMap(rtnStatus);

        // --------------------------------------------------------------------------
        //TODO: need to clean this up ... two functions for setting configuration.
        //---------------------------------------------------------------------------
        
        //rtnStatus = setConfigurationFromMap(configMap,showErrorDlg); 
        rtnStatus = setConfigurationFromMap_Lenient(configMap,oldConfigMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            QString origErrMsg = rtnStatus.message;

            // Something went wrong - try to revert to old configuration
            rtnStatus = setConfigurationFromMap(oldConfigMap,showErrorDlg);
            if (!rtnStatus.success)
            {
                QString errMsgText("Unable to revert to previous configuration");
                if (showErrorDlg)
                {
                    QMessageBox::critical(this, errMsgTitle, errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.message = origErrMsg;
                return rtnStatus;
            }
        }
        updateAllMenus();
        updateStatusLabel();
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setConfigurationFromMap(
            QVariantMap configMap, 
            bool showErrorDlg
            )
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error");

        // Check if camera is capturing
        // ----------------------------
        if (capturing_)
        {
            QString errMsgText("unable to set configuration - capturing");
            return onError(errMsgText, errMsgTitle, showErrorDlg);
        }

        // Set camera properties, videomode, etc.
        // --------------------------------------
        QVariantMap cameraMap = configMap["camera"].toMap();
        if (cameraMap.isEmpty())
        {
            QString errMsgText("Camera configuration is empty");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setCameraFromMap(cameraMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set logging configuration
        // --------------------------
        QVariantMap loggingMap = configMap["logging"].toMap();
        if (loggingMap.isEmpty())
        {
            QString errMsgText("Logging configuration is empty");
            if (showErrorDlg) 
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setLoggingFromMap(loggingMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set timer configuration
        // ------------------------
        QVariantMap timerMap = configMap["timer"].toMap();
        if (timerMap.isEmpty())
        {
            QString errMsgText("Timer configuration is empty");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setTimerFromMap(timerMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set display configuration
        // --------------------------
        QVariantMap displayMap = configMap["display"].toMap();
        if (displayMap.isEmpty())
        {
            QString errMsgText("Display configuration is empty");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setDisplayFromMap(displayMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set external control server configuration
        // -----------------------------------------
        QVariantMap serverMap = configMap["server"].toMap();
        if (serverMap.isEmpty())
        {
            QString errMsgText("Server configuration is empty");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setServerFromMap(serverMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set configuration file configuraiton 
        // -------------------------------------
        QVariantMap configFileMap = configMap["configuration"].toMap();
        if (configFileMap.isEmpty())
        {
            QString errMsgText("Configuration file information is empty");
            if (showErrorDlg) 
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setConfigFileFromMap(configFileMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }

    // same as setConfigurationFromMap, but uses the old configuration if 
    // fields are missing from the new one
    RtnStatus CameraWindow::setConfigurationFromMap_Lenient( 
            QVariantMap configMap, 
            QVariantMap oldConfigMap,
            bool showErrorDlg
            )
    {

        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error");

        // Check if camera is capturing
        // ----------------------------
        if (capturing_)
        {
            QString errMsgText("unable to set configuration - capturing");
            return onError(errMsgText, errMsgTitle, showErrorDlg);
        }

        // Set camera properties, videomode, etc.
        // --------------------------------------
        QVariantMap cameraMap = configMap["camera"].toMap();
        if (cameraMap.isEmpty())
        {
            cameraMap = oldConfigMap["camera"].toMap();
        }
        rtnStatus = setCameraFromMap(cameraMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set logging configuration
        // --------------------------
        QVariantMap loggingMap = configMap["logging"].toMap();
        if (loggingMap.isEmpty())
        {
            loggingMap = oldConfigMap["logging"].toMap();
        }
        rtnStatus = setLoggingFromMap(loggingMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set timer configuration
        // ------------------------
        QVariantMap timerMap = configMap["timer"].toMap();
        if (timerMap.isEmpty())
        {
            timerMap = oldConfigMap["timer"].toMap();
        }
        rtnStatus = setTimerFromMap(timerMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set display configuration
        // --------------------------
        QVariantMap displayMap = configMap["display"].toMap();
        if (displayMap.isEmpty())
        {
            displayMap = oldConfigMap["display"].toMap();
        }
        rtnStatus = setDisplayFromMap(displayMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set external control server configuration
        // -----------------------------------------
        QVariantMap serverMap = configMap["server"].toMap();
        if (serverMap.isEmpty())
        {
            serverMap = oldConfigMap["server"].toMap();
        }
        rtnStatus = setServerFromMap(serverMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Set configuration file configuraiton 
        // -------------------------------------
        QVariantMap configFileMap = configMap["configuration"].toMap();
        if (configFileMap.isEmpty())
        {
            configFileMap = oldConfigMap["configuration"].toMap();
        }
        rtnStatus = setConfigFileFromMap(configFileMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");

        return rtnStatus;
    }

    RtnStatus CameraWindow::enableLogging(bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString msgTitle("Logging Enable Error");

        if (actionLoggingEnabledPtr_ -> isChecked())
        {
            QString msgText("Logging already enabled");
            if (showErrorDlg) 
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = true;
            rtnStatus.message = msgText;
        }
        else 
        {
            if (!haveDefaultVideoFileDir_)
            {
                QString msgText("Unable to determine default location for video files");
                if (showErrorDlg)
                {
                    QMessageBox::critical(this, msgTitle, msgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = msgText;
            }
            else if (capturing_)
            {
                QString msgText("Unable to enable logging: capturing images");
                if (showErrorDlg)
                {
                    QMessageBox::critical(this, msgTitle, msgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = msgText;
            }
            else
            {
                logging_ = true;
                actionLoggingEnabledPtr_ -> setChecked(true);
                rtnStatus.success = true;
                rtnStatus.message = QString("");
            }
        }
        return rtnStatus;
    }


    RtnStatus CameraWindow::disableLogging(bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString msgTitle("Logging Disable Error");

        if (!(actionLoggingEnabledPtr_ -> isChecked()))
        {
            QString msgText("Logging already disabled");
            if (showErrorDlg)
            {
                QMessageBox::critical(this, msgTitle, msgText);
            }
            rtnStatus.success = true;
            rtnStatus.message = msgText;
        }
        else
        {
            if (capturing_)
            {
                QString msgText("Unable to disable logging: capturing images");
                if (showErrorDlg)
                {
                    QMessageBox::critical(this, msgTitle, msgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = msgText;
            }
            else
            {
                logging_ = false;
                actionLoggingEnabledPtr_ -> setChecked(false);
                rtnStatus.success = true;
                rtnStatus.message = QString("");
            }
        }
        return rtnStatus;
    }

    
    QString CameraWindow::getCameraGuidString(RtnStatus &rtnStatus)
    {
        if (!connected_)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to get camera Guid: camera not connected");
            QString emptyString;
            return emptyString;
        }

        Guid guid = cameraPtr_ -> getGuid();
        QString guidString = QString::fromStdString(guid.toString());
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return guidString;
    }


    RtnStatus CameraWindow::setVideoFile(QString videoFileString)
    {
        RtnStatus rtnStatus;

        if (capturing_ && logging_)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to set video file: recording in progress");
            return rtnStatus;
        }
        
        QFileInfo videoFileInfo = QFileInfo(videoFileString);
        QDir videoFileDir = videoFileInfo.dir();
        QString videoFileName = videoFileInfo.baseName();

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        if (videoFileName.isEmpty())
        {
            if (currentVideoFileName_.isEmpty())
            {
                videoFileName = DEFAULT_VIDEO_FILE_NAME;
            }
            else
            {
                videoFileName = currentVideoFileName_;
            }
            videoFileDir = currentVideoFileDir_;
            rtnStatus.success = false;
            rtnStatus.message = QString("Video file name is missing");
        }

        if (!videoFileDir.exists())
        {
            bool ok = videoFileDir.mkpath(".");
            if (!ok)
            {
                QString msgText("Could not create videoFile directory");
                if (!rtnStatus.success)
                {
                    rtnStatus.message = QString("%1, %2").arg(rtnStatus.message).arg(msgText);
                }
                else
                {
                    rtnStatus.success = false;
                    rtnStatus.message = msgText;
                }
            }
        }
        currentVideoFileDir_ = videoFileDir;
        currentVideoFileName_ = videoFileName;
        // DEBUG
        // ---------------------------------------------------------------------------------------------
        //std::cout << "currentVideoFileDir:  " << currentVideoFileDir_.path().toStdString() << std::endl;
        //std::cout << "currentVideoFileName: " << currentVideoFileName_.toStdString() << std::endl;
        // ---------------------------------------------------------------------------------------------
        return rtnStatus;
    }


    void CameraWindow::setUserCameraName(QString cameraName)
    {
        RtnStatus rtnStatus;
        userCameraName_ = cameraName;
        updateWindowTitle();
    }


    RtnStatus CameraWindow::setWindowGeometry(QRect windowGeom)
    {
        RtnStatus rtnStatus;
        if (!windowGeom.isValid())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("window geometry is not a valid rectangle");
            return rtnStatus;
        }

        QPointer<QDesktopWidget> desktopWidgetPtr = QApplication::desktop();
        int screenNumber = desktopWidgetPtr -> screenNumber();
        QRect desktopGeom = desktopWidgetPtr -> availableGeometry(screenNumber);
        if (!desktopGeom.contains(windowGeom))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("window geometery is not contain within desktop");
            return rtnStatus;
        }
        setGeometry(windowGeom);
        return rtnStatus;
    }


    RtnStatus CameraWindow::setWindowGeometryFromJson(QByteArray jsonGeomArray)
    {
        RtnStatus rtnStatus;

        bool ok;
        QString jsonGeomString = QString(jsonGeomArray);
        QVariantMap windowGeomMap = QtJson::parse(jsonGeomString, ok).toMap();

        QList<QString> nameList;
        nameList << QString("x") << QString("y") << QString("width") << QString("height");

        QList<QString>::iterator it;
        QMap<QString, int> nameToValueMap;
        for (it=nameList.begin(); it!=nameList.end(); it++)
        {
            QString name = *it;
            if (!windowGeomMap.contains(name))
            {
                QString errMsgText; 
                errMsgText = QString("Set Window Geometry: '%1' position not present").arg(name);
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            if (!windowGeomMap[name].canConvert<int>())
            {
                QString errMsgText;
                errMsgText = QString("Set Window Geometry: unable to convert '%1' to int").arg(name);
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            nameToValueMap[name] = windowGeomMap[name].toInt();
        }
        
        QRect windowGeom(
                nameToValueMap["x"], 
                nameToValueMap["y"], 
                nameToValueMap["width"], 
                nameToValueMap["height"]
                );
        rtnStatus = setWindowGeometry(windowGeom);

        return rtnStatus;
    }


    QRect CameraWindow::getWindowGeometry()
    {
        return geometry();
    }


    void CameraWindow::setCaptureDuration(unsigned long duration)
    {
        captureDurationSec_ = duration;
        emit timerDurationChanged(duration);
    }


    RtnStatus CameraWindow::setCurrentPlugin(QString pluginName)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        bool pluginNameFound = false;
        for (auto itemPluginName : pluginMap_.keys())
        {
            if (itemPluginName == pluginName)
            {
                pluginNameFound = true;
                pluginMap_[pluginName] -> setActive(true);; 
                pluginActionMap_[pluginName] -> setChecked(true);
                updateTimerMenu();
            }
        }
        if (!pluginNameFound)
        {
            rtnStatus.success = false; 
            rtnStatus.message = QString("unable to find plugin with name %1").arg(pluginName);
        }
        return rtnStatus;
    } 


    QString CameraWindow::getCurrentPluginName(RtnStatus &rtnStatus)
    {
        QString currentPluginName = QString("");
        QPointer<QAction> currentActionPtr = pluginActionGroupPtr_ -> checkedAction();
        if (!currentActionPtr.isNull())
        {
            currentPluginName = currentActionPtr -> data().toString();
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("no plugin is checked");
        }
        return currentPluginName;
    }

    RtnStatus CameraWindow::runPluginCmd(QByteArray jsonPluginCmdArray, bool showErrorDlg)
    {
        qDebug() << __PRETTY_FUNCTION__;

        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        QString errMsgTitle("Plugin Run Command Error");

        // Convert json bytearray to QVariant map
        bool ok;
        QVariantMap pluginCmdMap = QtJson::parse(QString(jsonPluginCmdArray), ok).toMap();
        if (!ok)
        {
            QString errMsgText("error parsing plugin command - "); 
            errMsgText += "unable to parse json.";
            if (showErrorDlg) 
            {
                QMessageBox::critical(this, errMsgTitle, errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Get plugin name
        if (!pluginCmdMap.contains("plugin"))
        {
            QString errMsgText("runPluginCmd: plugin name not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!pluginCmdMap["plugin"].canConvert<QString>())
        {
            QString errMsgText("runPluginCmd: unable to convert plugin name to string");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString cmdPluginName  = pluginCmdMap["plugin"].toString();

        // Try to find plugin with matching name
        bool nameFound = false;
        for (auto pluginName : pluginMap_.keys())
        {
            if (pluginName == cmdPluginName)
            {
                nameFound = true;
                qDebug() << "found matching name: " << pluginName;
                rtnStatus = pluginMap_[pluginName] -> runCmdFromMap(pluginCmdMap,showErrorDlg);
            }
        }
        if (!nameFound)
        {
            qDebug() << "name not found";
        }


        return rtnStatus;
    }


    QVariantMap CameraWindow::getWindowGeometryMap()
    {
        QVariantMap windowGeomMap;
        QRect windowGeom = getWindowGeometry();
        windowGeomMap.insert("x", windowGeom.x());
        windowGeomMap.insert("y", windowGeom.y());
        windowGeomMap.insert("width", windowGeom.width());
        windowGeomMap.insert("height", windowGeom.height());
        return windowGeomMap;
    }


    QString CameraWindow::getVideoFileFullPath()
    {
        QString fileExtension;
        if (videoFileFormat_ != VIDEOFILE_FORMAT_BMP)
        {
            fileExtension = VIDEOFILE_EXTENSION_MAP[videoFileFormat_];
        }
        else
        {
            fileExtension = "";

        }
        QString fileName = currentVideoFileName_;
        if (!fileExtension.isEmpty())
        {
            fileName +=  "." + fileExtension;
        }
        QFileInfo videoFileInfo(currentVideoFileDir_, fileName);
        QString videoFileFullPath = videoFileInfo.absoluteFilePath();
        return videoFileFullPath;
    }

    QDir CameraWindow::getVideoFileDir()
    {
        return currentVideoFileDir_;
    }


    double CameraWindow::getTimeStamp()
    {
        return timeStamp_;
    }


    double CameraWindow::getFramesPerSec()
    {
        return framesPerSec_;
    }


    unsigned long CameraWindow::getFrameCount()
    {
        return frameCount_;
    }


    float CameraWindow::getFormat7PercentSpeed()
    {
        return format7PercentSpeed_;
    }


    bool CameraWindow::isConnected()
    {
        return connected_;
    }


    bool CameraWindow::isCapturing()
    {
        return capturing_;
    }

    
    bool CameraWindow::isLoggingEnabled()
    {
        return logging_;
    }

    // Protected methods
    // ----------------------------------------------------------------------------------

    void CameraWindow::showEvent(QShowEvent *event)
    {
        resizeAllImageLabels();
    }


    void CameraWindow::resizeEvent(QResizeEvent *event)
    {
        resizeAllImageLabels();
    }


    void CameraWindow::closeEvent(QCloseEvent *event)
    {
        if (capturing_)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Close Request");
            msgBox.setText("The application is currently capturing images:");
            msgBox.setInformativeText("Do you want to stop capture and close the window?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Cancel)
            {
                event -> ignore();
                return;
            }
            else 
            {
                stopImageCapture();
            }
        }

        if (connected_)
        {
            disconnectCamera();
        }

        event -> accept();
    }


    // Private slots
    // ----------------------------------------------------------------------------------

    void CameraWindow::startCaptureDurationTimer()
    {
        if (actionTimerEnabledPtr_ -> isChecked())
        {
            captureStartDateTime_ = QDateTime::currentDateTime();
            captureStopDateTime_ = captureStartDateTime_.addSecs(captureDurationSec_);
            captureDurationTimerPtr_ -> start();
        }
    }


    void CameraWindow::connectButtonClicked()
    {
        (!connected_) ? connectCamera() : disconnectCamera();
    }


    void CameraWindow::startButtonClicked()
    { 
        (!capturing_) ? startImageCapture() : stopImageCapture();
    }
   

    void CameraWindow::updateDisplayOnTimer()
    {
        //std::cout << "update display on timer" << std::endl;

        if (capturing_) 
        {
            bool haveNewImage = false;
            cv::Mat cameraImageMat;

            // Get information from image dispatcher
            // -------------------------------------------------------------------

            if (imageDispatcherPtr_ -> tryLock(IMAGE_DISPLAY_CAMERA_LOCK_TRY_DT))
            {
                cameraImageMat = imageDispatcherPtr_ -> getImage();
                framesPerSec_ = imageDispatcherPtr_ -> getFPS();
                timeStamp_ = imageDispatcherPtr_ -> getTimeStamp();
                frameCount_ = imageDispatcherPtr_ -> getFrameCount();
                imageDispatcherPtr_ -> releaseLock();
                haveNewImage = true;
            }
            // -------------------------------------------------------------------

            if (haveNewImage)
            {
                cv::Mat histMat = calcHistogram(cameraImageMat);
                cv::Size imgSize = cameraImageMat.size();
                if (colorMapNumber_ != COLORMAP_NONE)
                {
                    cv::applyColorMap(cameraImageMat,cameraImageMat, colorMapNumber_);
                }
                QImage img = matToQImage(cameraImageMat);


                // Set pixmaps and update image labels - note need to add pluginPixmap
                if (!img.isNull()) 
                {
                    previewPixmapOriginal_ = QPixmap::fromImage(img);
                    haveImagePixmap_ = true;
                }

                // Update status message
                QString statusMsg = QString().sprintf("%dx%d", imgSize.width, imgSize.height);
                statusMsg += QString().sprintf(",  %1.1f fps", framesPerSec_);
                if ((logging_) && (!imageLoggerPtr_.isNull()))
                {
                    imageLoggerPtr_ -> acquireLock();
                    unsigned int logQueueSize = imageLoggerPtr_ -> getLogQueueSize();
                    imageLoggerPtr_ -> releaseLock();
                    statusMsg += QString(",  log queue size = %1").arg(logQueueSize);
                }
                updateStatusLabel(statusMsg);

                // Set update capture time 
                QDateTime currentDateTime = QDateTime::currentDateTime();
                qint64 captureDt = currentDateTime.toMSecsSinceEpoch();
                captureDt -= captureStartDateTime_.toMSecsSinceEpoch();
                if ( (actionTimerEnabledPtr_ -> isChecked() )  && !(captureDurationTimerPtr_ -> isActive()) )
                {
                    setCaptureTimeLabel(0.0);
                }
                else
                {
                    setCaptureTimeLabel(double(1.0e-3*captureDt));
                }
                updateHistogramPixmap(histMat);
            }

        } // if (capturing_)


        // Update plugin preview
        if ((BiasPlugin::pluginsEnabled()) && (tabWidgetPtr_ -> currentWidget() == pluginPreviewTabPtr_))
        {
            bool haveNewImage = false;
            cv::Mat pluginImageMat;

            if (!pluginHandlerPtr_.isNull())
            {
                if (pluginHandlerPtr_ -> tryLock(IMAGE_DISPLAY_CAMERA_LOCK_TRY_DT))
                {
                    pluginImageMat = pluginHandlerPtr_ -> getImage();
                    pluginHandlerPtr_ -> releaseLock();
                    haveNewImage = true;
                }
            }

            if (haveNewImage)
            {

                cv::Size pluginImageSize = pluginImageMat.size();
                QImage pluginImage  = matToQImage(pluginImageMat);

                if (!pluginImage.isNull())
                {
                    pluginPixmapOriginal_ = QPixmap::fromImage(pluginImage);
                }
            }
        }
       
        updateAllImageLabels();
    }


    void CameraWindow::checkDurationOnTimer()
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        if (currentDateTime >= captureStopDateTime_)
        {
            stopImageCapture();
        }
    }


    void CameraWindow::tabWidgetChanged(int index)
    {
        updateAllImageLabels();
    }


    void CameraWindow::startImageCaptureError(unsigned int errorId, QString errorMsg)
    {
        stopImageCapture();
        QString msgTitle("Start Image Capture Error");
        QString msgText("Failed to start image capture\n\nError ID: ");
        msgText += QString::number(errorId);
        msgText += "\n\n";
        msgText += errorMsg;
        QMessageBox::critical(this, msgTitle, msgText);
    }


    void CameraWindow::stopImageCaptureError(unsigned int errorId, QString errorMsg)
    {
        QString msgTitle("Stop Image Capture Error");
        QString msgText("Failed to stop image capture\n\nError ID: ");
        msgText += QString::number(errorId);
        msgText += "\n\n";
        msgText += errorMsg;
        QMessageBox::critical(this, msgTitle, msgText);
    }

    
    void CameraWindow::imageCaptureError(unsigned int errorId, QString errorMsg)
    {
        stopImageCapture();
        QString msgTitle("Image Capture Error");
        QString msgText("Error ID: ");
        msgText += QString::number(errorId);
        msgText += "\n\n";
        msgText += errorMsg;
        QMessageBox::critical(this, msgTitle, msgText);
    }


    void CameraWindow::imageLoggingError(unsigned int errorId, QString errorMsg)
    {
        stopImageCapture();
        QString msgTitle("Image Logging Error");
        QString msgText("image logging has failed\n\nError ID: ");
        msgText += QString::number(errorId);
        msgText += "\n\n";
        msgText += errorMsg;
        QMessageBox::critical(this, msgTitle, msgText);
    }


    void CameraWindow::actionFileLoadConfigTriggered()
    {
        QString configFileFullPath = getConfigFileFullPath();

        // Query user for desired video filename and directory
        QString configFileString = QFileDialog::getOpenFileName(
                this, 
                QString("Load Configuration File"),
                configFileFullPath
                );

        if (configFileString.isEmpty())
        {
            return;
        }
        loadConfiguration(configFileString);
    }


    void CameraWindow::actionFileSaveConfigTriggered()
    {
        QString configFileFullPath = getConfigFileFullPath();

        // Query user for desired video filename and directory
        QString configFileString = QFileDialog::getSaveFileName(
                this, 
                QString("Save Configuration File"),
                configFileFullPath
                );
        QFileInfo configFileInfo = QFileInfo(configFileString);

        QDir configFileDir = configFileInfo.dir();
        QString configFileName = configFileInfo.baseName();


        // Check return results and assign values
        if (configFileName.isEmpty())
        {
            if (currentConfigFileName_.isEmpty())
            {
                configFileName = DEFAULT_VIDEO_FILE_NAME;
            }
            else
            {
                configFileName = currentConfigFileName_;
            }
            configFileDir = currentConfigFileDir_;
        }
        if (!configFileDir.exists())
        {
            configFileDir = defaultConfigFileDir_;
        }

        currentConfigFileDir_ = configFileDir;
        currentConfigFileName_ = configFileName;
        configFileFullPath = getConfigFileFullPath();
        saveConfiguration(configFileFullPath);

        //std::cout << "dir:  " << currentConfigFileDir_.absolutePath().toStdString() << std::endl;
        //std::cout << "file: " << currentConfigFileName_.toStdString() << std::endl;

    }


    void CameraWindow::actionFileHideWindowTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Hide not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionCameraInfoTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Camera info not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionCameraFormat7SettingsTriggered()
    {
        // If format7 settings dialog doesn't exist create it otherwise raise
        if (format7SettingsDialogPtr_.isNull()) 
        {
            format7SettingsDialogPtr_ = new Format7SettingsDialog(
                    cameraPtr_, 
                    capturing_, 
                    logging_,
                    this 
                    );

            connect(
                    format7SettingsDialogPtr_,
                    SIGNAL(roiEnableStateChanged()),
                    this,
                    SLOT(format7RoiEnableStateChanged())
                   );

            format7SettingsDialogPtr_ -> show();

        }
        else
        {
            format7SettingsDialogPtr_ -> raise();
        }
    }


    void CameraWindow::format7RoiEnableStateChanged()
    {
        haveImagePixmap_ = false;
    }

    
    void CameraWindow::actionCameraTriggerExternalTriggered()
    {
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            cameraPtr_ -> setTriggerExternal();
            cameraPtr_ -> releaseLock();
        }
        else
        {
            actionCameraTriggerExternalPtr_ -> setChecked(false);
            actionCameraTriggerInternalPtr_ -> setChecked(true);
            if (showCameraLockFailMsg_)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
        }
    }


    void CameraWindow::actionCameraTriggerInternalTriggered()
    {
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            cameraPtr_ -> setTriggerInternal();
            cameraPtr_ -> releaseLock();
        }
        else
        {
            actionCameraTriggerExternalPtr_ -> setChecked(true);
            actionCameraTriggerInternalPtr_ -> setChecked(false);
            if (showCameraLockFailMsg_)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
        }
    }


    void CameraWindow::actionLoggingEnabledTriggered()
    {
        if (actionLoggingEnabledPtr_ -> isChecked())
        {
            if (haveDefaultVideoFileDir_) {
                logging_ = true; 
            }
            else
            {
                actionLoggingEnabledPtr_ -> setChecked(false);
                logging_ = false;
                QString msgTitle("Logging Enable Error");
                QString msgText("Unable to determine default location for video files.");
                QMessageBox::critical(this, msgTitle, msgText);
            }
        }
        else
        {
            logging_ = false;
        }
        updateStatusLabel();
    }


    void CameraWindow::actionLoggingVideoFileTriggered()
    {
        // Get current video filename
        if (!currentVideoFileDir_.exists()) 
        {
            currentVideoFileDir_ = defaultVideoFileDir_;
        }
        QString videoFileFullPath = getVideoFileFullPath();
        QFileInfo videoFileInfo(videoFileFullPath);

        // Query user for desired video filename and directory
        QString videoFileString = QFileDialog::getSaveFileName(
                this, 
                QString("Select Video File"),
                videoFileInfo.absoluteFilePath()
                );

        setVideoFile(videoFileString);
    }
    

    void CameraWindow::actionLoggingSettingsTriggered()
    {
        // If logging settings dialog does exist create it otherwise raise
        if (loggingSettingsDialogPtr_.isNull()) 
        {
            loggingSettingsDialogPtr_ = new LoggingSettingsDialog(
                    videoWriterParams_,
                    this
                    );

            connect(
                    loggingSettingsDialogPtr_,
                    SIGNAL(parametersChanged(VideoWriterParams)),
                    this,
                    SLOT(loggingSettingsChanged(VideoWriterParams))
                   );

            loggingSettingsDialogPtr_ -> show();
        }
        else
        {
            loggingSettingsDialogPtr_ -> raise();
        }

        // Set current tab based on current logging format
        QTabWidget *dialogTabWidgetPtr = loggingSettingsDialogPtr_ -> tabWidgetPtr_;
        switch (videoFileFormat_)
        {
            case VIDEOFILE_FORMAT_BMP:
                dialogTabWidgetPtr -> setCurrentWidget(loggingSettingsDialogPtr_ -> bmpTabPtr_);
                break;
                
            case VIDEOFILE_FORMAT_AVI:
                dialogTabWidgetPtr -> setCurrentWidget(loggingSettingsDialogPtr_ -> aviTabPtr_);
                break;

            case VIDEOFILE_FORMAT_FMF:
                dialogTabWidgetPtr -> setCurrentWidget(loggingSettingsDialogPtr_ -> fmfTabPtr_);
                break;

            case VIDEOFILE_FORMAT_UFMF:
                dialogTabWidgetPtr -> setCurrentWidget(loggingSettingsDialogPtr_ -> ufmfTabPtr_);
                break;

            default:
                break;
        }
    }


    void CameraWindow::loggingSettingsChanged(VideoWriterParams params)
    {
        videoWriterParams_ = params;
    }


    void CameraWindow::actionLoggingFormatTriggered()
    {
        // Get Format string
        QPointer<QAction> actionPtr = qobject_cast<QAction *>(sender());
        videoFileFormat_ = actionToVideoFileFormatMap_[actionPtr]; 

        //std::cout << "video file format: "; 
        //std::cout << VIDEOFILE_EXTENSION_MAP[videoFileFormat_].toStdString();
        //std::cout << std::endl;
    }
    

    void CameraWindow::actionLoggingAutoNamingTriggered()
    {
        if (autoNamingDialogPtr_.isNull()) 
        {
            autoNamingDialogPtr_ = new AutoNamingDialog(
                    autoNamingOptions_, 
                    numberOfCameras_, 
                    this
                    );

            autoNamingDialogPtr_ -> show();

            connect(
                    autoNamingDialogPtr_,
                    SIGNAL(autoNamingOptionsChanged(AutoNamingOptions)),
                    this,
                    SLOT(autoNamingOptionsChanged(AutoNamingOptions))
                   );
        }
        else
        {
            autoNamingDialogPtr_ -> raise();
        }
    }


    void CameraWindow::autoNamingOptionsChanged(AutoNamingOptions options)
    {
        //std::cout << __PRETTY_FUNCTION__ << std::endl;
        //std::cout << options.toStdString() << std::endl;
        autoNamingOptions_ = options;
    }


    void CameraWindow::actionTimerEnabledTriggered()
    {
        setCaptureTimeLabel(0.0);
        updateStatusLabel();
    }


    void CameraWindow::actionTimerSettingsTriggered()
    {
        if (timerSettingsDialogPtr_.isNull()) 
        {
            timerSettingsDialogPtr_ = new TimerSettingsDialog(
                    captureDurationSec_,
                    this
                    );
            timerSettingsDialogPtr_ -> show();

            connect(
                    timerSettingsDialogPtr_,
                    SIGNAL(durationChanged(unsigned long)),
                    this,
                    SLOT(onTimerDurationChanged(unsigned long))
                   );
        }
        else
        {
            timerSettingsDialogPtr_ -> raise();
        }
    }


    void CameraWindow::onTimerDurationChanged(unsigned long duration)
    {
        captureDurationSec_ = duration;
        captureStopDateTime_ = captureStartDateTime_.addSecs(captureDurationSec_);
        setCaptureTimeLabel(0.0);
        emit timerDurationChanged(captureDurationSec_);
    }


    void CameraWindow::actionDisplayUpdateFreqTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Display update frequency not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionDisplayFlipVertTriggered()
    {
        flipVert_ = actionDisplayFlipVertPtr_ -> isChecked();
        emit imageOrientationChanged(flipVert_, flipHorz_, imageRotation_);
        updateAllImageLabels();
    }


    void CameraWindow::actionDisplayFlipHorzTriggered()
    {
        flipHorz_ = actionDisplayFlipHorzPtr_ -> isChecked();
        emit imageOrientationChanged(flipVert_, flipHorz_, imageRotation_);
        updateAllImageLabels();
    }


    void CameraWindow::actionDisplayRotTriggered()
    {
        QPointer<QAction> actionPtr = qobject_cast<QAction *>(sender());
        imageRotation_ = actionToRotationMap_[actionPtr];
        emit imageOrientationChanged(flipVert_, flipHorz_, imageRotation_);
        updateAllImageLabels();
    }


    void CameraWindow::actionDisplayAlignToolsTriggered()
    {
        // If alignment settings dialog does exist create it otherwise raise
        if (alignmentSettingsDialogPtr_.isNull()) 
        {
            alignmentSettingsDialogPtr_ = new AlignmentSettingsDialog(
                    alignmentSettings_,
                    this
                    );

            alignmentSettingsDialogPtr_ -> show();

            connect(
                    alignmentSettingsDialogPtr_,
                    SIGNAL(alignmentSettingsChanged(AlignmentSettings)),
                    this,
                    SLOT(alignmentSettingsChanged(AlignmentSettings))
                   );
        }
        else
        {
            alignmentSettingsDialogPtr_ -> raise();
        }
    }

    void CameraWindow::alignmentSettingsChanged(AlignmentSettings settings)
    {
        alignmentSettings_ = settings;
    }



    void CameraWindow::actionVideoModeTriggered(int vidModeInt)
    {
        VideoMode vidMode = VideoMode(vidModeInt);
        QString vidModeString = QString::fromStdString(getVideoModeString(vidMode));

        // -----------------------------------------------------------------------
        // TO DO ... temporary, issue message and revert to format7 video mode
        // -----------------------------------------------------------------------
        if (vidMode == VIDEOMODE_FORMAT7) 
        {
            return;
        }

        QString msgTitle("Development");
        QString msgText("Can't change video mode to "); 
        msgText += vidModeString;
        msgText += QString(" - only Format7 currently supported");
        QMessageBox::information(this, msgTitle, msgText);

        QList<QAction *> actionList = menuCameraVideoModePtr_ -> actions();
        QList<QAction *>::iterator actionIt;
        for (actionIt=actionList.begin(); actionIt!=actionList.end(); actionIt++)
        {
            QPointer<QAction> actionPtr = *actionIt;
            if (actionPtr -> text() == QString("Format7"))
            {
                actionPtr -> setChecked(true);
            }
            else
            {
                actionPtr -> setChecked(false);
            }
        }
    }

    void CameraWindow::actionFrameRateTriggered(int frmRateInt)
    {
        FrameRate frmRate = FrameRate(frmRateInt);
        QString frmRateString = QString::fromStdString(getFrameRateString(frmRate));
    }


    void CameraWindow::actionPropertyTriggered(int propTypeInt)
    {
        PropertyType propType = PropertyType(propTypeInt);
        QString propTypeString = QString::fromStdString(getPropertyTypeString(propType));
        QPointer<PropertyDialog> propDialogPtr = new PropertyDialog(cameraPtr_, propType, this);
        propDialogPtr -> show();
    }


    void CameraWindow::actionColorMapTriggered(int colorMapInt)
    {
        colorMapNumber_ = colorMapInt;
    }
    

    void CameraWindow::actionPluginsEnabledTriggered()
    {
        // TODO - check that there is a plugin available to run
        if (actionPluginsEnabledPtr_ -> isChecked())
        {
            BiasPlugin::setPluginsEnabled(true);
        }
        else
        {
            BiasPlugin::setPluginsEnabled(false);
            setupImageLabels(false,false,true);
        }
    }

    void CameraWindow::actionPluginsSettingsTriggered()
    {
        QPointer<BiasPlugin> selectedPluginPtr = getCurrentPlugin();

        if (!selectedPluginPtr.isNull())
        {
            selectedPluginPtr -> show();
        }
        else
        {
            QString msgTitle("Plugins Message");
            QString msgText("No Plugins available");
            QMessageBox::information(this, msgTitle, msgText);
        }
    }


    void CameraWindow::actionServerEnabledTriggered()
    {
        if (actionServerEnabledPtr_ -> isChecked())
        {
            httpServerPtr_ -> listen(QHostAddress::Any, httpServerPort_);

        }
        else
        {
            httpServerPtr_ -> close();
        }
    }


    void CameraWindow::actionServerPortTriggered()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


    void CameraWindow::actionServerCommandsTriggered()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void CameraWindow::actionHelpUserManualTriggered()
    {
        QString msgTitle("Development");
        QString msgText("User manual not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionHelpAboutTriggered()
    {
        QString msgTitle("Development");
        QString msgText("About not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }

    
    void CameraWindow::actionDumpCameraPropsTriggered()
    {
        std::cout << "DEBUG: Dumping camera properties" << std::endl;
        std::ofstream camPropsStream;
        camPropsStream.open(DEBUG_DUMP_CAMERA_PROPS_FILE_NAME.toStdString());
        camPropsStream << (cameraPtr_  -> getAllPropertiesString());
        camPropsStream.close();

        QString msgTitle("Debug Info");
        QString msgText("Camera properties dumped to file: ");
        msgText += DEBUG_DUMP_CAMERA_PROPS_FILE_NAME;
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::pluginActionGroupTriggered(QAction *action)
    {
        qDebug() << __PRETTY_FUNCTION__;

        QMapIterator<QString,QPointer<BiasPlugin>> pluginIt(pluginMap_);
        while (pluginIt.hasNext())
        {
            pluginIt.next();
            QPointer<BiasPlugin> pluginPtr = pluginIt.value();
            pluginPtr -> setActive(false);
            pluginPtr -> hide();

        }

        RtnStatus rtnStatus;
        QString pluginName = getCurrentPluginName(rtnStatus);
        qDebug() << pluginName << rtnStatus.success; 

        if (rtnStatus.success)
        {
            setCurrentPlugin(pluginName);
        }
    }


    // Private methods
    // -----------------------------------------------------------------------------------

    void CameraWindow::initialize(
            Guid guid, 
            unsigned int cameraNumber, 
            unsigned int numberOfCameras
            )
    {
        connected_ = false;
        capturing_ = false;
        haveImagePixmap_ = false;
        logging_ = false; 

        flipVert_ = false;
        flipHorz_ = false;
        imageRotation_ = IMAGE_ROTATION_0;

        haveDefaultVideoFileDir_ = false;
        timeStamp_ = 0.0;
        framesPerSec_ = 0.0;
        frameCount_ = 0;
        userCameraName_ = QString("");
        format7PercentSpeed_ = DEFAULT_FORMAT7_PERCENT_SPEED;
        showCameraLockFailMsg_ = true;

        BiasPlugin::setPluginsEnabled(true);
        actionPluginsEnabledPtr_ -> setChecked(BiasPlugin::pluginsEnabled());
        // Temporary - plugin development
        // -------------------------------------------------------------------------------
        pluginHandlerPtr_  = new PluginHandler(this);
        pluginMap_[StampedePlugin::PLUGIN_NAME] = new StampedePlugin(this);
        pluginMap_[GrabDetectorPlugin::PLUGIN_NAME] = new GrabDetectorPlugin(pluginImageLabelPtr_,this);
        // -------------------------------------------------------------------------------

        colorMapNumber_ = DEFAULT_COLORMAP_NUMBER;
        videoFileFormat_ = VIDEOFILE_FORMAT_UFMF;
        imageDisplayFreq_ = DEFAULT_IMAGE_DISPLAY_FREQ;
        setCaptureDuration(DEFAULT_CAPTURE_DURATION);

        cameraNumber_ = cameraNumber;
        numberOfCameras_ = numberOfCameras;
        cameraPtr_ = std::make_shared<Lockable<Camera>>(guid);

        threadPoolPtr_ = new QThreadPool(this);
        threadPoolPtr_ -> setMaxThreadCount(MAX_THREAD_COUNT);
        newImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();
        logImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();
        pluginImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();

        setDefaultFileDirs();
        currentVideoFileDir_ = defaultVideoFileDir_;
        currentVideoFileName_ = DEFAULT_VIDEO_FILE_NAME;
        currentConfigFileDir_ = defaultConfigFileDir_;
        currentConfigFileName_ = DEFAULT_CONFIG_FILE_NAME;

        setupStatusLabel();
        setupCameraMenu();
        setupLoggingMenu();
        setupDisplayMenu();
        setupImageDisplayTimer();
        setupCaptureDurationTimer();
        setupImageLabels();
        setupPluginMenu();
        updateAllMenus(); 

        tabWidgetPtr_ -> setCurrentWidget(previewTabPtr_);

        //setCurrentPlugin(pluginMap_.firstKey());
        setCurrentPlugin("grabDetector");

        updateWindowTitle();
        updateCameraInfoMessage();
        setCaptureTimeLabel(0.0);

        connectButtonPtr_ -> setText(QString("Connect"));
        startButtonPtr_ -> setText(QString("Start"));
        startButtonPtr_ -> setEnabled(false);
        connectButtonPtr_ -> setEnabled(true);

        updateStatusLabel();

        ThreadAffinityService::assignThreadAffinity(false,cameraNumber_);

        httpServerPort_  = HTTP_SERVER_PORT_BEGIN; 
        httpServerPort_ += HTTP_SERVER_PORT_STEP*(cameraNumber_ + 1);
        httpServerPtr_ = new ExtCtlHttpServer(this,this);
        setServerPortText();
        if (DEFAULT_HTTP_SERVER_ENABLED)
        {
            actionServerEnabledPtr_ -> setChecked(true);
            httpServerPtr_ -> listen(QHostAddress::Any, httpServerPort_);

        }
        else
        {
            actionServerEnabledPtr_ -> setChecked(false);
        }


    }


    void CameraWindow::setupImageLabels(bool cameraPreview, bool histogram, bool pluginPreview)
    {
        QImage dummyImage;
        dummyImage = QImage(PREVIEW_DUMMY_IMAGE_SIZE,QImage::Format_RGB888);
        dummyImage.fill(QColor(Qt::gray).rgb());

        if (cameraPreview)
        {
            previewPixmapOriginal_ = QPixmap::fromImage(dummyImage);
        }
        if (pluginPreview)
        {
            pluginPixmapOriginal_ = QPixmap::fromImage(dummyImage);
        }

        if (histogram)
        {
            dummyImage = QImage(DEFAULT_HISTOGRAM_IMAGE_SIZE,QImage::Format_RGB888);
            dummyImage.fill(QColor(Qt::gray).rgb());
            histogramPixmapOriginal_ = QPixmap::fromImage(dummyImage);
        }

        updateAllImageLabels();
    }



    void CameraWindow::connectWidgets()
    {
        connect(
                startButtonPtr_, 
                SIGNAL(clicked()), 
                this, 
                SLOT(startButtonClicked())
                ); 

        connect(
                connectButtonPtr_, 
                SIGNAL(clicked()), 
                this, 
                SLOT(connectButtonClicked())
                );

        connect(
                actionFileLoadConfigPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionFileLoadConfigTriggered())
               );

        connect(
                actionFileSaveConfigPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionFileSaveConfigTriggered())
               );

        connect(
                actionFileCloseWindowPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(close())
               );

        connect(
                actionFileHideWindowPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionFileHideWindowTriggered())
               );

        connect(
                actionCameraInfoPtr_, 
                SIGNAL(triggered()), 
                this, 
                SLOT(actionCameraInfoTriggered())
                );

        connect(
                actionCameraFormat7SettingsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionCameraFormat7SettingsTriggered())
               );

        connect(
                actionCameraTriggerExternalPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionCameraTriggerExternalTriggered())
               );

        connect(
                actionCameraTriggerInternalPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionCameraTriggerInternalTriggered())
               );
       
        connect(
                actionLoggingEnabledPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingEnabledTriggered())
               );

        connect(
                actionLoggingVideoFilePtr_, 
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingVideoFileTriggered())
               );

        connect(
                actionLoggingSettingsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingSettingsTriggered())
               );

        connect(
                actionLoggingFormatBMPPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingFormatTriggered())
               );

        connect(
                actionLoggingFormatAVIPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingFormatTriggered())
               );

        connect(
                actionLoggingFormatFMFPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingFormatTriggered())
               );

        connect(
                actionLoggingFormatUFMFPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingFormatTriggered())
               );

        connect(
                actionLoggingFormatIFMFPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingFormatTriggered())
               );

        connect(
                actionLoggingAutoNamingPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionLoggingAutoNamingTriggered())
               );

        connect(
                actionTimerEnabledPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionTimerEnabledTriggered())
               );

        connect(
                actionTimerSettingsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionTimerSettingsTriggered())
               );

        connect(
                actionDisplayUpdateFreqPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayUpdateFreqTriggered())
               );

        connect(
                actionDisplayFlipVertPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayFlipVertTriggered())
               );

        connect(
                actionDisplayFlipHorzPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayFlipHorzTriggered())
               );

        connect(
                actionDisplayRot0Ptr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayRotTriggered())
               );

        connect(
                actionDisplayRot90Ptr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayRotTriggered())
               );

        connect(
                actionDisplayRot180Ptr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayRotTriggered())
               );

        connect(
                actionDisplayRot270Ptr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayRotTriggered())
               );

        connect(
                actionDisplayAlignToolsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDisplayAlignToolsTriggered())
               );

        connect(
                actionPluginsEnabledPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionPluginsEnabledTriggered())
               );

        connect(
                actionPluginsSettingsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionPluginsSettingsTriggered())
               );

        connect(
                actionServerEnabledPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionServerEnabledTriggered())
               );

        connect(
                actionServerPortPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionServerPortTriggered())
               );

        connect(
                actionServerCommandsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionServerCommandsTriggered())
               );

        connect(
                actionHelpUserManualPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionHelpUserManualTriggered())
               );

        connect(
                actionHelpAboutPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionHelpAboutTriggered())
               );

        connect(
                actionDumpCameraPropsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionDumpCameraPropsTriggered())
               );

        connect(
                tabWidgetPtr_,
                SIGNAL(currentChanged(int)),
                this,
                SLOT(tabWidgetChanged(int))
               );

    }


    void CameraWindow::setDefaultFileDirs()
    {
#ifdef WIN32

        QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
        settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
        QString myDocsString = settings.value("Personal").toString();

        QDir myDocsDir = QDir(myDocsString);
        QDir videoDir = myDocsDir;

        bool cdToVideosOk = videoDir.cdUp();
        cdToVideosOk = cdToVideosOk && videoDir.cd("Videos");


        QDir userProfileDir = QDir(QString(getenv("USERPROFILE")));

        // Set default video file dir
        if ((cdToVideosOk) && (videoDir.exists()))
        {
            defaultVideoFileDir_ = videoDir;
        }
        else if (myDocsDir.exists())
        { 
            defaultVideoFileDir_ = myDocsDir;
        }
        else
        {
            defaultVideoFileDir_ = userProfileDir;
        }

        // Set default config file dir
        if (myDocsDir.exists())
        {
            defaultConfigFileDir_ = myDocsDir;
        }
        else
        {
            defaultConfigFileDir_ = userProfileDir;
        }
#else
        defaultVideoFileDir_ = QDir(QString(getenv("HOME")));
        defaultConfigFileDir_ = QDir(QString(getenv("HOME")));
#endif 
        // Check that default video file directory exists
        if (!defaultVideoFileDir_.exists())
        {
            haveDefaultVideoFileDir_ = false;
            QString msgTitle("Initialization Error");
            QString msgText("Unable to determine default location for video files.");
            QMessageBox::critical(this, msgTitle, msgText);
        }
        else
        {
            haveDefaultVideoFileDir_ = true;
        }

        // Check that default config file directory exists
        if (!defaultConfigFileDir_.exists())
        {
            haveDefaultConfigFileDir_ = false;
            QString msgTitle("Initialization Error");
            QString msgText("Unable to determine default location for configuration files.");
            QMessageBox::critical(this, msgTitle, msgText);
        }
        else
        {
            haveDefaultConfigFileDir_ = true;
        }
    }


    void CameraWindow::setupImageDisplayTimer()
    {
        imageDisplayTimerPtr_ = new QTimer(this);
        connect(
                imageDisplayTimerPtr_, 
                SIGNAL(timeout()), 
                this, 
                SLOT(updateDisplayOnTimer())
                );

        unsigned int imageDisplayDt = int(1000.0/imageDisplayFreq_);
        imageDisplayTimerPtr_ -> start(imageDisplayDt);
    }


    void CameraWindow::setupCaptureDurationTimer()
    {
        captureDurationTimerPtr_ = new QTimer(this);
        captureDurationTimerPtr_ -> setInterval(DURATION_TIMER_INTERVAL);
        connect(
                captureDurationTimerPtr_,
                SIGNAL(timeout()),
                this,
                SLOT(checkDurationOnTimer())
               );
    }


    void CameraWindow::updateWindowTitle()
    {
        QString windowTitle;
        if (userCameraName_.isEmpty())
        {
            Guid guid = cameraPtr_ -> getGuid();  // don't need lock for this
            QString guidString = QString::fromStdString(guid.toString());
            windowTitle = QString("BIAS Camera Window, Cam: %1, Guid: %2").arg(cameraNumber_).arg(guidString);
        }
        else
        {
            // Not currently being used???
            windowTitle = QString("BIAS Camera Window, Name: %1").arg(userCameraName_);
        }
        setWindowTitle(windowTitle);
    }


    QPointer<BiasPlugin> CameraWindow::getCurrentPlugin()
    {
        QPointer<BiasPlugin> currentPluginPtr;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        QString currentPluginName = getCurrentPluginName(rtnStatus);
        if (rtnStatus.success)
        {
            if (pluginMap_.size() > 0)
            {
                currentPluginPtr = pluginMap_[currentPluginName];
            }
        }
        return currentPluginPtr;
    }

    
    void CameraWindow::setupStatusLabel()
    {
        setStyleSheet("QStatusBar::item {border: 0px solid black};");
        statusbarPtr_ -> setStyleSheet("QLabel {padding-left: 8px};");
        statusLabelPtr_ = new QLabel();
        statusLabelPtr_ -> setAlignment(Qt::AlignLeft);
        statusbarPtr_ -> addPermanentWidget(statusLabelPtr_,1);
    }


    void CameraWindow::setupCameraMenu()
    {
        videoModeActionGroupPtr_ = new QActionGroup(menuCameraVideoModePtr_);
        frameRateActionGroupPtr_ = new QActionGroup(menuCameraFrameRatePtr_);

        videoModeSignalMapperPtr_ = new QSignalMapper(menuCameraVideoModePtr_);
        frameRateSignalMapperPtr_ = new QSignalMapper(menuCameraFrameRatePtr_);
        propertiesSignalMapperPtr_ = new QSignalMapper(menuCameraPropertiesPtr_);

        connect(
                videoModeSignalMapperPtr_,
                SIGNAL(mapped(int)),
                this,
                SLOT(actionVideoModeTriggered(int))
               );

        connect(
                frameRateSignalMapperPtr_,
                SIGNAL(mapped(int)),
                this,
                SLOT(actionFrameRateTriggered(int))
               );

        connect(
                propertiesSignalMapperPtr_,
                SIGNAL(mapped(int)),
                this,
                SLOT(actionPropertyTriggered(int))
               );

        cameraTriggerActionGroupPtr_ = new QActionGroup(menuCameraPtr_);
        cameraTriggerActionGroupPtr_ -> addAction(actionCameraTriggerInternalPtr_);
        cameraTriggerActionGroupPtr_ -> addAction(actionCameraTriggerExternalPtr_);
    }


    void CameraWindow::setupLoggingMenu()
    {
        loggingFormatActionGroupPtr_ = new QActionGroup(menuLoggingFormatPtr_);
        loggingFormatActionGroupPtr_ -> addAction(actionLoggingFormatBMPPtr_);
        loggingFormatActionGroupPtr_ -> addAction(actionLoggingFormatAVIPtr_);
        loggingFormatActionGroupPtr_ -> addAction(actionLoggingFormatFMFPtr_);
        loggingFormatActionGroupPtr_ -> addAction(actionLoggingFormatUFMFPtr_);
        loggingFormatActionGroupPtr_ -> addAction(actionLoggingFormatIFMFPtr_);
        actionToVideoFileFormatMap_[actionLoggingFormatBMPPtr_] = VIDEOFILE_FORMAT_BMP;
        actionToVideoFileFormatMap_[actionLoggingFormatAVIPtr_] = VIDEOFILE_FORMAT_AVI;
        actionToVideoFileFormatMap_[actionLoggingFormatFMFPtr_] = VIDEOFILE_FORMAT_FMF;
        actionToVideoFileFormatMap_[actionLoggingFormatUFMFPtr_] = VIDEOFILE_FORMAT_UFMF;

        if (logging_)
        {
            RtnStatus rtnStatus = enableLogging();
            if (!rtnStatus.success)
            {
                logging_ = false;
            }
            else
            {
                actionLoggingEnabledPtr_ -> setChecked(true);
            }
        }

        updateLoggingMenu();
    }


    void CameraWindow::setupDisplayMenu() 
    {
        setupDisplayRotMenu();
        setupDisplayOrientMenu();
        setupDisplayColorMapMenu();
    }


    void CameraWindow::setupDisplayOrientMenu()
    {
        updateDisplayOrientMenu();
    }
    

    void CameraWindow::setupDisplayRotMenu()
    {
        rotationActionGroupPtr_ =  new QActionGroup(menuDisplayRotPtr_);
        rotationActionGroupPtr_ -> addAction(actionDisplayRot0Ptr_);
        rotationActionGroupPtr_ -> addAction(actionDisplayRot90Ptr_);
        rotationActionGroupPtr_ -> addAction(actionDisplayRot180Ptr_);
        rotationActionGroupPtr_ -> addAction(actionDisplayRot270Ptr_);
        actionToRotationMap_[actionDisplayRot0Ptr_] = IMAGE_ROTATION_0;
        actionToRotationMap_[actionDisplayRot90Ptr_] = IMAGE_ROTATION_90;
        actionToRotationMap_[actionDisplayRot180Ptr_] = IMAGE_ROTATION_180;
        actionToRotationMap_[actionDisplayRot270Ptr_] = IMAGE_ROTATION_270; 
        updateDisplayRotMenu();
    }


    void CameraWindow::setupDisplayColorMapMenu()
    {
        colorMapSignalMapperPtr_ = new QSignalMapper(menuColorMapPtr_);
        connect(
                colorMapSignalMapperPtr_,
                SIGNAL(mapped(int)),
                this,
                SLOT(actionColorMapTriggered(int))
               );
    }


    void CameraWindow::setupPluginMenu()
    {

        QMapIterator<QString,QPointer<BiasPlugin>> pluginIt(pluginMap_);
        pluginActionGroupPtr_ = new QActionGroup(menuPluginsPtr_);

        while (pluginIt.hasNext())
        {
            pluginIt.next();
            QPointer<BiasPlugin> pluginPtr = pluginIt.value();
            QString pluginName = pluginPtr -> getName();
            QString pluginDisplayName = pluginPtr -> getDisplayName();

            QPointer<QAction> pluginActionPtr = menuPluginsPtr_ -> addAction(pluginDisplayName);
            pluginActionMap_.insert(pluginName, pluginActionPtr);
            pluginActionPtr -> setData(QVariant(pluginName));
            pluginActionGroupPtr_ -> addAction(pluginActionPtr);
            pluginActionPtr -> setCheckable(true);

            pluginActionPtr -> setChecked(false);
            pluginPtr -> setActive(false);

            //if (pluginName == QString("stampede")) 
            //{
            //    pluginActionPtr -> setChecked(true);
            //    pluginPtr -> setActive(true);
            //}
            //else
            //{
            //    pluginActionPtr -> setChecked(false);
            //    pluginPtr -> setActive(false);
            //}
        }


        connect(
                pluginActionGroupPtr_,
                SIGNAL(triggered(QAction*)),
                this,
                SLOT(pluginActionGroupTriggered(QAction*))
               );

    }


    void CameraWindow::updateImageLabel(
            ImageLabel *imageLabelPtr, 
            QPixmap &pixmapOriginal,
            bool flipAndRotate,
            bool addFrameCount,
            bool addRoiBoundary,
            bool addAlignmentObjs
            )
    {
        // Draw ROI
        QPixmap pixmapCopy = QPixmap(pixmapOriginal);

        if (haveImagePixmap_ && addRoiBoundary && (!format7SettingsDialogPtr_.isNull()))
        {
            if (format7SettingsDialogPtr_ -> isRoiShowChecked())
            {
                int x = format7SettingsDialogPtr_ -> getRoiXOffset();
                int y = format7SettingsDialogPtr_ -> getRoiYOffset();
                int w = format7SettingsDialogPtr_ -> getRoiXWidth();
                int h = format7SettingsDialogPtr_ -> getRoiYHeight();
                QPainter roiPainter(&pixmapCopy);
                QPen roiPen = QPen(ROI_BOUNDARY_COLOR);
                roiPen.setWidth(ROI_BOUNDARY_LINE_WIDTH);
                roiPainter.setPen(roiPen);
                roiPainter.drawRect(QRect(x,y,w,h));
                roiPainter.end();
            }
        }

        // Scale pixmap
        QPixmap pixmapScaled =  pixmapCopy.scaled(
                imageLabelPtr -> size(),
                Qt::KeepAspectRatio, 
                Qt::SmoothTransformation
                );

        float scaleFactor = float(pixmapScaled.width())/float(pixmapCopy.width());
        imageLabelPtr -> setScaleFactor(scaleFactor);

        // Add alignment objects
        if (haveImagePixmap_ && addAlignmentObjs)
        { 
            QSize scaledPixmapSize = pixmapScaled.size();
            QSize origPixmapSize = pixmapOriginal.size();

            unsigned int scaledPixmapWidth = scaledPixmapSize.width();
            unsigned int scaledPixmapHeight = scaledPixmapSize.height();
            unsigned int origPixmapWidth = origPixmapSize.width();
            unsigned int origPixmapHeight = origPixmapSize.height();

            // Draw alignment grid
            if (alignmentSettings_.gridVisible)
            {
                QPainter gridPainter(&pixmapScaled);
                QPen gridPen = QPen(alignmentSettings_.gridQColor);
                gridPen.setWidth(alignmentSettings_.gridPenWidth);
                gridPainter.setPen(gridPen);

                unsigned int posX;
                unsigned int posY; 

                for (unsigned int i=0;i<alignmentSettings_.gridNumCol; i++)
                {
                    posX = (i+1)*(scaledPixmapWidth/(alignmentSettings_.gridNumCol+1));
                    gridPainter.drawLine(posX,0, posX, scaledPixmapHeight);
                }

                for (unsigned int i=0; i<alignmentSettings_.gridNumRow; i++)
                {
                    posY = (i+1)*(scaledPixmapHeight/(alignmentSettings_.gridNumRow+1));
                    gridPainter.drawLine(0,posY,scaledPixmapWidth,posY);
                }
                gridPainter.end();
            }

            // Draw alignement circle
            if (alignmentSettings_.ellipseVisible)
            {
                float scaleX = float(scaledPixmapWidth)/float(origPixmapWidth);
                float scaleY = float(scaledPixmapHeight)/float(origPixmapHeight);
                QPainter ellipsePainter(&pixmapScaled);
                QPen ellipsePen = QPen(alignmentSettings_.ellipseQColor);
                ellipsePen.setWidth(alignmentSettings_.ellipsePenWidth);
                ellipsePainter.setPen(ellipsePen);
                ellipsePainter.drawEllipse(
                        (unsigned int)(scaleX*alignmentSettings_.ellipsePosX),
                        (unsigned int)(scaleY*alignmentSettings_.ellipsePosY),
                        (unsigned int)(2*scaleX*alignmentSettings_.ellipseRadiusX),
                        (unsigned int)(2*scaleY*alignmentSettings_.ellipseRadiusY)
                        );
                ellipsePainter.end();
            }
        }

        
        // Flip and rotate pixmap if required
        if (flipAndRotate) {
            if ((imageRotation_ != IMAGE_ROTATION_0) || flipVert_ || flipHorz_ )
            {
                QTransform transform;
                transform.rotate(-1.0*float(imageRotation_));
                if (flipVert_)
                {
                    transform.scale(1.0,-1.0);
                }
                if (flipHorz_) 
                {
                    transform.scale(-1.0,1.0);
                }
                pixmapScaled = pixmapScaled.transformed(transform);
            }
        }

        // Add frame count
        if (haveImagePixmap_ && addFrameCount && (frameCount_ > 0))
        {
            QPainter painter(&pixmapScaled);
            QString msg;  
            msg.sprintf("%lu",frameCount_);
            painter.setPen(QColor(0,220,0));
            painter.drawText(5,12, msg);
        }

        imageLabelPtr -> setPixmap(pixmapScaled);
    }


    void CameraWindow::updateAllImageLabels()
    { 
        updateImageLabel(
                previewImageLabelPtr_,   
                previewPixmapOriginal_,   
                true,  
                true,  
                true,
                true
                );

        updateImageLabel(
                pluginImageLabelPtr_,    
                pluginPixmapOriginal_,    
                true,  
                false, 
                false,
                false
                );

        updateImageLabel(
                histogramImageLabelPtr_, 
                histogramPixmapOriginal_, 
                false, 
                false, 
                false,
                false
                );
    }


    void CameraWindow::resizeImageLabel( 
            ImageLabel *imageLabelPtr, 
            QPixmap &pixmapOriginal, 
            bool flipAndRotate,
            bool addFrameCount,
            bool addRoiBoundary,
            bool addAlignmentObjs
            )
    {
        // Determines if resize of pixmap of image on Qlabel is required and 
        // if so resizes the pixmap.
        
        if (pixmapOriginal.isNull() || ((imageLabelPtr -> pixmap()) == 0))  
        {
            return;
        }

        QSize sizeImageLabel = imageLabelPtr -> size();
        QSize sizeAdjusted = pixmapOriginal.size();
        sizeAdjusted.scale(sizeImageLabel, Qt::KeepAspectRatio);
        QSize sizeImageLabelPixmap = imageLabelPtr -> pixmap() -> size();
        if (sizeImageLabelPixmap != sizeAdjusted) 
        {
            updateImageLabel(
                    imageLabelPtr,
                    pixmapOriginal,
                    flipAndRotate,
                    addFrameCount,
                    addRoiBoundary,
                    addAlignmentObjs
                    );
        }
    }


    void CameraWindow::resizeAllImageLabels()
    { 
        resizeImageLabel(
                previewImageLabelPtr_, 
                previewPixmapOriginal_, 
                true, 
                true, 
                true, 
                true
                );

        resizeImageLabel(
                pluginImageLabelPtr_, 
                pluginPixmapOriginal_, 
                true, 
                false, 
                false, 
                false
                );

        resizeImageLabel(
                histogramImageLabelPtr_, 
                histogramPixmapOriginal_, 
                false, 
                false, 
                false, 
                false
                );
    }


    void CameraWindow::updateHistogramPixmap(cv::Mat hist)
    {
        QImage dummyImage = QImage(DEFAULT_HISTOGRAM_IMAGE_SIZE,QImage::Format_RGB888);
        dummyImage.fill(QColor(Qt::gray).rgb());
        histogramPixmapOriginal_ = QPixmap::fromImage(dummyImage);

        QPainter painter(&histogramPixmapOriginal_);
        painter.setPen(QColor(50,50,50));
        cv::Size histSize = hist.size();

        float histImageMaxY = float(DEFAULT_HISTOGRAM_IMAGE_SIZE.height() - 1.0);

        for (int i=0; i<histSize.height; i++) 
        {
            int y0 = int(histImageMaxY);
            int y1 = int(histImageMaxY - hist.at<float>(0,i));
            painter.drawLine(i,y0,i,y1);
        }

    }


    void CameraWindow::updateCameraInfoMessage()
    {
        QString vendorName = QString("_____");
        QString modelName  = QString("_____");

        if (connected_) 
        {
            if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
            {
                vendorName = QString::fromStdString(cameraPtr_ -> getVendorName());
                modelName  = QString::fromStdString(cameraPtr_ -> getModelName());
                cameraPtr_ -> releaseLock();
            }
        }
        setCameraInfoMessage(vendorName, modelName);
    }

    
    void CameraWindow::setCameraInfoMessage(QString vendorName, QString modelName)
    {
        QString cameraInfoString("Camera:  ");
        cameraInfoString += vendorName;
        cameraInfoString += QString(",  ");
        cameraInfoString += modelName; 
        cameraInfoLabelPtr_ -> setText(cameraInfoString);
    }


    void CameraWindow::updateStatusLabel(QString msg)
    {
        QString statusMsg;
        if (connected_)
        {
            if (!capturing_)
            {
                statusMsg = QString("Connected, logging = ");
                statusMsg += boolToOnOffQString(logging_);
                statusMsg += QString(", timer = ");
                statusMsg += boolToOnOffQString(actionTimerEnabledPtr_ -> isChecked());
                statusMsg += QString(", Stopped");
            }
            else
            {
                statusMsg = QString("Capturing, logging = ");
                statusMsg += boolToOnOffQString(logging_);
                statusMsg += QString(", timer = ");
                statusMsg += boolToOnOffQString(actionTimerEnabledPtr_ -> isChecked());
            }
        }
        else
        {
            statusMsg = QString("Disconnected");
        }

        if (!msg.isEmpty())
        {
            statusMsg += QString(", ") + msg;
        }
        statusLabelPtr_ -> setText(statusMsg);
    }


    void CameraWindow::setMenuChildrenEnabled(QWidget *parentWidgetPtr, bool value)
    {
        QList<QMenu *> childList = parentWidgetPtr -> findChildren<QMenu *>();
        QList<QMenu *>::iterator childIt;
        for (childIt=childList.begin(); childIt!=childList.end(); childIt++)
        {
            QPointer<QMenu> menuPtr = *childIt;
            menuPtr -> setEnabled(value);
        }

        QList<QAction *> actionList = parentWidgetPtr -> actions();
        QList<QAction *>::iterator actionIt;
        for (actionIt=actionList.begin(); actionIt!=actionList.end(); actionIt++)
        {
            QPointer<QAction> actionPtr = *actionIt;
            actionPtr -> setEnabled(value);
        }
    } 


    void CameraWindow::updateAllMenus()
    {
        updateFileMenu();
        updateCameraMenu();
        updateLoggingMenu();
        updateTimerMenu();
        updateDisplayMenu();
        updateDebugMenu();
    }


    void CameraWindow::updateFileMenu()
    {
        if (capturing_ || !connected_)
        {
            actionFileLoadConfigPtr_ -> setEnabled(false);
        }
        else
        {
            actionFileLoadConfigPtr_ -> setEnabled(true);
        }

        if (!connected_)
        {
            actionFileSaveConfigPtr_ -> setEnabled(false);
        }
        else 
        {
            actionFileSaveConfigPtr_ -> setEnabled(true);
        }
    }


    void CameraWindow::updateCameraMenu()
    {
        // WBD: DEVEL
        // ------------------------------------
        updateCameraVideoModeMenu();
        updateCameraFrameRateMenu();
        updateCameraPropertiesMenu();
        updateCameraTriggerMenu();
        // ------------------------------------

        if (connected_) 
        { 
            setMenuChildrenEnabled(menuCameraPtr_,true); 
        }
        else
        {
            setMenuChildrenEnabled(menuCameraPtr_,false);
        }
    }


    void CameraWindow::updateCameraVideoModeMenu()
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg;
        VideoMode currentVideoMode;
        VideoModeList videoModeList;

        if (!connected_) { return; }

        // Remove any existing actions
        deleteMenuActions(menuCameraVideoModePtr_, videoModeActionGroupPtr_);

        // Get list of allowed videomodes from camera 
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                currentVideoMode = cameraPtr_ -> getVideoMode();
                videoModeList = cameraPtr_ -> getAllowedVideoModes(); 
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            if (showCameraLockFailMsg_)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return;
        }

        if (error) 
        {
            QString msgTitle("Camera Query Error");
            QString msgText("Failed to read allowed video modes from camera:");
            msgText += QString("\n\nError ID: ") + QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        // Add action to menu for each allowed video mode
        VideoModeList::iterator modeIt;
        for (modeIt=videoModeList.begin(); modeIt!=videoModeList.end(); modeIt++)
        {
            VideoMode mode = *modeIt;
            QString modeString = QString::fromStdString(getVideoModeString(mode));
            QPointer<QAction> modeActionPtr = menuCameraVideoModePtr_ -> addAction(modeString);
            videoModeActionGroupPtr_ -> addAction(modeActionPtr);
            modeActionPtr -> setCheckable(true);
            
            connect( 
                    modeActionPtr, 
                    SIGNAL(triggered()), 
                    videoModeSignalMapperPtr_, 
                    SLOT(map())
                   );
            videoModeSignalMapperPtr_ -> setMapping(modeActionPtr, int(mode));

            if (mode == currentVideoMode)
            {
                modeActionPtr -> setChecked(true);
            }
            else
            {
                modeActionPtr -> setChecked(false);
            }
            modeActionPtr -> setEnabled(true);
        }

        if (capturing_)
        {
            setMenuChildrenEnabled(menuCameraVideoModePtr_,false);
        }
        else
        {
            setMenuChildrenEnabled(menuCameraVideoModePtr_,true);
        }
    }


    void CameraWindow::updateCameraFrameRateMenu()
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg;
        FrameRateList allowedRateList;
        FrameRate currentFrameRate;
        VideoMode currentVideoMode;

        if (!connected_) { return; }

        // Remove any existing actions from menu
        deleteMenuActions(menuCameraFrameRatePtr_, frameRateActionGroupPtr_);

        // Get list of allowed framerates from camera 
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                currentFrameRate = cameraPtr_ -> getFrameRate();
                currentVideoMode = cameraPtr_ -> getVideoMode();
                allowedRateList = cameraPtr_ -> getAllowedFrameRates(currentVideoMode); 
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            if (showCameraLockFailMsg_)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return;
        }

        if (error) 
        {
            QString msgTitle("Camera Query Error");
            QString msgText("Failed to read frame rates from camera:");  
            msgText += QString("\n\nError ID: ") + QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        // Add action to menu for each allowed frame rate
        FrameRateList::iterator rateIt;
        for (rateIt=allowedRateList.begin(); rateIt!=allowedRateList.end(); rateIt++)
        {
            FrameRate rate = *rateIt;
            QString rateString = QString::fromStdString(getFrameRateString(rate));
            QPointer<QAction> rateActionPtr = 
                menuCameraFrameRatePtr_ -> addAction(rateString);
            frameRateActionGroupPtr_ -> addAction(rateActionPtr);
            rateActionPtr -> setCheckable(true);

            connect( 
                    rateActionPtr, 
                    SIGNAL(triggered()), 
                    frameRateSignalMapperPtr_, 
                    SLOT(map())
                   );
            frameRateSignalMapperPtr_ -> setMapping(rateActionPtr, int(rate));

            if (rate == currentFrameRate)
            {
                rateActionPtr -> setChecked(true);
            }
            else
            {
                rateActionPtr -> setChecked(false);
            } 
            rateActionPtr -> setEnabled(true);
        }

        if (capturing_)
        {
            setMenuChildrenEnabled(menuCameraFrameRatePtr_,false);
        }
        else
        {
            setMenuChildrenEnabled(menuCameraFrameRatePtr_,true);
        }
    }


    void CameraWindow::updateCameraPropertiesMenu()
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg;
        PropertyList propList;
        PropertyInfoMap propInfoMap;

        if (!connected_) 
        { 
            return; 
        }

        deleteMenuActions(menuCameraPropertiesPtr_);

        // Get list of properties from camera 
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                propList = cameraPtr_ -> getListOfProperties();
                propInfoMap = cameraPtr_ -> getMapOfPropertyInfos();
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            if (showCameraLockFailMsg_)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return;
        }

        if (error) 
        {
            QString msgTitle("Camera Query Error");
            QString msgText("Failed to read properties from camera:");  
            msgText += QString("\n\nError ID: ") + QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }


        // Temporary - for disabling trigger mode property
        // ------------------------------------------------
        QPointer<QAction> triggerModeActionPtr; 
        // ------------------------------------------------

        // Action action to menu for each property in list
        PropertyList::iterator propIt;
        for (propIt=propList.begin(); propIt!=propList.end(); propIt++)
        {
            Property prop = *propIt;
            PropertyInfo propInfo = propInfoMap[prop.type];
            if (prop.present)
            {
                std::string propStringStd = getPropertyTypeString(prop.type);
                QString propString = QString::fromStdString(propStringStd);
                QPointer<QAction> propActionPtr = 
                    menuCameraPropertiesPtr_ -> addAction(propString);

                connect( 
                        propActionPtr, 
                        SIGNAL(triggered()), 
                        propertiesSignalMapperPtr_, 
                        SLOT(map())
                        );
                propertiesSignalMapperPtr_ -> setMapping(propActionPtr, int(prop.type));


                // Temporary - for disabling trigger mode
                // ------------------------------------------ 
                if (prop.type == PROPERTY_TYPE_TRIGGER_MODE)
                {
                    triggerModeActionPtr = propActionPtr;
                }
                // ------------------------------------------
            }
        }

        // New allow properties to be changed while camera is running
        setMenuChildrenEnabled(menuCameraPropertiesPtr_,true);

        // Temporary - for disabling trigger mode property
        // ------------------------------------------------
        triggerModeActionPtr -> setEnabled(false);
        // ------------------------------------------------
    }

    void CameraWindow::updateCameraTriggerMenu()
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg;
        TriggerType trigType;

        if (!connected_) 
        {
            return;
        }

        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                trigType = cameraPtr_ -> getTriggerType();
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            if (showCameraLockFailMsg_)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
            return;
        }

        if (error)
        {
            QString msgTitle("Camera Query Error");
            QString msgText("Failed to read trigger type from camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        if (trigType == TRIGGER_INTERNAL)
        {
            actionCameraTriggerInternalPtr_ -> setChecked(true);
            actionCameraTriggerExternalPtr_ -> setChecked(false);
        }
        else 
        {
            actionCameraTriggerInternalPtr_ -> setChecked(false);
            actionCameraTriggerExternalPtr_ -> setChecked(true);
        }

        if (capturing_)
        {
            actionCameraTriggerInternalPtr_ -> setEnabled(false);
            actionCameraTriggerExternalPtr_ -> setEnabled(false);
        }
        else
        {
            // TO DO ... temporary, currently only internal trigger supported
            actionCameraTriggerInternalPtr_ -> setEnabled(true);
            actionCameraTriggerExternalPtr_ -> setEnabled(true);
        }
    }


    void CameraWindow::updateLoggingMenu() 
    {
        // Set selected format
        QMap<QAction*, VideoFileFormat>::iterator it;
        for (
                it  = actionToVideoFileFormatMap_.begin();
                it != actionToVideoFileFormatMap_.end();
                it++
            )
        {
            VideoFileFormat fileFormat = *it;
            QPointer<QAction> actionPtr = actionToVideoFileFormatMap_.key(fileFormat);
            if (videoFileFormat_ == fileFormat) 
            {
                actionPtr -> setChecked(true);
            }
            else
            {
                actionPtr -> setChecked(false);
            }
        }

        // Set Enabled based on capturing state
        if (capturing_)
        {
            setMenuChildrenEnabled(menuLoggingPtr_, false);
        }
        else
        {
            setMenuChildrenEnabled(menuLoggingPtr_, true);
        }
    }


    void CameraWindow::updateTimerMenu()
    {
        if (capturing_)
        {
            setMenuChildrenEnabled(menuTimerPtr_, false);
        }
        else
        {
            setMenuChildrenEnabled(menuTimerPtr_, true);

            if (BiasPlugin::pluginsEnabled())
            {
                QPointer<BiasPlugin> pluginPtr = getCurrentPlugin();
                if (!pluginPtr.isNull())
                {
                    if (pluginPtr -> requireTimer())
                    {
                        actionTimerEnabledPtr_ -> setChecked(true);
                        actionTimerEnabledPtr_ -> setEnabled(false);
                    }
                }
            }
        }
    }


    void CameraWindow::updateDisplayMenu()
    {
        updateDisplayOrientMenu();
        updateDisplayRotMenu();
        updateDisplayColorMapMenu();
    }

    void CameraWindow::updateDebugMenu()
    {
        if (connected_)
        {
            actionDumpCameraPropsPtr_ -> setEnabled(true);
        }
        else
        {
            actionDumpCameraPropsPtr_ -> setEnabled(false);
        }
    }


    void CameraWindow::updateDisplayOrientMenu()
    {
        actionDisplayFlipVertPtr_ -> setChecked(flipVert_);
        actionDisplayFlipHorzPtr_ -> setChecked(flipHorz_);
    }

   
    void CameraWindow::updateDisplayRotMenu()
    {
        QMap<QAction*, ImageRotationType>:: iterator mapIt;
        for (
                mapIt = actionToRotationMap_.begin();
                mapIt != actionToRotationMap_.end();
                mapIt++
            )
        {
            ImageRotationType rotType = *mapIt;
            QAction *actionPtr = mapIt.key();
            if (rotType == imageRotation_)
            {
                actionPtr -> setChecked(true);
            }
            else
            {
                actionPtr -> setChecked(false);
            }
        }
    }


    void CameraWindow::updateDisplayColorMapMenu()
    {
        deleteMenuActions(menuColorMapPtr_, colorMapActionGroupPtr_);
        colorMapActionGroupPtr_ = new QActionGroup(menuColorMapPtr_);

        if (!COLORMAP_INT_TO_STRING_MAP.contains(colorMapNumber_))
        {
            colorMapNumber_ = COLORMAP_NONE; // Set to None if not found 
        }

        QMap<int,QString>::const_iterator mapIt;
        for (
                mapIt = COLORMAP_INT_TO_STRING_MAP.begin(); 
                mapIt != COLORMAP_INT_TO_STRING_MAP.end(); 
                mapIt++
            )
        {
            QPointer<QAction> colorMapActionPtr = menuColorMapPtr_ -> addAction(mapIt.value());
            colorMapActionPtr -> setCheckable(true);
            colorMapActionGroupPtr_ -> addAction(colorMapActionPtr);
            if (colorMapNumber_ == mapIt.key())
            {
                colorMapActionPtr -> setChecked(true);
            }
            else
            {
                colorMapActionPtr -> setChecked(false);
            }

            connect( 
                    colorMapActionPtr, 
                    SIGNAL(triggered()), 
                    colorMapSignalMapperPtr_, 
                    SLOT(map())
                    );

            colorMapSignalMapperPtr_ -> setMapping(colorMapActionPtr, mapIt.key());
        }
    }



    void CameraWindow::deleteMenuActions(QMenu *menuPtr, QActionGroup *actionGroupPtr)
    {
        QList<QAction *> actionList = menuPtr -> actions();
        QList<QAction *>::iterator actionIt;
        for (actionIt=actionList.begin(); actionIt!=actionList.end(); actionIt++)
        {
            QPointer<QAction> actionPtr = *actionIt;
            if (actionGroupPtr != NULL)
            {
                actionGroupPtr -> removeAction(actionPtr);
            }
            menuPtr -> removeAction(actionPtr);
        }
    }


    void CameraWindow::setCaptureTimeLabel(double timeStamp)
    {
        QString stampString = timeStampToQString(timeStamp); 
        if (actionTimerEnabledPtr_ -> isChecked())
        {
            QString captureDurationString = timeStampToQString(
                    double(captureDurationSec_)
                    );
            QString timeLabelString = stampString + " / " + captureDurationString;
            captureTimeLabelPtr_ -> setText(timeLabelString);
        }
        else
        {
            captureTimeLabelPtr_ -> setText(stampString);
        }
    }


    void CameraWindow::setServerPortText()
    {
        QString text = QString("Port (%1)...").arg(httpServerPort_);
        actionServerPortPtr_ -> setText(text);
    }


    QString CameraWindow::getVideoFileFullPathWithAutoNaming()
    {
        QString fileName = currentVideoFileName_;

        // Add camera identifier - guid or camera number
        //if ((autoNamingOptions_.includeCameraIdentifier) || (numberOfCameras_  > 1))
        if (autoNamingOptions_.includeCameraIdentifier)
        {
            if (autoNamingOptions_.cameraIdentifier == AutoNamingOptions::GUID_IDENTIFIER)
            {
                Guid cameraGuid = cameraPtr_ -> getGuid(); // don't need lock for this
                fileName += QString("_guid_") + QString::fromStdString(cameraGuid.toString());
            }
            else
            {
                fileName += QString("_cam_%1").arg(cameraNumber_);
            }
        }

        // Add time and date
        if (autoNamingOptions_.includeTimeAndDate)
        {
            QDateTime dateTime= QDateTime::currentDateTime();
            QString dateTimeString = dateTime.toString(autoNamingOptions_.timeAndDateFormat);
            fileName += QString("_") + dateTimeString;
        }

        // Add file extension
        QString fileExtension;  
        if (videoFileFormat_ != VIDEOFILE_FORMAT_BMP)
        {
            fileExtension = VIDEOFILE_EXTENSION_MAP[videoFileFormat_];
        }
        else
        {
            fileExtension = QString("");
        }
        if (!fileExtension.isEmpty())
        {
            fileName +=  QString(".") + fileExtension;
        }
        QFileInfo videoFileInfo(currentVideoFileDir_, fileName);
        QString videoFileFullPath = videoFileInfo.absoluteFilePath();
        return videoFileFullPath;
    }


    QString CameraWindow::getConfigFileFullPath()
    {
        QString fileName = currentConfigFileName_ + "." + CONFIG_FILE_EXTENSION;
        if (!currentConfigFileDir_.exists())
        {
            currentConfigFileDir_ = defaultConfigFileDir_;
        }
        QFileInfo configFileInfo(currentConfigFileDir_, fileName);
        QString configFileFullPath = configFileInfo.absoluteFilePath();
        return configFileFullPath;
    }
    

    RtnStatus CameraWindow::setCameraFromMap(QVariantMap cameraMap, bool showErrorDlg)
    {
        RtnStatus rtnStatus;

        QString errMsgTitle("Load Configuration Error (Camera)");
        QString currVendorName;
        QString currModelName;
        PropertyList currCameraPropList;
        PropertyInfoMap cameraPropInfoMap;
        Format7Settings format7Settings;
        Format7Info format7Info;
        QString errorMsg;
        bool error = false;
        unsigned int errorId;

        // Get Values from the camera - for making sure that vendor and model match etc.
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                currVendorName = QString::fromStdString(cameraPtr_ -> getVendorName());
                currModelName = QString::fromStdString(cameraPtr_ -> getModelName());
                currCameraPropList = cameraPtr_ -> getListOfProperties();
                cameraPropInfoMap = cameraPtr_ -> getMapOfPropertyInfos();
                format7Settings = cameraPtr_ -> getFormat7Settings();
                format7Info = cameraPtr_ -> getFormat7Info(format7Settings.mode);
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            if (showCameraLockFailMsg_ && showErrorDlg)
            {
                QString msgTitle("Camera Access Error");
                QString msgText("unable to acquire camera lock");
                QMessageBox::critical(this, msgTitle, msgText);
            }
        }

        if (error)
        {
            QString errMsgText("Error retrieving values from camera. Error ID: ");
            errMsgText += QString::number(errorId);
            errMsgText += "\n\n";
            errMsgText += errorMsg;
            if (showErrorDlg)
            {
                QMessageBox::critical(this, errMsgTitle, errMsgText);
            }
            QByteArray emptyByteArray = QByteArray();
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        QString vendorName = cameraMap["vendor"].toString();
        if (vendorName.isEmpty())
        {
            QString errMsgText("Camera: vendor name is not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (vendorName != currVendorName)
        {
            QString errMsgText("Camera: current vendor does not match that in configuration file");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString modelName = cameraMap["model"].toString();
        if (modelName.isEmpty())
        {
            QString errMsgText("Camera: model name is not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (modelName != currModelName)
        {
            QString errMsgText("Camera: current  model does not match that in configuration file");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle, errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Try to set the camera properties
        QVariantMap cameraPropMap = cameraMap["properties"].toMap();
        if (cameraPropMap.isEmpty())
        {
            QString errMsgText("Camera: properties are not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        PropertyList::iterator propListIt;
        for (
                propListIt = currCameraPropList.begin();
                propListIt != currCameraPropList.end();
                propListIt++
            )
        {
            Property prop = *propListIt;
            PropertyInfo propInfo = cameraPropInfoMap[prop.type];
            // -----------------------------------------------------------------------
            // TEMPORARY - ignore tigger mode (some funny happening with the property)
            // -----------------------------------------------------------------------
            if (prop.type == PROPERTY_TYPE_TRIGGER_MODE)
            {
                continue;
            }
            // -----------------------------------------------------------------------
            //std::cout << prop.toString() << std::endl;
            //std::cout << propInfo.toString() << std::endl;
            QString propName = QString::fromStdString(getPropertyTypeString(prop.type));
            QString camelCaseName = propNameToCamelCase(propName);

            QVariantMap propValueMap = cameraPropMap[camelCaseName].toMap();
            if (propValueMap.isEmpty())
            {
                QString errMsgText = QString(
                        "Camera: property %1 is not present"
                        ).arg(camelCaseName);
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }

            rtnStatus = setCameraPropertyFromMap(propValueMap, propInfo, showErrorDlg);
            if (!rtnStatus.success)
            {
                return rtnStatus;
            }

        } // for ( propListIt ...

        // Video Mode
        // ----------
        QString videoModeString = cameraMap["videoMode"].toString();
        if (videoModeString.isEmpty())
        {
            QString errMsgText("VideoMode: is not present in configuration");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        VideoMode videoMode = convertStringToVideoMode(videoModeString);

        // Frame Rate
        // ----------
        QString frameRateString = cameraMap["frameRate"].toString();
        if (frameRateString.isEmpty())
        {
            QString errMsgText("Camera: frameRate is not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        FrameRate frameRate = convertStringToFrameRate(frameRateString);

        // --------------------------------------------------------------------
        // TEMPORARY - currently only allow format7 for videomode and framerate
        // --------------------------------------------------------------------
        if (videoMode != VIDEOMODE_FORMAT7)
        {
            QString errMsgText = QString("Development Error: videoMode = %1").arg(videoModeString); 
            errMsgText += "\n\n";
            errMsgText += "currently only videoMode=Format7 supported";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (frameRate != FRAMERATE_FORMAT7)
        {
            QString errMsgText = QString("Development Error: frameRate = %1").arg(frameRateString); 
            errMsgText += "\n\n";
            errMsgText += "currently only frameRatee=Format7 supported";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        // --------------------------------------------------------------------
        // TO DO - check if videoMode and frameRate are allowed and if so set
        // to new value.
        // --------------------------------------------------------------------


        // Format7 settings
        QVariantMap format7SettingsMap = cameraMap["format7Settings"].toMap();
        if (cameraPropMap.isEmpty())
        {
            QString errMsgText("Camera: format7 settings are not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setFormat7SettingsFromMap(format7SettingsMap, format7Info, showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // Trigger Type
        QString triggerTypeString = cameraMap["triggerType"].toString();
        if (triggerTypeString.isEmpty())
        {
            QString errMsgText("Camera: triggerType is not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        TriggerType triggerType = convertStringToTriggerType(triggerTypeString);

        // --------------------------------------------------------------------
        // TO DO - Check if trigger type is allowed 
        // --------------------------------------------------------------------
        switch (triggerType)
        {
            case TRIGGER_INTERNAL:
                if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
                {
                    cameraPtr_ -> setTriggerInternal();
                    cameraPtr_ -> releaseLock();
                }
                else
                {
                    rtnStatus.success = false;
                    rtnStatus.message = QString("setTriggerInternal - unable to acquire camera lock");
                    return rtnStatus;
                }
                actionCameraTriggerInternalPtr_ -> setChecked(true);
                actionCameraTriggerExternalPtr_ -> setChecked(false);
                break;

            case TRIGGER_EXTERNAL:
                if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
                {
                    cameraPtr_ -> setTriggerExternal();
                    cameraPtr_ -> releaseLock();
                }
                else
                {
                    rtnStatus.success = false;
                    rtnStatus.message = QString("setTriggerInternal - unable to acquire camera lock");
                    return rtnStatus;
                }
                actionCameraTriggerInternalPtr_ -> setChecked(false);
                actionCameraTriggerExternalPtr_ -> setChecked(true);
                break;

            default:
                {
                    QString errMsgText = QString("Unknown triggerType = %1").arg(triggerType); 
                    if (showErrorDlg)
                    {
                        QMessageBox::critical(this,errMsgTitle,errMsgText);
                    }
                    rtnStatus.success = false;
                    rtnStatus.message = errMsgText;
                    return rtnStatus;
                }

        } // swtich(triggerType)

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setLoggingFromMap(QVariantMap loggingMap,bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error (Logging)");

        // Get "Enabled" value
        // -------------------
        if (!loggingMap.contains("enabled"))
        {
            QString errMsgText("Logging configuration: enabled not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!loggingMap["enabled"].canConvert<bool>())
        {
            QString errMsgText("Logging configuration: unable to convert enabled to bool");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        bool loggingEnabledValue  = loggingMap["enabled"].toBool();

        // Get "Format" value
        // -------------------
        if (!loggingMap.contains("format"))
        {
            QString errMsgText("Logging configuration: format not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!loggingMap["format"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration: unable to convert");
            errMsgText += " format to string";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString formatString = loggingMap["format"].toString();
        VideoFileFormat format = convertStringToVideoFileFormat(formatString);
        if (format == VIDEOFILE_FORMAT_UNSPECIFIED)
        {
            QString errMsgText = QString(
                    "Logging configuration: unknown video file format %1"
                    ).arg(formatString); 
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoFileFormat_ = format;

        // Get "Directory" value
        // ----------------------
        if (!loggingMap.contains("directory"))
        {
            QString errMsgText( "Logging configuration: directory not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!loggingMap["directory"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration: unable to convert");
            errMsgText += " directory to string";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString directoryString = loggingMap["directory"].toString();
        QDir directory = QDir(directoryString);
        if (!directory.exists())
        {
            QString errMsgText("Logging configuration: directory does not exist");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        currentVideoFileDir_ = directory;


        // Get "File Name" value
        // ---------------------
        if (!loggingMap.contains("fileName"))
        {
            QString errMsgText("Logging configuration: fileName not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!loggingMap["fileName"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration: unable to convert");
            errMsgText += " fileName to string";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString fileNameString = loggingMap["fileName"].toString();
        currentVideoFileName_ = fileNameString;

        // Set the logging format settings
        // -------------------------------
        QVariantMap formatSettingsMap = loggingMap["settings"].toMap();
        if (formatSettingsMap.isEmpty())
        { 
            QString errMsgText("Logging configuration: settings not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setLoggingFormatFromMap(formatSettingsMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }


        // Set the logging auto naming options 
        // -----------------------------------
        QVariantMap autoNamingOptionsMap = loggingMap["autoNamingOptions"].toMap();
        if (autoNamingOptionsMap.isEmpty())
        {
            QString errMsgText("Logging configuration: autoNamingOptions not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        rtnStatus = setAutoNamingOptionsFromMap(autoNamingOptionsMap,showErrorDlg);
        if (!rtnStatus.success)
        {
            return rtnStatus;
        }

        // After we have all logging information - try and enable logging
        if (loggingEnabledValue)
        {
            enableLogging(showErrorDlg);
        }
        else
        {
            disableLogging(showErrorDlg);
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setTimerFromMap(QVariantMap timerMap, bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error (Timer)");

        // Set "Enabled" value
        if (!timerMap.contains("enabled"))
        {
            QString errMsgText("Timer configuration: enabled");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!timerMap["enabled"].canConvert<bool>())
        {
            QString errMsgText("Timer configuration: unable to ");
            errMsgText += " convert enabled to bool";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        bool timerEnabled = timerMap["enabled"].toBool();
        actionTimerEnabledPtr_ -> setChecked(timerEnabled);

        // Get Settings map
        QVariantMap settingsMap = timerMap["settings"].toMap();
        if (settingsMap.isEmpty())
        {
            QString errMsgText("Timer configuration: settings");
            errMsgText += " not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Set "Duration" value
        if (!settingsMap.contains("duration"))
        {
            QString errMsgText("Timer configuration: settings");
            errMsgText += " duration is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!settingsMap["duration"].canConvert<unsigned long long>())
        {
            QString errMsgText("Timer configuration: unable to convert");
            errMsgText += " settings duration to unsigned long";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        captureDurationSec_ = (unsigned long)(settingsMap["duration"].toULongLong());
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setDisplayFromMap(QVariantMap displayMap, bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Congifuration Error (Display)");

        // Get Orientation map
        QVariantMap orientMap = displayMap["orientation"].toMap();
        if (orientMap.isEmpty())
        {
            QString errMsgText("Display configuration: orientation");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Set Orientation Flip Vertical
        if (!orientMap.contains("flipVertical"))
        {
            QString errMsgText("Display configuration: orientation");
            errMsgText += " flipVertical is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!orientMap["flipVertical"].canConvert<bool>())
        {
            QString errMsgText("Display configuration: unable to convert");
            errMsgText += " orientation flipVertical to bool";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        flipVert_ = orientMap["flipVertical"].toBool();

        // Set Orientation Flip Horizontal
        if (!orientMap.contains("flipHorizontal"))
        {
            QString errMsgText("Display configuration: orientation");
            errMsgText += " flipHorizontal is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!orientMap["flipHorizontal"].canConvert<bool>())
        {
            QString errMsgText("Display configuration: unable to convert");
            errMsgText += " orientation flipHorizontal to bool";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        flipHorz_ = orientMap["flipHorizontal"].toBool();

        // Set Rotation
        if (!displayMap.contains("rotation"))
        {
            QString errMsgText("Display configuration: rotation");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!displayMap["rotation"].canConvert<unsigned int>())
        {
            QString errMsgText("Display configuration: unable to convert");
            errMsgText += " rotation to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int rotationUInt = displayMap["rotation"].toUInt();
        if ( 
                (rotationUInt != IMAGE_ROTATION_0 )  && 
                (rotationUInt != IMAGE_ROTATION_90)  &&
                (rotationUInt != IMAGE_ROTATION_180) &&
                (rotationUInt != IMAGE_ROTATION_270) 
           )
        {
            QString errMsgText("Display configuration: rotation");
            errMsgText += " must be 0, 90, 180, or 270";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        imageRotation_ = ImageRotationType(rotationUInt);

        // Set Colormap (only change if present).
        if (displayMap.contains("colorMap"))
        {
            if (!displayMap["colorMap"].canConvert<QString>())
            {
                QString errMsgText("Display configuration: unable to convert");
                errMsgText += " colorMap to QString";
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            QString colorMapString = displayMap["colorMap"].toString();
            colorMapNumber_ = COLORMAP_INT_TO_STRING_MAP.key(colorMapString,COLORMAP_NONE);
        }


        // Set Image Display frequency
        if (!displayMap.contains("updateFrequency"))
        {
            QString errMsgText("Display configuration: updateFrequency");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!displayMap["updateFrequency"].canConvert<double>())
        {
            QString errMsgText("Display configuration: unable to convert");
            errMsgText += " updateFrequency to double";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        double displayFreq = displayMap["updateFrequency"].toDouble();
        if (displayFreq < MIN_IMAGE_DISPLAY_FREQ)
        {
            QString errMsgText("Display configuration: updateFrequency");
            errMsgText += QString(" must be greater than or equal to %1").arg(
                    MIN_IMAGE_DISPLAY_FREQ
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (displayFreq > MAX_IMAGE_DISPLAY_FREQ)
        {
            QString errMsgText("Display configuration: updateFrequency");
            errMsgText += QString(" must be less than or equal to %1").arg(
                    MIN_IMAGE_DISPLAY_FREQ
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        imageDisplayFreq_ = displayFreq;

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }

    RtnStatus CameraWindow::setServerFromMap(QVariantMap serverMap, bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load configuration Error (Server)");

        // Get "enabled" value
        // -------------------
        if (!serverMap.contains("enabled"))
        {
            QString errMsgText("Server configuration: enabled not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!serverMap["enabled"].canConvert<bool>())
        {
            QString errMsgText("Server configuration: unable to convert enabled to bool");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        bool serverEnabled = serverMap["enabled"].toBool();

        // Get "port" value
        // ----------------
        if (!serverMap.contains("port"))
        {
            QString errMsgText("Server configuration: port is not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!serverMap["port"].canConvert<unsigned int>())
        {
            QString errMsgText("Server configuration: unable to convert port to unsigned int");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int port = serverMap["port"].toUInt();
        if (port < HTTP_SERVER_PORT_BEGIN)
        {
            QString errMsgText = QString("Server configuration: port is too low, must be >= %1").arg(
                    HTTP_SERVER_PORT_BEGIN
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (port > HTTP_SERVER_PORT_END)
        {
            QString errMsgText = QString("Server configuration: port is too high, must be <= %1").arg(
                    HTTP_SERVER_PORT_END
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        httpServerPort_ = port;

        if (serverEnabled)
        {
            actionServerEnabledPtr_ -> setChecked(true);
            httpServerPtr_ -> close();
            httpServerPtr_ -> listen(QHostAddress::Any, httpServerPort_);

        }
        else
        {
            actionServerEnabledPtr_ -> setChecked(false);
            httpServerPtr_ -> close();
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setConfigFileFromMap(
            QVariantMap configFileMap, 
            bool showErrorDlg
            )
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error (File)");

        // Set Directory
        if (!configFileMap.contains("directory"))
        {
            QString errMsgText("Configuration file: directory");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!configFileMap["directory"].canConvert<QString>())
        {
            QString errMsgText("Configuration file: unable to convert");
            errMsgText += " directory to string";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString directoryString = configFileMap["directory"].toString();
        QDir directory = QDir(directoryString);
        if (!directory.exists())
        {
            QString errMsgText("Configuration file: directory");
            errMsgText += " does not exist";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        currentConfigFileDir_ = directory;

        // Set File Name
        if (!configFileMap.contains("fileName"))
        {
            QString errMsgText("Configuration file: fileName");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!configFileMap["fileName"].canConvert<QString>())
        {
            QString errMsgText("Configuration file: unable to convert");
            errMsgText += " fileName to string";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString fileName = configFileMap["fileName"].toString();
        currentConfigFileName_ = fileName;

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setCameraPropertyFromMap(
            QVariantMap propValueMap, 
            PropertyInfo propInfo, 
            bool showErrorDlg
            )
    {
        RtnStatus rtnStatus;
        Property newProp;
        newProp.type = propInfo.type;
        QString errMsgTitle("Load Configuration Error (Camera Property)");
        QString name = QString::fromStdString(getPropertyTypeString(propInfo.type));

        // Get value for "Present"
        if (!propValueMap.contains("present"))
        {
            QString errMsgText = QString(
                    "Camera: property %1 has no value for present"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!propValueMap["present"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera: property %1 unable to cast present to bool"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        newProp.present =  propValueMap["present"].toBool();
        if (newProp.present != propInfo.present)
        {
            QString errMsgText = QString(
                    "Camera: property %1 present value does not match that in property info"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Get value for "Absolute Control"
        if (!propValueMap.contains("absoluteControl"))
        {
            QString errMsgText = QString(
                    "Camera: property %1 has no value for absoluteControl"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!propValueMap["absoluteControl"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera: property %1 unable to convedrt absoluteControl to bool"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        newProp.absoluteControl = propValueMap["absoluteControl"].toBool();
        if (newProp.absoluteControl && !propInfo.absoluteCapable)
        {
            QString errMsgText = QString(
                    "Camera: property %1 is not capable of absoluteControl"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Get value for "One Push"
        if (!propValueMap.contains("onePush"))
        {
            QString errMsgText = QString(
                    "Camera: property %1 has no value for onePush"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!propValueMap["onePush"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera: property %1 unable to convert onePush to bool"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        newProp.onePush = propValueMap["onePush"].toBool();
        if (newProp.onePush && !propInfo.onePushCapable)
        {
            QString errMsgText = QString(
                    "Camera: property %1 is not capable of onePush"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // Get value for "On"
        if (!propValueMap.contains("on"))
        {
            QString errMsgText = QString(
                    "Camera: property %1 has no value for on"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!propValueMap["on"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera: property %1 unable to convert on to bool"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        newProp.on = propValueMap["on"].toBool();

        // Get Value for "Auto Active"
        if (!propValueMap.contains("autoActive"))
        {
            QString errMsgText = QString(
                    "Camera: property %1 has no value for autoActive"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!propValueMap["autoActive"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera: property %1 unable to convert autoActive to bool"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        newProp.autoActive = propValueMap["autoActive"].toBool();
        if (newProp.autoActive && !propInfo.autoCapable)
        {
            QString errMsgText = QString(
                    "Camera: property %1 is not auto capable"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }


        // Get Value
        if (newProp.type == PROPERTY_TYPE_WHITE_BALANCE)
        {
            // Handle special case of white balance
            if (!propValueMap.contains("valueRed"))
            {
                QString errMsgText = QString(
                        "Camera: property %1 has no valueRed"
                        ).arg(name);
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            if (!propValueMap.contains("valueBlue"))
            {
                QString errMsgText = QString(
                        "Camera: property %1 has no valueBlue"
                        ).arg(name);
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            if (!propValueMap["valueRed"].canConvert<unsigned int>())
            {
                QString errMsgText = QString(
                        "Camera: property %1 unable to convert valueRed to unsigned int"
                        ).arg(name);
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            if (!propValueMap["valueBlue"].canConvert<unsigned int>())
            {
                QString errMsgText = QString(
                        "Camera: property %1 unable to convert valueBlue to unsigned int"
                        ).arg(name);
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            newProp.valueA = propValueMap["valueRed"].toUInt();
            newProp.valueB = propValueMap["valueBlue"].toUInt();

            newProp.valueA = std::max(newProp.valueA, propInfo.minValue);
            newProp.valueA = std::min(newProp.valueA, propInfo.maxValue);
            newProp.valueB = std::max(newProp.valueB, propInfo.minValue);
            newProp.valueB = std::min(newProp.valueB, propInfo.maxValue);

            //if (newProp.valueA < propInfo.minValue)
            //{ 
            //    QString errMsgText = QString(
            //            "Camera: property %1 valueRed is out of range (too low)"
            //            ).arg(name);
            //    if (showErrorDlg)
            //    {
            //        QMessageBox::critical(this,errMsgTitle,errMsgText);
            //    }
            //    rtnStatus.success = false;
            //    rtnStatus.message = errMsgText;
            //    return rtnStatus;
            //}
            //else if (newProp.valueA > propInfo.maxValue)
            //{
            //    QString errMsgText = QString(
            //            "Camera: property %1 valueRed is out of range (too high)"
            //            ).arg(name);
            //    if (showErrorDlg)
            //    {
            //        QMessageBox::critical(this,errMsgTitle,errMsgText);
            //    }
            //    rtnStatus.success = false;
            //    rtnStatus.message = errMsgText;
            //    return rtnStatus;
            //}
            //if (newProp.valueB < propInfo.minValue)
            //{ 
            //    QString errMsgText = QString(
            //            "Camera: property %1 valueBlue is out of range (too low)"
            //            ).arg(name);
            //    if (showErrorDlg)
            //    {
            //        QMessageBox::critical(this,errMsgTitle,errMsgText);
            //    }
            //    rtnStatus.success = false;
            //    rtnStatus.message = errMsgText;
            //    return rtnStatus;
            //}
            //else if (newProp.valueB > propInfo.maxValue)
            //{
            //    QString errMsgText = QString(
            //            "Camera: property %1 valueBlue is out of range (too high)"
            //            ).arg(name);
            //    if (showErrorDlg)
            //    {
            //        QMessageBox::critical(this,errMsgTitle,errMsgText);
            //    }
            //    rtnStatus.success = false;
            //    rtnStatus.message = errMsgText;
            //    return rtnStatus;
            //}
        } 
        else
        {
            // Handle case of normal (non white balance) properties values
            if (!propValueMap.contains("value"))
            {
                QString errMsgText = QString(
                        "Camera: property %1 has no value"
                        ).arg(name);
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            if (!propValueMap["value"].canConvert<unsigned int>())
            {
                QString errMsgText = QString(
                        "Camera: property %1 unable to convert value to unsigned int"
                        ).arg(name);
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            newProp.value = propValueMap["value"].toUInt();

            newProp.value = std::max(newProp.value, propInfo.minValue);
            newProp.value = std::min(newProp.value, propInfo.maxValue);

            //if (!newProp.absoluteControl) 
            //{
            //    if (newProp.value < propInfo.minValue)
            //    {
            //        QString errMsgText = QString(
            //                "Camera: property %1 value is out of range (too low)"
            //                ).arg(name);
            //        if (showErrorDlg)
            //        {
            //            QMessageBox::critical(this,errMsgTitle,errMsgText);
            //        }
            //        rtnStatus.success = false;
            //        rtnStatus.message = errMsgText;
            //        return rtnStatus;
            //    }
            //    else if (newProp.value > propInfo.maxValue)
            //    {
            //        QString errMsgText = QString(
            //                "Camera: property %1 value is out of range (too high)"
            //                ).arg(name);
            //        if (showErrorDlg)
            //        {
            //            QMessageBox::critical(this,errMsgTitle,errMsgText);
            //        }
            //        rtnStatus.success = false;
            //        rtnStatus.message = errMsgText;
            //        return rtnStatus;
            //    }
            //}
        }  

        // Get "Absolute Value"
        if (!propValueMap.contains("absoluteValue"))
        {
            QString errMsgText = QString(
                    "Camera: property %1 has no absoluteValue"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!propValueMap["absoluteValue"].canConvert<float>())
        {
            QString errMsgText = QString(
                    "Camera: property %1 unable to convert absoluteValue to float"
                    ).arg(name);
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        newProp.absoluteValue = propValueMap["absoluteValue"].toFloat();
        if (newProp.absoluteControl)
        {
            newProp.absoluteValue = std::max(newProp.absoluteValue, propInfo.minAbsoluteValue);
            newProp.absoluteValue = std::min(newProp.absoluteValue, propInfo.maxAbsoluteValue);

            //if (newProp.absoluteValue < propInfo.minAbsoluteValue)
            //{
            //    QString errMsgText = QString(
            //            "Camera: property %1 absoluteValue is out of range (too low)"
            //            ).arg(name);
            //    if (showErrorDlg)
            //    {
            //        QMessageBox::critical(this,errMsgTitle,errMsgText);
            //    }
            //    rtnStatus.success = false;
            //    rtnStatus.message = errMsgText;
            //    return rtnStatus;
            //}
            //else if (newProp.absoluteValue > propInfo.maxAbsoluteValue)
            //{
            //    QString errMsgText = QString(
            //            "Camera: property %1 absoluteValue is out of range (too high)"
            //            ).arg(name);
            //    if (showErrorDlg)
            //    {
            //        QMessageBox::critical(this,errMsgTitle,errMsgText);
            //    }
            //    rtnStatus.success = false;
            //    rtnStatus.message = errMsgText;
            //    return rtnStatus;
            //}
        }

        // Set value in camera
        if (propInfo.present) 
        {
            bool error = false;
            unsigned int errorId;
            QString errorMsg;

            if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
            {
                try
                {
                    cameraPtr_ -> setProperty(newProp);
                }
                catch (RuntimeError &runtimeError)
                {
                    error = true;
                    errorId = runtimeError.id();
                    errorMsg = QString::fromStdString(runtimeError.what());
                }
                cameraPtr_ -> releaseLock();
            }
            else
            {
                if (showCameraLockFailMsg_ && showErrorDlg)
                {
                    QString msgTitle("Camera Access Error");
                    QString msgText("unable to acquire camera lock");
                    QMessageBox::critical(this, msgTitle, msgText);
                }
            }

            if (error)
            {
                QString errMsgText = QString("Error setting camera property %1.\n\nError ID: ").arg(name);
                errMsgText += QString::number(errorId);
                errMsgText += "\n\n";
                errMsgText += errorMsg;
                if (showErrorDlg)
                {
                    QMessageBox::critical(this, errMsgTitle, errMsgText);
                }
                QByteArray emptyByteArray = QByteArray();

                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
        }
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setFormat7SettingsFromMap(
            QVariantMap settingsMap, 
            Format7Info format7Info, 
            bool showErrorDlg
            )
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error (Format7 Settings)");

        // Mode
        if (!settingsMap.contains("mode"))
        {
            QString errMsgText("Format7 Settings: mode not present"); 
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if (!settingsMap["mode"].canConvert<QString>())
        {
            QString errMsgText("Format7 Settings: unable to convert mode to string");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        QString imageModeString = settingsMap["mode"].toString();
        ImageMode imageMode = convertStringToImageMode(imageModeString);
        if (imageMode == IMAGEMODE_UNSPECIFIED)
        {
            QString errMsgText = QString("Format7 Settings: unknown image mode ");
            errMsgText += QString("%1").arg(imageModeString);
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }

        // Pixel Format
        if (!settingsMap.contains("pixelFormat"))
        {
            QString errMsgText("Format7 Settings: pixelFormat not present");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if (!settingsMap["pixelFormat"].canConvert<QString>())
        {
            QString errMsgText("Format7 Settings: unable to convert pixelFormat to string");
            return onError(errMsgText,errMsgTitle,showErrorDlg);

        }
        QString pixelFormatString = settingsMap["pixelFormat"].toString();
        PixelFormat pixelFormat = convertStringToPixelFormat(pixelFormatString);
        if (pixelFormat == PIXEL_FORMAT_UNSPECIFIED)
        {
            QString errMsgText("Format7 Settings: unknown pixelFormat, ");
            errMsgText += QString("%1").arg(pixelFormatString);
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }

        QVariantMap roiMap = settingsMap["roi"].toMap();
        if (roiMap.isEmpty())
        {
            QString errMsgText("Format7 Settings: roi no present");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }

        // OffsetX
        if (!roiMap.contains("offsetX"))
        {
            QString errMsgText("Format7 Settings: ROI offsetX not present");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if (!roiMap["offsetX"].canConvert<unsigned int>())
        {
            QString errMsgText("Format7 Settings: unable to convert ROI offsetX to unsigned int");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        unsigned int offsetX = roiMap["offsetX"].toUInt();
        if (offsetX > (format7Info.maxWidth-format7Info.offsetHStepSize))
        {
            QString errMsgText("Format7 Settings: ROI offsetX out of range");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if ((offsetX%format7Info.offsetHStepSize)!=0)
        {
            QString errMsgText = QString("Format7 Settings: ROI offsetX must be "); 
            errMsgText += QString("divisible by step size = %1").arg(format7Info.offsetHStepSize);
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }

        // offsetY
        if (!roiMap.contains("offsetY"))
        { 
            QString errMsgText("Format7 Settings: ROI offsetY not present");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if (!roiMap["offsetY"].canConvert<unsigned int>())
        {
            QString errMsgText("Format7 Settings: unable to convert ROI offsetY "); 
            errMsgText += "to unsigned int";
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        unsigned int offsetY = roiMap["offsetY"].toUInt();
        if (offsetY > (format7Info.maxHeight-format7Info.offsetVStepSize))
        {
            QString errMsgText("Format7 Settings: ROI offsetY out of range");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if ((offsetY%format7Info.offsetVStepSize)!=0)
        {
            QString errMsgText = QString("Format7 Settings: offsetY must be divisible "); 
            errMsgText += QString("by step size = %1").arg(format7Info.offsetVStepSize);
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        
        // Width
        if (!roiMap.contains("width"))
        {
            QString errMsgText("Format7 Settings: ROI width not present");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        } 
        if (!roiMap["width"].canConvert<unsigned int>())
        {
            QString errMsgText("Format7 Settings: unable to convert ROI width"); 
            errMsgText += "to unsigned int";
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        unsigned int width = roiMap["width"].toUInt();
        if (width > format7Info.maxWidth)
        {
            QString errMsgText("Format7 Settings: ROI width > maxWidth"); 
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if ((width%format7Info.imageHStepSize)!=0)
        {
            QString errMsgText("Format7 Settings: ROI width must be divisible by "); 
            errMsgText += QString("step size = %1").arg(format7Info.imageHStepSize);
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if((offsetX + width) > format7Info.maxWidth)
        {
            QString errMsgText("Format7 Settings: ROI offsetX + width > maxWidth"); 
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }

        // Height
        if (!roiMap.contains("height"))
        {
            QString errMsgText("Format7 Settings: ROI height not present");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if (!roiMap["height"].canConvert<unsigned int>())
        {
            QString errMsgText("Format7 Settings: unablel to convert ROI height ");
            errMsgText += QString("to unsigned int");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        unsigned int height = roiMap["height"].toUInt();
        if (height > format7Info.maxHeight)
        {
            QString errMsgText("Format7 Settings: ROI height > maxHeight");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if ((height%format7Info.imageVStepSize)!=0)
        {
            QString errMsgText("Format7 Settings: ROI height must be divisible by ");
            errMsgText += QString("step size = %1").arg(format7Info.imageVStepSize);
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }
        if ((offsetY + height) > format7Info.maxHeight)
        {
            QString errMsgText("Format7 Settings: ROI offsetY + height > maxHeight");
            return onError(errMsgText,errMsgTitle,showErrorDlg);
        }

        // Set format7 settings
        Format7Settings format7Settings;
        format7Settings.mode = imageMode;
        format7Settings.pixelFormat = pixelFormat;
        format7Settings.offsetX = offsetX;
        format7Settings.offsetY = offsetY;
        format7Settings.width = width;
        format7Settings.height = height;
        //format7Settings.print();

        bool captureStopped = false;
        if (capturing_) 
        {
            stopImageCapture();
            captureStopped = true;
        }

        bool error = false;
        bool settingsAreValid = false;
        unsigned int errorId;
        QString errorMsg;

        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            try
            {
                settingsAreValid = cameraPtr_ -> validateFormat7Settings(format7Settings);
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            QString errMsgText("unable to acquire camera lock - failed to validate format7 settings.");
            return onError(errMsgText, errMsgTitle, showErrorDlg);
        }

        if (error)
        {
            QString errMsgText("Failed to validate format7 settings, ");
            errMsgText += QString("Error ID: ") + QString::number(errorId);
            errMsgText += QString(", %1").arg(errorMsg);
            return onError(errMsgText, errMsgTitle, showErrorDlg);
        }

        if (settingsAreValid)
        {
            if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
            {
                try
                {
                    cameraPtr_ -> setFormat7Configuration(
                            format7Settings, 
                            format7PercentSpeed_
                            );
                }
                catch (RuntimeError &runtimeError)
                {
                    error = true;
                    errorId = runtimeError.id();
                    errorMsg = QString::fromStdString(runtimeError.what());
                }
                cameraPtr_ -> releaseLock();
            }
            else
            {
                QString errMsgText("unable to acquire camera lock - failed to set format7 settings.");
                return onError(errMsgText, errMsgTitle, showErrorDlg);
            }
            
            if (error)
            {
                QString errMsgText("Failed to set format7 settings, ");
                errMsgText += QString("Error ID: ") + QString::number(errorId);
                errMsgText += QString(", %1").arg(errorMsg);;
                return onError(errMsgText, errMsgTitle, showErrorDlg);
            }
        }
        else
        { 
            QString errMsgText("Format7 settings invalid");
            return onError(errMsgText, errMsgTitle, showErrorDlg);
        }

        if (captureStopped)
        { 
            startImageCapture();
        }

        emit format7SettingsChanged();
        rtnStatus.success = true;
        rtnStatus.message = "";
        return rtnStatus;
    }


    RtnStatus CameraWindow::setLoggingFormatFromMap(QVariantMap formatMap, bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error (Logging Format Settings)");

        // Get avi values
        // --------------
        QVariantMap aviMap = formatMap["avi"].toMap();
        if (aviMap.isEmpty())
        {
            QString errMsgText("Logging Settings: avi settings not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!aviMap.contains("frameSkip"))
        {
            QString errMsgText("Logging Settings: avi frameSkip not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!aviMap["frameSkip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: unable to convert");
            errMsgText += " frameSkip to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int aviFrameSkip = aviMap["frameSkip"].toUInt();
        if (aviFrameSkip == 0)
        {
            QString errMsgText("Logging Settings: avi frameSkip");
            errMsgText += " must be greater than zero"; 
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.avi.frameSkip = aviFrameSkip;

        if (!aviMap.contains("codec"))
        {
            QString errMsgText("Logging Settings: avi codec not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!aviMap["codec"].canConvert<QString>())
        {
            QString errMsgText("Logging Settings: unable to convert");
            errMsgText += " avi codec to string";
            if (showErrorDlg)
            {
                QMessageBox::critical(this, errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString aviCodec = aviMap["codec"].toString();
        if (!VideoWriter_avi::isAllowedCodec(aviCodec))
        {
            QString errMsgText = QString("Logging Settings: avi codec %1").arg(aviCodec);
            errMsgText += " is not allowed";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.avi.codec = aviCodec;


        // Get bmp values
        // --------------
        QVariantMap bmpMap = formatMap["bmp"].toMap();
        if (bmpMap.isEmpty())
        {
            QString errMsgText("Logging Settings: bmp settings not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!bmpMap.contains("frameSkip"))
        {
            QString errMsgText("Logging Settings: bmp frameSkip not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!bmpMap["frameSkip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: bmp unable to convert");
            errMsgText += " frameSkip to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int bmpFrameSkip = bmpMap["frameSkip"].toUInt();
        if (bmpFrameSkip == 0)
        {
            QString errMsgText("Logging Settings: bmp frameSkip must");
            errMsgText += " be greater than zero";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.bmp.frameSkip = bmpFrameSkip;

        // Get fmf values
        // --------------
        QVariantMap fmfMap = formatMap["fmf"].toMap();
        if (fmfMap.isEmpty())
        {
            QString errMsgText("Logging Settings: fmf settings not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!fmfMap.contains("frameSkip"))
        {
            QString errMsgText("Logging Settings: fmf frameSkip not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!fmfMap["frameSkip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: fmf unable to convert");
            errMsgText += " frameSkip to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int fmfFrameSkip = fmfMap["frameSkip"].toUInt();
        if (fmfFrameSkip == 0)
        {
            QString errMsgText("Logging Settings: fmf frameSkip must");
            errMsgText += " be greater than zero";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.fmf.frameSkip = fmfFrameSkip;
        
        // Get ufmf values
        // ---------------
        QVariantMap ufmfMap = formatMap["ufmf"].toMap();
        if (ufmfMap.isEmpty())
        {
            QString errMsgText("Logging Settings: ufmf settings not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        // ufmf Frame Skip
        if (!ufmfMap.contains("frameSkip"))
        {
            QString errMsgText("Logging Settings: ufmf frameSkip not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfMap["frameSkip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: ufmf unable to convert");
            errMsgText += " frameSkip to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfFrameSkip = ufmfMap["frameSkip"].toUInt();
        if (ufmfFrameSkip == 0)
        {
            QString errMsgText("Logging Settings: ufmf frameSkip must");
            errMsgText += " be greater than zero";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.ufmf.frameSkip = ufmfFrameSkip;

        // ufmf Background threshold
        if (!ufmfMap.contains("backgroundThreshold"))
        {
            QString errMsgText("Logging Settings: ufmf"); 
            errMsgText += " backgroundThreshold not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfMap["backgroundThreshold"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: ufmf unable");
            errMsgText += " to convert backgroundThreshold to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfBackgroundThreshold = ufmfMap["backgroundThreshold"].toUInt();
        if (ufmfBackgroundThreshold < VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD)
        {
            QString errMsgText("Logging Settings: ufmf backgroundThreshold"); 
            errMsgText += QString(" must be greater than %1").arg(
                    VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (ufmfBackgroundThreshold > VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD)
        {
            QString errMsgText("Logging Settings: ufmf backgroundThreshold");
            errMsgText += QString(" msut be less than %1").arg(
                    VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.ufmf.backgroundThreshold = ufmfBackgroundThreshold;

        // ufmf Box length
        if (!ufmfMap.contains("boxLength"))
        {
            QString errMsgText("Logging Settings: ufmf"); 
            errMsgText += " boxLength not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfMap["boxLength"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: ufmf unable");
            errMsgText += " to convert boxLength to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfBoxLength = ufmfMap["boxLength"].toUInt();
        if (ufmfBoxLength < VideoWriter_ufmf::MIN_BOX_LENGTH)
        {
            QString errMsgText("Logging Settings: ufmf boxLength"); 
            errMsgText += QString(" must be greater than %1").arg(
                    VideoWriter_ufmf::MIN_BOX_LENGTH
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (ufmfBoxLength > VideoWriter_ufmf::MAX_BOX_LENGTH)
        {
            QString errMsgText("Logging Settings: ufmf boxLength");
            errMsgText += QString(" must be less than %1").arg(
                    VideoWriter_ufmf::MAX_BOX_LENGTH
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.ufmf.boxLength = ufmfBoxLength;

        // ufmf Compression Threads
        if (!ufmfMap.contains("compressionThreads"))
        {
            QString errMsgText("Logging Settings: ufmf"); 
            errMsgText += " compressionThreads not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfMap["compressionThreads"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: ufmf unable");
            errMsgText += " to convert compressionThreads to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfCompressionThreads = ufmfMap["compressionThreads"].toUInt();
        if (ufmfCompressionThreads < VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS)
        {
            QString errMsgText("Logging Settings: ufmf compressionThreads"); 
            errMsgText += QString(" must be greater than %1").arg(
                    VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.ufmf.numberOfCompressors = ufmfCompressionThreads;

        // ufmf median update count
        if (!ufmfMap.contains("medianUpdateCount"))
        {
            QString errMsgText("Logging Settings: ufmf"); 
            errMsgText += " medianUpdateCount not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfMap["medianUpdateCount"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: ufmf unable");
            errMsgText += " to convert medianUpdateCount to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfMedianUpdateCount = ufmfMap["medianUpdateCount"].toUInt();
        if (ufmfMedianUpdateCount < BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT )
        {
            QString errMsgText("Logging Settings: ufmf medianUpdateCount"); 
            errMsgText += QString(" must be greater than %1").arg(
                    BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.ufmf.medianUpdateCount = ufmfMedianUpdateCount;

        // ufmf median update interval
        if (!ufmfMap.contains("medianUpdateInterval"))
        {
            QString errMsgText("Logging Settings: ufmf");
            errMsgText += " medianUpdateInterval not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfMap["medianUpdateInterval"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: ufmf unable");
            errMsgText += " to convert medianUpdateInterval to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfMedianUpdateInterval = ufmfMap["medianUpdateInterval"].toUInt();
        if (ufmfMedianUpdateInterval < BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL )
        {
            QString errMsgText("Logging Settings: ufmf medianUpdateInterval");
            errMsgText += QString(" must be greater than %1").arg(
                    BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL
                    );
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        videoWriterParams_.ufmf.medianUpdateInterval = ufmfMedianUpdateInterval;

        // ufmf Dilate
        QVariantMap ufmfDilateMap = ufmfMap["dilate"].toMap();
        if (ufmfDilateMap.isEmpty())
        {
            QString errMsgText("Logging Settings: ufmf dilate");
            errMsgText += " is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        // ufmf Dilate On
        if (!ufmfDilateMap.contains("on"))
        {
            QString errMsgText("Logging Settins: ufmf dilate");
            errMsgText += " on is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfDilateMap["on"].canConvert<bool>())
        {
            QString errMsgText("Logging Settings: unable to convert");
            errMsgText += " ufmf dilate on to bool";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        bool ufmfDilateOn = ufmfDilateMap["on"].toBool();
        videoWriterParams_.ufmf.dilateState = ufmfDilateOn;


        // ufmf Dilate Window Size
        if (!ufmfDilateMap.contains("windowSize"))
        {
            QString errMsgText("Logging Settings: ufmf dilate");
            errMsgText += " windowSize is not present";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!ufmfDilateMap["windowSize"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging Settings: unable to convert");
            errMsgText += " ufmf dilate windowSize to unsigned int";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        unsigned int ufmfDilateWindowSize = ufmfDilateMap["windowSize"].toUInt();
        // ---------------------------------------------------------------------
        // TO DO ... add ufmf check of ufmf Dilate window size range
        // ----------------------------------------------------------------------
        videoWriterParams_.ufmf.dilateWindowSize = ufmfDilateWindowSize;

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus CameraWindow::setAutoNamingOptionsFromMap( 
            QVariantMap autoNamingOptionsMap, 
            bool showErrorDlg
            )
    {
        RtnStatus rtnStatus;
        QString errMsgTitle("Load Configuration Error (Logging Auto Naming Options)");

        // Get includeCameraIdentifier value
        // ------------------------------------------------------------------------------
        if (!autoNamingOptionsMap.contains("includeCameraIdentifier"))
        {
            QString errMsgText("Logging Auto Naming: includeCameraIdentifier not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!autoNamingOptionsMap["includeCameraIdentifier"].canConvert<bool>())
        {
            QString errMsgText("Logging Auto Naming: unable to convert"); 
            errMsgText += "includeCameraIdentifier to bool ";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        autoNamingOptions_.includeCameraIdentifier = autoNamingOptionsMap["includeCameraIdentifier"].toBool();

        // Get includeTimeAndDate value
        // ------------------------------------------------------------------------------
        if (!autoNamingOptionsMap.contains("includeTimeAndDate"))
        {
            QString errMsgText("Logging Auto Naming: includeTimeAndDate not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!autoNamingOptionsMap["includeTimeAndDate"].canConvert<bool>())
        {
            QString errMsgText("Logging Auto Naming: unable to convert"); 
            errMsgText += "includeTimeAndDate to bool ";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        autoNamingOptions_.includeTimeAndDate = autoNamingOptionsMap["includeTimeAndDate"].toBool();


        if (!autoNamingOptionsMap.contains("includeVersionNumber"))
        {
            // This is a new option - if it doesn't exist set it to the default value.
            AutoNamingOptions optionsDefault;
            autoNamingOptions_.includeVersionNumber = optionsDefault.includeVersionNumber;
        }
        else
        {
            if (!autoNamingOptionsMap["includeVersionNumber"].canConvert<bool>())
            {
                QString errMsgText("Logging Auto Naming: unable to convert"); 
                errMsgText += "includeVersionNumber to bool ";
                if (showErrorDlg)
                {
                    QMessageBox::critical(this,errMsgTitle,errMsgText);
                }
                rtnStatus.success = false;
                rtnStatus.message = errMsgText;
                return rtnStatus;
            }
            autoNamingOptions_.includeVersionNumber = autoNamingOptionsMap["includeVersionNumber"].toBool();
        }

        // Get cameraIdentifier value
        // ------------------------------------------------------------------------------
        if (!autoNamingOptionsMap.contains("cameraIdentifier"))
        {
            QString errMsgText("Logging Auto Naming: cameraIdentifier not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!autoNamingOptionsMap["cameraIdentifier"].canConvert<QString>())
        {
            QString errMsgText("Logging Auto Naming: unable to convert"); 
            errMsgText += "caneraIdentifier to string ";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString cameraIdentifierString = autoNamingOptionsMap["cameraIdentifier"].toString();
        int cameraIdentifier = AutoNamingOptions::cameraIdentifierFromString(cameraIdentifierString);
        if (cameraIdentifier == -1)
        {
            QString errMsgText = QString("Logging Auto Naming:"); 
            errMsgText += QString(" unknown camera identifier type %1").arg(cameraIdentifierString); 
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        autoNamingOptions_.cameraIdentifier = cameraIdentifier;


        // Get time and date format string
        // ------------------------------------------------------------------------------
        if (!autoNamingOptionsMap.contains("timeAndDateFormat"))
        {
            QString errMsgText("Logging Auto Naming: timeAndDateFormat not present");
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        if (!autoNamingOptionsMap["timeAndDateFormat"].canConvert<QString>())
        {
            QString errMsgText("Logging Auto Naming: unable to convert "); 
            errMsgText += "timeAndDateFormat to string ";
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QString timeAndDateFormat = autoNamingOptionsMap["timeAndDateFormat"].toString();
        if (!AutoNamingOptions::isAllowedTimeAndDateFormat(timeAndDateFormat))
        {
            QString errMsgText("Logging Auto Naming: ");
            errMsgText += QString("invalid timeAndDateFormat %1").arg(timeAndDateFormat); 
            if (showErrorDlg)
            {
                QMessageBox::critical(this,errMsgTitle,errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        autoNamingOptions_.timeAndDateFormat = timeAndDateFormat;
       

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    cv::Mat CameraWindow::calcHistogram(cv::Mat mat)
    {
        // -----------------------------------------------------------------------------
        // TO DO  - only for 8bit black and white cameras needs modification for color.
        // -----------------------------------------------------------------------------
        int channels[] = {0};
        int histSize[] = {256};
        float dimRange[] = {0,256}; // need to be set based on image type.
        const float *ranges[] = {dimRange};
        double minVal = 0;
        double maxVal = 0;
        double histImageMaxY = DEFAULT_HISTOGRAM_IMAGE_SIZE.height() - 1.0;

        cv::Mat hist;
        cv::calcHist(&mat, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);
        minMaxLoc(hist,&minVal,&maxVal,NULL,NULL);
        hist = hist*(float(histImageMaxY)/float(maxVal));
        return hist;
    }

    RtnStatus CameraWindow::onError(QString message, QString title, bool showErrorDlg)
    { 
        RtnStatus rtnStatus;
        if (showErrorDlg)
        { 
            QMessageBox::critical(this,title,message);
        }
        rtnStatus.success = false;
        rtnStatus.message = message;
        return rtnStatus;
    }


    // Utility functions
    // ----------------------------------------------------------------------------------

    QString boolToOnOffQString(bool value)
    {
        return (value) ? QString("on") : QString("off");
    }


    QString timeStampToQString(double timeStamp)
    {
        double hrs, min, sec;
        double rem = timeStamp; 

        hrs = std::floor(timeStamp/3600.0);
        rem -= 3600*hrs;
        min = std::floor(rem/60.0);
        rem -= 60*min;
        sec = std::floor(rem);

        QString timeString;
        timeString.sprintf("%02d:%02d:%02d", int(hrs), int(min), int(sec));
        return timeString;
    }



    VideoMode convertStringToVideoMode(QString videoModeString)
    {
        QMap<QString,VideoMode> map = getStringToVideoModeMap();
        VideoMode mode;

        if (map.contains(videoModeString))
        {
            mode = map[videoModeString];
        }
        else
        {
            mode = VIDEOMODE_UNSPECIFIED;
        }
        return mode;
    }


    FrameRate convertStringToFrameRate(QString frameRateString)
    {
        QMap<QString,FrameRate> map = getStringToFrameRateMap();
        FrameRate rate;

        if (map.contains(frameRateString))
        {
            rate = map[frameRateString];
        }
        else
        {
            rate = FRAMERATE_UNSPECIFIED;
        }
        return rate;
    }


    TriggerType convertStringToTriggerType(QString trigTypeString)
    {
        QMap<QString,TriggerType> map = getStringToTriggerTypeMap();
        TriggerType trigType;

        if (map.contains(trigTypeString))
        {
            trigType = map[trigTypeString];
        }
        else
        {
            trigType = TRIGGER_TYPE_UNSPECIFIED;
        }
        return trigType;
    }
    

    VideoFileFormat convertStringToVideoFileFormat(QString formatString)
    {
        VideoFileFormat videoFileFormat = VIDEOFILE_FORMAT_UNSPECIFIED;
        QMap<VideoFileFormat,QString>::iterator it;
        QMap<VideoFileFormat,QString> extensionMap(VIDEOFILE_EXTENSION_MAP);
        for (it  = extensionMap.begin(); it != extensionMap.end(); it++)
        {
            VideoFileFormat allowedFormat = it.key();
            QString allowedFormatString = it.value();
            if (formatString == allowedFormatString)
            {
                videoFileFormat = allowedFormat;
            }
        }
        return videoFileFormat;
    }


    ImageMode convertStringToImageMode(QString imageModeString)
    {
        QMap<QString, ImageMode> map = getStringToImageModeMap();
        ImageMode  imageMode;
        if (map.contains(imageModeString))
        {
            imageMode = map[imageModeString];
        }
        else
        {
            imageMode = IMAGEMODE_UNSPECIFIED;
        }
        return imageMode;
    }


    PixelFormat convertStringToPixelFormat(QString pixelFormatString)
    {
        QMap<QString, PixelFormat> map = getStringToPixelFormatMap();
        PixelFormat pixelFormat;
        if (map.contains(pixelFormatString))
        {
            pixelFormat = map[pixelFormatString];
        }
        else
        {
            pixelFormat = PIXEL_FORMAT_UNSPECIFIED;
        }
        return pixelFormat;
    }


    QMap<QString,VideoMode> getStringToVideoModeMap()
    {
        QMap<QString,VideoMode> map;
        VideoModeList modeList = getListOfVideoModes();
        VideoModeList::iterator it;
        for (it=modeList.begin(); it!=modeList.end(); it++)
        {
            VideoMode mode = *it;
            QString modeString = QString::fromStdString(getVideoModeString(mode));
            map[modeString] = mode;
        }
        return map;
    }


    QMap<QString,FrameRate> getStringToFrameRateMap()
    {
        QMap<QString,FrameRate> map;
        FrameRateList rateList = getListOfFrameRates();
        FrameRateList::iterator it;
        for(it=rateList.begin(); it!=rateList.end(); it++)
        {
            FrameRate rate = *it;
            QString rateString = QString::fromStdString(getFrameRateString(rate));
            map[rateString] = rate;
        }
        return map;
    }


    QMap<QString,TriggerType> getStringToTriggerTypeMap()
    {
        QMap<QString,TriggerType> map;
        TriggerTypeList typeList = getListOfTriggerTypes();
        TriggerTypeList::iterator it;
        for (it=typeList.begin(); it!=typeList.end(); it++)
        {
            TriggerType trigType = *it;
            QString typeString = QString::fromStdString(getTriggerTypeString(trigType));
            map[typeString] = trigType;
        }
        return map;
    }


    QMap<QString,ImageMode> getStringToImageModeMap()
    {
        QMap<QString,ImageMode> map;
        ImageModeList modeList = getListOfImageModes();
        ImageModeList::iterator it;
        for (it=modeList.begin(); it!=modeList.end(); it++)
        {
            ImageMode mode = *it;
            QString modeString = QString::fromStdString(getImageModeString(mode));
            map[modeString] = mode;
        }
        return map;
    }


    QMap<QString,PixelFormat> getStringToPixelFormatMap()
    {
        QMap<QString, PixelFormat> map;
        PixelFormatList formatList = getListOfPixelFormats();
        PixelFormatList::iterator it;
        for (it=formatList.begin(); it!=formatList.end(); it++)
        {
            PixelFormat format = *it;
            QString formatString = QString::fromStdString(getPixelFormatString(format));
            map[formatString] = format;
        }
        return map;
    }

    
    QString propNameToCamelCase(QString propName)
    {
        QString camelCaseName; 
        camelCaseName.append(propName[0].toLower());
        for (unsigned int i=1; i<propName.length(); i++)
        {
            if (propName[i] != QChar(' '))
            {
                camelCaseName.append(propName[i]);
            }
        }
        return camelCaseName;
    }
} // namespace bias

       

