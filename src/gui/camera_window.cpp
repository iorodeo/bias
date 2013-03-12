#include "camera_window.hpp"
#include "camera_facade.hpp"
#include "mat_to_qimage.hpp"
#include "stamped_image.hpp"
#include "lockable.hpp"
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
#include <cstdlib>
#include <cmath>
#include <QtGui>
#include <QTimer>
#include <QThreadPool>
#include <QSignalMapper>
#include <QVariantMap>
#include <QFile>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

//#include <qjson/serializer.h>
//#include <qjson/parser.h>
#include "json.hpp"


namespace bias
{
    // Constants
    // ----------------------------------------------------------------------------------
    
    const unsigned int DURATION_TIMER_INTERVAL = 1000; // msec
    const QSize PREVIEW_DUMMY_IMAGE_SIZE = QSize(320,256);
    const unsigned int JSON_INDENT_STEP = 4;

    // Default settings
    const unsigned long DEFAULT_CAPTURE_DURATION = 300; // sec
    const double DEFAULT_IMAGE_DISPLAY_FREQ = 10.0;     // Hz
    const QSize DEFAULT_HISTOGRAM_IMAGE_SIZE = QSize(256,204);
    const QString DEFAULT_VIDEO_FILE_NAME = QString("bias_video");
    const QString DEFAULT_CONFIG_FILE_NAME = QString("bias_config");
    const QString CONFIG_FILE_EXTENSION = QString("json");

    QMap<VideoFileFormat, QString> createExtensionMap()
    {
        QMap<VideoFileFormat, QString> map;
        map.insert(VIDEOFILE_FORMAT_BMP,  QString(""));
        map.insert(VIDEOFILE_FORMAT_AVI,  QString("avi"));
        map.insert(VIDEOFILE_FORMAT_FMF,  QString("fmf"));
        map.insert(VIDEOFILE_FORMAT_UFMF, QString("ufmf"));
        return map;
    };
    const QMap<VideoFileFormat, QString> VIDEOFILE_EXTENSION_MAP = createExtensionMap();


    // Public methods
    // ----------------------------------------------------------------------------------

    CameraWindow::CameraWindow(Guid cameraGuid, QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);
        connectWidgets();
        initialize(cameraGuid);
    }


    void CameraWindow::saveConfiguration(QString fileName)
    {
        // --------------------------------------------------------------------
        // TO DO  ... need to add error checking. 
        // --------------------------------------------------------------------
        if (!connected_) { return; }
        QByteArray jsonConfig = getConfiguration();
        QByteArray jsonConfigPretty = prettyIndentJson(jsonConfig); 
        QFile configFile(fileName);
        configFile.open(QIODevice::WriteOnly);
        configFile.write(jsonConfigPretty);
        configFile.close();
    }


    void CameraWindow::loadConfiguration(QString fileName)
    {
        // --------------------------------------------------------------------
        // TO DO  ... need to add error checking. 
        // --------------------------------------------------------------------
        QFile configFile(fileName);
        if (!configFile.exists())
        {
            // ----------------------------------------------------------------
            // TO DO ... error message
            // ----------------------------------------------------------------
            return;
        }
        configFile.open(QIODevice::ReadOnly);
        QByteArray jsonConfig = configFile.readAll();
        configFile.close();
        setConfiguration(jsonConfig);
    }


    QByteArray CameraWindow::getConfiguration()
    {
        // --------------------------------------------------------------------
        // TO DO  ... need to add error checking. 
        // --------------------------------------------------------------------
        QByteArray jsonConfig;

        if (!connected_) { return jsonConfig; }

        QVariantMap configurationMap;

        //QJson::Serializer serializer;
        //serializer.setIndentMode(QJson::IndentFull);

        // Add camera configuration 
        QVariantMap cameraMap;

        // --------------------------------------------------------------------
        // CAMERA LOCKED  - need to add error checking!!
        // --------------------------------------------------------------------
        cameraPtr_ -> acquireLock();

        QString vendorName = QString::fromStdString(cameraPtr_ -> getVendorName());
        cameraMap.insert("Vendor", vendorName); 

        QString modelName = QString::fromStdString(cameraPtr_ -> getModelName());
        cameraMap.insert("Model", modelName);

        QString guidString = QString::fromStdString((cameraPtr_ -> getGuid()).toString());
        cameraMap.insert("GUID", guidString);

        // Add camera properties
        QVariantMap cameraPropMap;
        PropertyList propList = cameraPtr_ -> getListOfProperties();
        for (
                PropertyList::iterator it = propList.begin();
                it != propList.end();
                it++
            )
        {
            Property prop = *it;
            QString propName = QString::fromStdString(getPropertyTypeString(prop.type));
            QVariantMap valueMap;
            valueMap.insert("Present", prop.present);
            valueMap.insert("Absolute Control", prop.absoluteControl);
            valueMap.insert("One Push", prop.onePush);
            valueMap.insert("On", prop.on);
            valueMap.insert("Auto Active", prop.autoActive);
            valueMap.insert("Value", prop.value);
            valueMap.insert("Absolute Value", prop.absoluteValue);
            cameraPropMap.insert(propName, valueMap);
        }

        cameraMap.insert("Properties", cameraPropMap);

        // Add videomode, framerate and trigger information
        VideoMode videoMode = cameraPtr_ -> getVideoMode();
        QString videoModeString = QString::fromStdString(getVideoModeString(videoMode));
        cameraMap.insert("Video Mode", videoModeString);
        
        FrameRate frameRate = cameraPtr_ -> getFrameRate();
        QString frameRateString = QString::fromStdString(getFrameRateString(frameRate));
        cameraMap.insert("Frame Rate", frameRateString);

        TriggerType trigType = cameraPtr_ -> getTriggerType();
        QString trigTypeString = QString::fromStdString(getTriggerTypeString(trigType));
        cameraMap.insert("Trigger Type", trigTypeString);
        
        cameraPtr_ -> releaseLock();
        // ------------------------------------------------------------------------
        // CAMERA UNLOCKED
        // ------------------------------------------------------------------------
        configurationMap.insert("Camera", cameraMap);

        // Add logging information
        QVariantMap loggingMap;
        loggingMap.insert("Enabled", logging_);
        loggingMap.insert("Format", VIDEOFILE_EXTENSION_MAP[videoFileFormat_]);
        loggingMap.insert("Directory", currentVideoFileDir_.canonicalPath());
        loggingMap.insert("File Name", currentVideoFileName_);
        
        // Add logging configuration 
        QVariantMap loggingSettingsMap;
        
        QVariantMap bmpSettingsMap;
        bmpSettingsMap.insert("Frame Skip", videoWriterParams_.bmp.frameSkip);
        loggingSettingsMap.insert("bmp", bmpSettingsMap);

        QVariantMap aviSettingsMap;
        aviSettingsMap.insert("Frame Skip", videoWriterParams_.avi.frameSkip);
        aviSettingsMap.insert("Codec", videoWriterParams_.avi.codec);
        loggingSettingsMap.insert("avi", aviSettingsMap);

        QVariantMap fmfSettingsMap;
        fmfSettingsMap.insert("Frame Skip", videoWriterParams_.fmf.frameSkip);
        loggingSettingsMap.insert("fmf", fmfSettingsMap);

        QVariantMap ufmfSettingsMap;
        ufmfSettingsMap.insert("Frame Skip", videoWriterParams_.ufmf.frameSkip);
        ufmfSettingsMap.insert("Background Threshold", videoWriterParams_.ufmf.backgroundThreshold);
        ufmfSettingsMap.insert("Box Length", videoWriterParams_.ufmf.boxLength);
        ufmfSettingsMap.insert("Median Update Count", videoWriterParams_.ufmf.medianUpdateCount);
        ufmfSettingsMap.insert("Compression Threads", videoWriterParams_.ufmf.numberOfCompressors);

        QVariantMap ufmfDilateMap;
        ufmfDilateMap.insert("On", videoWriterParams_.ufmf.dilateState);
        ufmfDilateMap.insert("Window Size", videoWriterParams_.ufmf.dilateWindowSize);
        ufmfSettingsMap.insert("Dilate", ufmfDilateMap);
        
        loggingSettingsMap.insert("ufmf", ufmfSettingsMap);
        loggingMap.insert("Settings", loggingSettingsMap);
        configurationMap.insert("Logging", loggingMap);

        // Add Timer configuration
        QVariantMap timerMap;
        timerMap.insert("Enabled", actionTimerEnabledPtr_ -> isChecked());
        QVariantMap timerSettingsMap;
        timerSettingsMap.insert("Duration", qulonglong(captureDurationSec_));
        timerMap.insert("Settings", timerSettingsMap);
        configurationMap.insert("Timer", timerMap);

        // Add display configuration
        QVariantMap displayMap;
        QVariantMap orientationMap;
        orientationMap.insert("Flip Vertical", flipVert_ );
        orientationMap.insert("Flip Horizontal", flipHorz_);
        displayMap.insert("Orientation", orientationMap);
        displayMap.insert("Rotation", (unsigned int)(imageRotation_));
        displayMap.insert("Update Frequency", imageDisplayFreq_);
        configurationMap.insert("Display", displayMap);

        // Add configuration configuration
        QVariantMap configConfigMap;
        configConfigMap.insert("Directory", currentConfigFileDir_.canonicalPath());
        configConfigMap.insert("File Name", currentConfigFileName_);
        configurationMap.insert("Configuration", configConfigMap);

        // Serialize configuration
        bool ok;
        //jsonConfig = serializer.serialize(configurationMap,&ok);
        jsonConfig = QtJson::serialize(configurationMap,ok);
        if (!ok)
        {
            std::cout << "Error converting config to json - unable to serialize" << std::endl;
        }
        return jsonConfig;
    }


    void CameraWindow::setConfiguration(QByteArray jsonConfig)
    {
        std::cout << QString(jsonConfig).toStdString() << std::endl;

        bool ok;
        QVariantMap configurationMap = QtJson::parse(QString(jsonConfig), ok).toMap();
        if (!ok)
        {
            // ---------------------------------------------------
            // TO DO  ... error message
            // ---------------------------------------------------
            std::cout << "configuration parsing error" << std::endl;
        }
        QVariantMap cameraMap = configurationMap["Camera"].toMap();
        std::cout << "Frame Rate: " << cameraMap["Frame Rate"].toString().toStdString() << std::endl;
        
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

    void CameraWindow::connectButtonClicked()
    {
        (!connected_) ? connectCamera() : disconnectCamera();
        std::cout << "connected: "; 
        std::cout << std::boolalpha << connected_ << std::noboolalpha << std::endl;
    }


    void CameraWindow::startButtonClicked()
    { 
        (!capturing_) ? startImageCapture() : stopImageCapture();
        std::cout << "capturing: ";
        std::cout << std::boolalpha << capturing_ << std::noboolalpha << std::endl;
    }
   

    void CameraWindow::updateDisplayOnTimer()
    {
        // Get information from image dispatcher
        // -------------------------------------------------------------------
        imageDispatcherPtr_ -> acquireLock();

        cv::Mat imgMat = imageDispatcherPtr_ -> getImage();
        QImage img = matToQImage(imgMat);
        double fps = imageDispatcherPtr_ -> getFPS();
        double stamp = imageDispatcherPtr_ -> getTimeStamp();
        frameCount_ = imageDispatcherPtr_ -> getFrameCount();
        cv::Mat histMat = calcHistogram(imgMat);
        cv::Size imgSize = imgMat.size();

        imageDispatcherPtr_ -> releaseLock();
        // -------------------------------------------------------------------

        // Set pixmaps and update image labels - note need to add pluginPixmap
        if (!img.isNull()) 
        {
            previewPixmapOriginal_ = QPixmap::fromImage(img);
        }
        updateHistogramPixmap(histMat);

        updateAllImageLabels();

        // Update statusbar message
        QString statusMsg("Capturing,  logging = ");
        statusMsg += boolToOnOffQString(logging_);
        statusMsg += QString(", timer = ");
        statusMsg += boolToOnOffQString(actionTimerEnabledPtr_ -> isChecked());
        statusMsg += QString().sprintf(",  %dx%d", imgSize.width, imgSize.height);
        statusMsg += QString().sprintf(",  %1.1f fps", fps);
        statusbarPtr_ -> showMessage(statusMsg);

        // Set update caputure time 
        QDateTime currentDateTime = QDateTime::currentDateTime();
        qint64 captureDt = currentDateTime.toMSecsSinceEpoch();
        captureDt -= captureStartDateTime_.toMSecsSinceEpoch();
        setCaptureTimeLabel(double(1.0e-3*captureDt));

        // --------------------------
        //setCaptureTimeLabel(stamp); 
        // --------------------------
    }


    void CameraWindow::checkDurationOnTimer()
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        if (currentDateTime >= captureStopDateTime_)
        {
            stopImageCapture();
            std::cout << "image caputre stopped by timer" << std::endl;
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


    void CameraWindow::actionFileSaveconfigTriggered()
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

        std::cout << "dir:  " << currentConfigFileDir_.absolutePath().toStdString() << std::endl;
        std::cout << "file: " << currentConfigFileName_.toStdString() << std::endl;

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
        QString msgTitle("Development");
        QString msgText("Format7 settings not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }

    
    void CameraWindow::actionCameraTriggerExternalTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Set camera trigger external not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionCameraTriggerInternalTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Set camera trigger internal not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionLoggingEnabledTriggered()
    {
        if (haveDefaultVideoFileDir_) {
            logging_ = actionLoggingEnabledPtr_ -> isChecked();
        }
        else
        {
            actionLoggingEnabledPtr_ -> setChecked(false);
            logging_ = false;
            QString msgTitle("Initialization Error");
            QString msgText("Unable to determine default location for video files.");
            QMessageBox::critical(this, msgTitle, msgText);
        }
        std::cout << "logging: ";
        std::cout << std::boolalpha << logging_ << std::noboolalpha;
        std::cout << std::endl;
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
        videoFileInfo = QFileInfo(videoFileString);

        QDir videoFileDir = videoFileInfo.dir();
        QString videoFileName = videoFileInfo.baseName();

        // Check return results and assign values
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
        }
        if (!videoFileDir.exists())
        {
            videoFileDir = defaultVideoFileDir_;
        }

        currentVideoFileDir_ = videoFileDir;
        currentVideoFileName_ = videoFileName;

        std::cout << "dir:  " << currentVideoFileDir_.absolutePath().toStdString() << std::endl;
        std::cout << "file: " << currentVideoFileName_.toStdString() << std::endl;
    }
    

    void CameraWindow::actionLoggingSettingsTriggered()
    {
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
    }


    void CameraWindow::loggingSettingsChanged(VideoWriterParams params)
    {
        videoWriterParams_ = params;
        //std::cout << "loggingSettingsChanged" << std::endl;
        //std::cout << videoWriterParams_.toString() << std::endl;
    }


    void CameraWindow::actionLoggingFormatTriggered()
    {
        // Get Format string
        QPointer<QAction> actionPtr = qobject_cast<QAction *>(sender());
        videoFileFormat_ = actionToVideoFileFormatMap_[actionPtr]; 

        std::cout << "video file format: "; 
        if (videoFileFormat_ != VIDEOFILE_FORMAT_BMP)
        {
            std::cout << VIDEOFILE_EXTENSION_MAP[videoFileFormat_].toStdString();
        }
        else
        {
            std::cout << "bmp";
        }
        std::cout << std::endl;
    }


    void CameraWindow::actionTimerEnabledTriggered()
    {
        setCaptureTimeLabel(0.0);
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
                    SLOT(timerDurationChanged(unsigned long))
                   );
        }
        else
        {
            timerSettingsDialogPtr_ -> raise();
        }
    }


    void CameraWindow::timerDurationChanged(unsigned long duration)
    {
        captureDurationSec_ = duration;
        captureStopDateTime_ = captureStartDateTime_.addSecs(captureDurationSec_);
        setCaptureTimeLabel(0.0);
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
        updateAllImageLabels();
    }


    void CameraWindow::actionDisplayFlipHorzTriggered()
    {
        flipHorz_ = actionDisplayFlipHorzPtr_ -> isChecked();
        updateAllImageLabels();
    }


    void CameraWindow::actionDisplayRotTriggered()
    {
        QPointer<QAction> actionPtr = qobject_cast<QAction *>(sender());
        imageRotation_ = actionToRotationMap_[actionPtr];
        updateAllImageLabels();
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

    void CameraWindow::actionPluginsSettingsTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Plugin settings not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
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


    // Private methods
    // -----------------------------------------------------------------------------------

    void CameraWindow::initialize(Guid guid)
    {
        connected_ = false;
        capturing_ = false;
        logging_ = false;
        flipVert_ = false;
        flipHorz_ = false;
        haveDefaultVideoFileDir_ = false;

        imageRotation_ = IMAGE_ROTATION_0;
        videoFileFormat_ = VIDEOFILE_FORMAT_UFMF;
        captureDurationSec_ = DEFAULT_CAPTURE_DURATION;

        imageDisplayFreq_ = DEFAULT_IMAGE_DISPLAY_FREQ;
        cameraPtr_ = std::make_shared<Lockable<Camera>>(guid);

        threadPoolPtr_ = new QThreadPool(this);
        threadPoolPtr_ -> setMaxThreadCount(MAX_THREAD_COUNT);
        newImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();
        logImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();

        setDefaultFileDirs();
        currentVideoFileDir_ = defaultVideoFileDir_;
        currentVideoFileName_ = DEFAULT_VIDEO_FILE_NAME;
        currentConfigFileDir_ = defaultConfigFileDir_;
        currentConfigFileName_ = DEFAULT_CONFIG_FILE_NAME;

        setupCameraMenu();
        setupLoggingMenu();
        setupDisplayMenu();
        setupImageDisplayTimer();
        setupCaptureDurationTimer();
        setupImageLabels();
        updateAllMenus(); 

        tabWidgetPtr_ -> setCurrentWidget(previewTabPtr_);

        QString windowTitle("BIAS Camera Window, Guid: ");
        windowTitle += QString::fromStdString(guid.toString());
        setWindowTitle(windowTitle);

        updateCameraInfoMessage();
        setCaptureTimeLabel(0.0);

        connectButtonPtr_ -> setText(QString("Connect"));
        startButtonPtr_ -> setText(QString("Start"));
        statusbarPtr_ -> showMessage(QString("Camera found, disconnected"));

        startButtonPtr_ -> setEnabled(false);
        connectButtonPtr_ -> setEnabled(true);

        // Assign thread cpu affinity
        assignThreadAffinity(false,1);
    }

    void CameraWindow::setupImageLabels()
    {
        QImage dummyImage;
        dummyImage = QImage(PREVIEW_DUMMY_IMAGE_SIZE,QImage::Format_RGB888);
        dummyImage.fill(QColor(Qt::gray).rgb());
        previewPixmapOriginal_ = QPixmap::fromImage(dummyImage);
        pluginPixmapOriginal_ = QPixmap::fromImage(dummyImage);

        dummyImage = QImage(DEFAULT_HISTOGRAM_IMAGE_SIZE,QImage::Format_RGB888);
        dummyImage.fill(QColor(Qt::gray).rgb());
        histogramPixmapOriginal_ = QPixmap::fromImage(dummyImage);

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
                actionFileLoadConfigPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionFileLoadConfigTriggered())
               );

        connect(
                actionFileSaveConfigPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionFileSaveconfigTriggered())
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
                actionPluginsSettingsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionPluginsSettingsTriggered())
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
    }


    void CameraWindow::setupDisplayMenu() 
    {
        setupDisplayRotMenu();
        setupDisplayOrientMenu();
    }


    void CameraWindow::setupDisplayOrientMenu()
    {

        actionDisplayFlipVertPtr_ -> setChecked(flipVert_);
        actionDisplayFlipHorzPtr_ -> setChecked(flipHorz_);
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


    void CameraWindow::updateImageLabel(
            QLabel *imageLabelPtr, 
            QPixmap &pixmapOriginal,
            bool flipAndRotate,
            bool addFrameCount
            )
    {
        // Updates pixmap of image on Qlabel - sizing based on QLabel size

        QPixmap pixmapScaled =  pixmapOriginal.scaled(
                imageLabelPtr -> size(),
                Qt::KeepAspectRatio, 
                Qt::SmoothTransformation
                );
        
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
        if (addFrameCount && (frameCount_ > 0))
        {
            QPainter painter(&pixmapScaled);
            QString msg;  
            msg.sprintf("%d",frameCount_);
            painter.setPen(QColor(0,220,0));
            painter.drawText(5,12, msg);
        }

        imageLabelPtr -> setPixmap(pixmapScaled);
    }


    void CameraWindow::updateAllImageLabels()
    { 
        updateImageLabel(previewImageLabelPtr_, previewPixmapOriginal_, true, true);
        updateImageLabel(pluginImageLabelPtr_, pluginPixmapOriginal_, true, false);
        updateImageLabel(histogramImageLabelPtr_, histogramPixmapOriginal_, false, false);
    }


    void CameraWindow::resizeImageLabel( 
            QLabel *imageLabelPtr, 
            QPixmap &pixmapOriginal, 
            bool flipAndRotate,
            bool addFrameCount
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
                    addFrameCount
                    );
        }
    }

    void CameraWindow::resizeAllImageLabels()
    { 
        resizeImageLabel(previewImageLabelPtr_, previewPixmapOriginal_, true);
        resizeImageLabel(pluginImageLabelPtr_, pluginPixmapOriginal_, false);
        resizeImageLabel(histogramImageLabelPtr_, histogramPixmapOriginal_, false);
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

    void CameraWindow::connectCamera() 
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg;

        cameraPtr_ -> acquireLock();
        try
        {
            cameraPtr_ -> connect();
        }
        catch (RuntimeError &runtimeError)
        {
            error = true;
            errorId = runtimeError.id();
            errorMsg = QString::fromStdString(runtimeError.what());
        }
        cameraPtr_ -> releaseLock();

        if (error)
        {
            QString msgTitle("Connection Error");
            QString msgText("Failed to connect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }
        connected_ = true;
        connectButtonPtr_ -> setText(QString("Disconnect"));
        statusbarPtr_ -> showMessage(QString("Connected, Stopped"));
        startButtonPtr_ -> setEnabled(true);
        menuCameraPtr_ -> setEnabled(true);

        updateCameraInfoMessage();
        updateAllMenus();
    }


    void CameraWindow::disconnectCamera()
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg; 

        if (capturing_) 
        {
            stopImageCapture();
        }

        cameraPtr_ -> acquireLock();
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

        if (error)
        {
            QString msgTitle("Disconnection Error");
            QString msgText("Failed to disconnect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }
        connected_ = false;
        connectButtonPtr_ -> setText(QString("Connect"));
        statusbarPtr_ -> showMessage(QString("Disconnected"));
        startButtonPtr_ -> setEnabled(false);
        menuCameraPtr_ -> setEnabled(false);
        updateCameraInfoMessage();
        setCaptureTimeLabel(0.0);

        updateAllMenus();
    }

    
    void CameraWindow::startImageCapture() 
    {
        if (!connected_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to start image capture: not connected");
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        newImageQueuePtr_ -> clear();
        logImageQueuePtr_ -> clear();

        imageGrabberPtr_ = new ImageGrabber(cameraPtr_, newImageQueuePtr_);
        imageDispatcherPtr_ = new ImageDispatcher(
                logging_, 
                newImageQueuePtr_,
                logImageQueuePtr_
                );

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

        threadPoolPtr_ -> start(imageGrabberPtr_);
        threadPoolPtr_ -> start(imageDispatcherPtr_);

        if (logging_)
        {
            // Create video writer based on video file format type
            std::shared_ptr<VideoWriter> videoWriterPtr; 
            QString videoFileFullPath = getVideoFileFullPathWithGuid();

            switch (videoFileFormat_)
            {
                case VIDEOFILE_FORMAT_BMP:
                    videoWriterPtr = std::make_shared<VideoWriter_bmp>(
                            videoWriterParams_.bmp,
                            videoFileFullPath
                            );
                    break;

                case VIDEOFILE_FORMAT_AVI:  
                    videoWriterPtr = std::make_shared<VideoWriter_avi>(
                            videoWriterParams_.avi,
                            videoFileFullPath
                            );
                    break;

                case VIDEOFILE_FORMAT_FMF:
                    videoWriterPtr = std::make_shared<VideoWriter_fmf>(
                            videoWriterParams_.fmf,
                            videoFileFullPath
                            );
                    break;

                case VIDEOFILE_FORMAT_UFMF:
                    videoWriterPtr = std::make_shared<VideoWriter_ufmf>(
                            videoWriterParams_.ufmf,
                            videoFileFullPath
                            );
                    break;

                default:
                    videoWriterPtr = std::make_shared<VideoWriter>(
                            videoFileFullPath
                            );
                    break;
            }

            // Set output file
            //QString videoFileFullPath = getVideoFileFullPathWithGuid();
            videoWriterPtr -> setFileName(videoFileFullPath);

            imageLoggerPtr_ = new ImageLogger(videoWriterPtr, logImageQueuePtr_);

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
        }

        // Start display update timer
        unsigned int imageDisplayDt = int(1000.0/imageDisplayFreq_);
        imageDisplayTimerPtr_ -> start(imageDisplayDt);

        // Set Capture start and stop time
        captureStartDateTime_ = QDateTime::currentDateTime();
        captureStopDateTime_ = captureStartDateTime_.addSecs(captureDurationSec_);

        // Start duration timer - if enabled
        if (actionTimerEnabledPtr_ -> isChecked())
        {
            captureDurationTimerPtr_ -> start();
        }

        // Update GUI widget for capturing state
        startButtonPtr_ -> setText(QString("Stop"));
        connectButtonPtr_ -> setEnabled(false);
        statusbarPtr_ -> showMessage(QString("Capturing"));
        capturing_ = true;
        updateAllMenus();

        emit imageCaptureStarted();
    }


    void CameraWindow::stopImageCapture()
    {
        if (!connected_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to stop image capture: not connected");
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        // Stop timers.
        imageDisplayTimerPtr_ -> stop();
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

        // Wait until threads are finished
        threadPoolPtr_ -> waitForDone();

        // Clear any stale data out of existing queues
        newImageQueuePtr_ -> acquireLock();
        newImageQueuePtr_ -> clear();
        newImageQueuePtr_ -> releaseLock();

        logImageQueuePtr_ -> acquireLock();
        logImageQueuePtr_ -> clear();
        logImageQueuePtr_ -> releaseLock();

        // Update data GUI information
        startButtonPtr_ -> setText(QString("Start"));
        connectButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Connected, Stopped"));
        capturing_ = false;

        updateAllMenus();

        emit imageCaptureStopped();
    }


    void CameraWindow::updateCameraInfoMessage()
    {
        if (connected_) 
        {
            cameraPtr_ -> acquireLock();
            QString vendorName = QString::fromStdString(
                    cameraPtr_ -> getVendorName()
                    );
            QString modelName = QString::fromStdString( 
                    cameraPtr_ -> getModelName()
                    );
            cameraPtr_ -> releaseLock();
            setCameraInfoMessage(vendorName, modelName);
        }
        else
        {
            setCameraInfoMessage("_____", "_____");
        }

    }

    
    void CameraWindow::setCameraInfoMessage(QString vendorName, QString modelName)
    {
        QString cameraInfoString("Camera:  ");
        cameraInfoString += vendorName;
        cameraInfoString += QString(",  ");
        cameraInfoString += modelName; 
        cameraInfoLabelPtr_ -> setText(cameraInfoString);
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
    }

    void CameraWindow::updateFileMenu()
    {
        if (capturing_ || !connected_)
        {
            actionFileLoadConfigPtr_ -> setEnabled(false);
            actionFileSaveConfigPtr_ -> setEnabled(false);
        }
        else
        {
            actionFileLoadConfigPtr_ -> setEnabled(true);
            actionFileSaveConfigPtr_ -> setEnabled(true);
        }
    }


    void CameraWindow::updateCameraMenu()
    {
        updateCameraVideoModeMenu();
        updateCameraFrameRateMenu();
        updateCameraPropertiesMenu();
        updateCameraTriggerMenu();

        if (connected_) 
        { 
            setMenuChildrenEnabled(menuCameraPtr_,true); 
        }
        else
        {
            setMenuChildrenEnabled(menuCameraPtr_,false);
        }

        if (capturing_ || !connected_)
        {
            actionCameraFormat7SettingsPtr_ -> setEnabled(false);
        }
        else
        {
            actionCameraFormat7SettingsPtr_ -> setEnabled(true);
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
        cameraPtr_ -> acquireLock();
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
            QPointer<QAction> modeActionPtr = 
                menuCameraVideoModePtr_ -> addAction(modeString);
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
        cameraPtr_ -> acquireLock();
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

        if (!connected_) { return; }

        // Remove any existing actions from menu
        deleteMenuActions(menuCameraPropertiesPtr_);

        // Get list of properties from camera 
        cameraPtr_ -> acquireLock();
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

        cameraPtr_ -> acquireLock();
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
            actionCameraTriggerExternalPtr_ -> setEnabled(false);
        }
    }


    void CameraWindow::updateLoggingMenu() 
    {
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


    QString CameraWindow::getVideoFileFullPath()
    {
        QString fileExtension = VIDEOFILE_EXTENSION_MAP[videoFileFormat_];
        QString fileName = currentVideoFileName_;
        if (!fileExtension.isEmpty())
        {
            fileName +=  "." + fileExtension;
        }
        QFileInfo videoFileInfo(currentVideoFileDir_, fileName);
        QString videoFileFullPath = videoFileInfo.absoluteFilePath();
        return videoFileFullPath;
    }

    QString CameraWindow::getVideoFileFullPathWithGuid()
    {
        cameraPtr_ -> acquireLock();
        Guid cameraGuid = cameraPtr_ -> getGuid();
        cameraPtr_ -> releaseLock();
        
        QString fileExtension = VIDEOFILE_EXTENSION_MAP[videoFileFormat_];
        QString fileName = currentVideoFileName_;
        fileName += "_guid_" + QString::fromStdString(cameraGuid.toString());
        if (!fileExtension.isEmpty())
        {
            fileName +=  "." + fileExtension;
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


    QByteArray prettyIndentJson(QByteArray jsonArray)
    {
        QByteArray jsonArrayNew;
        unsigned int pos = 0;
        unsigned int indentLevel = 0;
        unsigned int indentStep = JSON_INDENT_STEP;
        unsigned int indent = 0;

        while (pos < jsonArray.size()) 
        {
            if (jsonArray[pos] == '}' || jsonArray[pos] == ']')
            {
                addNewLineToByteArray(jsonArrayNew);
                indentLevel -= 1;
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
                jsonArrayNew.append(jsonArray[pos]);
            }
            else if ((jsonArray[pos] =='{') || (jsonArray[pos] == '[') )
            {
                jsonArrayNew.append(jsonArray[pos]);
                addNewLineToByteArray(jsonArrayNew);
                indentLevel += 1;
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
            }
            else if (jsonArray[pos] == ',')
            {
                jsonArrayNew.append(jsonArray[pos]);
                addNewLineToByteArray(jsonArrayNew);
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
            }
            else
            {
                jsonArrayNew.append(jsonArray[pos]);
            }
            pos++;
        }
        return jsonArrayNew;
    }


    void addIndentToByteArray(QByteArray &array, unsigned int width)
    {
        for (unsigned int i=0; i<width; i++)
        {
            array.append(' ');
        }
    }


    void addNewLineToByteArray(QByteArray &array)
    {
#ifdef WIN32
        array.append('\r');
#endif
        array.append('\n');
    }

} // namespace bias

       

