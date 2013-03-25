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
#include "background_histogram_ufmf.hpp"
#include "json.hpp"
#include "basic_http_server.hpp"

#include <cstdlib>
#include <cmath>
#include <iostream>

#include <QtGui>
#include <QTimer>
#include <QThreadPool>
#include <QSignalMapper>
#include <QFile>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QTcpServer>


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
    const double MAX_IMAGE_DISPLAY_FREQ = 60.0;         // Hz
    const double MIN_IMAGE_DISPLAY_FREQ = 1.0;          // Hz 
    const QSize DEFAULT_HISTOGRAM_IMAGE_SIZE = QSize(256,204);
    const QString DEFAULT_VIDEO_FILE_NAME = QString("bias_video");
    const QString DEFAULT_CONFIG_FILE_NAME = QString("bias_config");
    const QString CONFIG_FILE_EXTENSION = QString("json");

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
        QByteArray jsonConfig = getConfigurationJson();
        if (jsonConfig.isEmpty()) 
        { 
            return; 
        }
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
            QString errMsgTitle("Load Configuration Error");
            QString errMsgText("Error configuration file does not exist");
            QMessageBox::critical(this, errMsgTitle, errMsgText);
            return;
        }
        configFile.open(QIODevice::ReadOnly);
        QByteArray jsonConfig = configFile.readAll();
        configFile.close();
        setConfigurationFromJson(jsonConfig);
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

        std::cout << "video file format: "; 
        std::cout << VIDEOFILE_EXTENSION_MAP[videoFileFormat_].toStdString();
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
    

    void CameraWindow::actionPluginsSettingsTriggered()
    {
        QString msgTitle("Development");
        QString msgText("Plugin settings not fully implemented");
        QMessageBox::information(this, msgTitle, msgText);
    }


    void CameraWindow::actionServerEnabledTriggered()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        if (actionServerEnabledPtr_ -> isChecked())
        {
            httpServerPtr_ -> listen(QHostAddress::Any, 5000);

        }
        else
        {
            httpServerPtr_ -> close();
        }
    }


    void CameraWindow::actionServerSettingsTriggered()
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


    void CameraWindow::handleHttpRequest(QMap<QString, QString> paramsMap)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (paramsMap.size() == 1)
        {
            if (paramsMap.contains("connect"))
            {
                std::cout << "connect" << std::endl;
            }
            else if (paramsMap.contains("start"))
            {
                std::cout << "start" << std::endl;
            }
            else if (paramsMap.contains("stop"))
            {
                std::cout << "stop" << std::endl;
            }
        }

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

        httpServerPtr_ = new BasicHttpServer(this);
        connect(
                httpServerPtr_,
                SIGNAL(httpRequest(QMap<QString,QString>)),
                this,
                SLOT(handleHttpRequest(QMap<QString,QString>))
               );
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
                actionServerSettingsPtr_,
                SIGNAL(triggered()),
                this,
                SLOT(actionServerSettingsTriggered())
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
        updateLoggingMenu();
    }


    void CameraWindow::setupDisplayMenu() 
    {
        setupDisplayRotMenu();
        setupDisplayOrientMenu();
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
            // TEMPORARY - set camera to known videomode and trigger type
            // ------------------------------------------------------------
            cameraPtr_ -> setVideoMode(VIDEOMODE_FORMAT7);
            cameraPtr_ -> setTriggerInternal();
            // ------------------------------------------------------------
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
        updateDisplayMenu();
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
        }
    }


    void CameraWindow::updateDisplayMenu()
    {
        updateDisplayOrientMenu();
        updateDisplayRotMenu();
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

    QString CameraWindow::getVideoFileFullPathWithGuid()
    {
        cameraPtr_ -> acquireLock();
        Guid cameraGuid = cameraPtr_ -> getGuid();
        cameraPtr_ -> releaseLock();
        
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
    

    QByteArray CameraWindow::getConfigurationJson()
    {
        // Get configuration map
        QVariantMap configurationMap = getConfigurationMap();
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
      

    QVariantMap CameraWindow::getConfigurationMap()
    {
        if (!connected_) { 
            QVariantMap emptyMap = QVariantMap();
            return emptyMap; 
        }

        // Get configuration values from camera
        QString vendorName;
        QString modelName;
        QString guidString;
        PropertyList propList;
        VideoMode videoMode;
        FrameRate frameRate;
        TriggerType trigType;
        QString errorMsg;
        bool error = false;
        unsigned int errorId;

        cameraPtr_ -> acquireLock();
        try
        { 
            vendorName = QString::fromStdString(cameraPtr_ -> getVendorName());
            modelName = QString::fromStdString(cameraPtr_ -> getModelName());
            guidString = QString::fromStdString((cameraPtr_ -> getGuid()).toString());
            propList = cameraPtr_ -> getListOfProperties();
            videoMode = cameraPtr_ -> getVideoMode();
            frameRate = cameraPtr_ -> getFrameRate();
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
            QString msgText("Error retrieving values from camera.\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            QVariantMap emptyMap = QVariantMap();
        }

        // Create configuration map 
        QVariantMap configurationMap;
        QVariantMap cameraMap;

        cameraMap.insert("Vendor", vendorName); 
        cameraMap.insert("Model", modelName);
        cameraMap.insert("GUID", guidString);

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
            valueMap.insert("Present", prop.present);
            valueMap.insert("Absolute Control", prop.absoluteControl);
            valueMap.insert("One Push", prop.onePush);
            valueMap.insert("On", prop.on);
            valueMap.insert("Auto Active", prop.autoActive);
            valueMap.insert("Value", prop.value);
            valueMap.insert("Absolute Value", double(prop.absoluteValue));
            cameraPropMap.insert(propName, valueMap);
        }
        cameraMap.insert("Properties", cameraPropMap);

        // Add videomode, framerate and trigger information
        QString videoModeString = QString::fromStdString(getVideoModeString(videoMode));
        cameraMap.insert("Video Mode", videoModeString);
        QString frameRateString = QString::fromStdString(getFrameRateString(frameRate));
        cameraMap.insert("Frame Rate", frameRateString);
        QString trigTypeString = QString::fromStdString(getTriggerTypeString(trigType));
        cameraMap.insert("Trigger Type", trigTypeString);
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
        ufmfSettingsMap.insert("Median Update Interval", videoWriterParams_.ufmf.medianUpdateInterval);
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
        QVariantMap configFileMap;
        configFileMap.insert("Directory", currentConfigFileDir_.canonicalPath());
        configFileMap.insert("File Name", currentConfigFileName_);
        configurationMap.insert("Configuration", configFileMap);

        return configurationMap;
    }


    bool CameraWindow::setConfigurationFromJson(QByteArray jsonConfig)
    {
        bool ok;
        QVariantMap configMap = QtJson::parse(QString(jsonConfig), ok).toMap();
        QVariantMap oldConfigMap = getConfigurationMap();
        QString errMsgTitle("Load Configuration Error");
        if (!ok)
        {
            QString errMsgText("Error loading configuration - "); 
            errMsgText += "unable to parse json.";
            QMessageBox::critical(this, errMsgTitle, errMsgText);
            return false;
        }

        ok = setConfigurationFromMap(configMap);
        if (!ok)
        {
            // Something went wrong - try to revert to old configuration
            ok = setConfigurationFromMap(oldConfigMap);
            if (!ok)
            {
                QString errMsgText("Error loading configuration and ");  
                errMsgText += "unable to revert to previous configuration";
                QMessageBox::critical(this, errMsgTitle, errMsgText);
                return false;
            }
            else
            {
                QString errMsgText("Error loading configuration - ");  
                errMsgText += "reverting to previous configuration";
                QMessageBox::critical(this, errMsgTitle, errMsgText);
                return false;
            }
        }
        updateAllMenus();
        return true;
    }


    bool CameraWindow::setConfigurationFromMap(QVariantMap configMap)
    {
        bool ok = true;
        QString errMsgTitle("Load Configuration Error");

        // Set camera properties, videomode, etc.
        QVariantMap cameraMap = configMap["Camera"].toMap();
        if (cameraMap.isEmpty())
        {
            QString errMsgText("Camera configuration is empty");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!setCameraFromMap(cameraMap))
        {
            return false;
        }

        // Set logging configuration
        // --------------------------
        QVariantMap loggingMap = configMap["Logging"].toMap();
        if (loggingMap.isEmpty())
        {
            QString errMsgText("Logging configuration is empty");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!setLoggingFromMap(loggingMap))
        {
            return false;
        }

        // Set timer configuration
        // ------------------------
        QVariantMap timerMap = configMap["Timer"].toMap();
        if (timerMap.isEmpty())
        {
            QString errMsgText("Timer configuration is empty");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!setTimerFromMap(timerMap))
        {
            return false;
        }

        // Set display configuration
        // --------------------------
        QVariantMap displayMap = configMap["Display"].toMap();
        if (displayMap.isEmpty())
        {
            QString errMsgText("Display configuration is empty");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!setDisplayFromMap(displayMap))
        {
            return false;
        }

        // Set configuration file configuraiton 
        // -------------------------------------
        QVariantMap configFileMap = configMap["Configuration"].toMap();
        if (configFileMap.isEmpty())
        {
            QString errMsgText("Configuration file information is empty");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!setConfigFileFromMap(configFileMap))
        {
            return false;
        }

        return true;
    }


    bool CameraWindow::setCameraFromMap(QVariantMap cameraMap)
    {
        // Get Values from the camera - for making sure that vendor and model match etc.
        QString errMsgTitle("Load Configuration Error (Camera)");
        QString currVendorName;
        QString currModelName;
        PropertyList currCameraPropList;
        PropertyInfoMap cameraPropInfoMap;
        QString errorMsg;
        bool error = false;
        unsigned int errorId;

        cameraPtr_ -> acquireLock();
        try
        {
            currVendorName = QString::fromStdString(cameraPtr_ -> getVendorName());
            currModelName = QString::fromStdString(cameraPtr_ -> getModelName());
            currCameraPropList = cameraPtr_ -> getListOfProperties();
            cameraPropInfoMap = cameraPtr_ -> getMapOfPropertyInfos();
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
            QString msgText("Error retrieving values from camera.\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            QByteArray emptyByteArray = QByteArray();
            return false;
        }

        QString vendorName = cameraMap["Vendor"].toString();
        if (vendorName.isEmpty())
        {
            QString errMsgText("Camera vendor name is not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (vendorName != currVendorName)
        {
            QString errMsgText("Current camera vendor does not match that in configuration file");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        QString modelName = cameraMap["Model"].toString();
        if (modelName.isEmpty())
        {
            QString errMsgText("Camera model name is not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (modelName != currModelName)
        {
            QString errMsgText("Current camera model does not match that in configuration file");
            QMessageBox::critical(this,errMsgTitle, errMsgText);
            return false;
        }

        // Try to set the camera properties
        QVariantMap cameraPropMap = cameraMap["Properties"].toMap();
        if (cameraPropMap.isEmpty())
        {
            QString errMsgText("Camera properties are not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
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
            std::cout << prop.toString() << std::endl;
            std::cout << propInfo.toString() << std::endl;
            QString name = QString::fromStdString(getPropertyTypeString(prop.type));

            QVariantMap propValueMap = cameraPropMap[name].toMap();
            if (propValueMap.isEmpty())
            {
                QString errMsgText = QString(
                        "Camera property %1 is not present"
                        ).arg(name);
                QMessageBox::critical(this,errMsgTitle,errMsgText);
                return false;
            }
            bool ok = setCameraPropertyFromMap(propValueMap, propInfo);
            if (!ok)
            {
                return false;
            }
        } // for ( propListIt ...

        // Video Mode
        // ----------
        QString videoModeString = cameraMap["Video Mode"].toString();
        if (videoModeString.isEmpty())
        {
            QString errMsgText("Video mode is not present in configuration");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        VideoMode videoMode = convertStringToVideoMode(videoModeString);

        // Frame Rate
        // ----------
        QString frameRateString = cameraMap["Frame Rate"].toString();
        if (frameRateString.isEmpty())
        {
            QString errMsgText("Camera frame rate is not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
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
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;   
        }
        if (frameRate != FRAMERATE_FORMAT7)
        {
            QString errMsgText = QString("Development Error: frameRate = %1").arg(frameRateString); 
            errMsgText += "\n\n";
            errMsgText += "currently only frameRatee=Format7 supported";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        // --------------------------------------------------------------------
        // TO DO - check if videoMode and frameRate are allowed and if so set
        // to new value.
        // --------------------------------------------------------------------

        // Trigger Type
        QString triggerTypeString = cameraMap["Trigger Type"].toString();
        if (triggerTypeString.isEmpty())
        {
            QString errMsgText("Camera trigger type is not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        TriggerType triggerType = convertStringToTriggerType(triggerTypeString);

        // --------------------------------------------------------------------
        // TEMPORARY - currently only allow internal trigger
        // --------------------------------------------------------------------
        if (triggerType != TRIGGER_INTERNAL)
        {
            QString errMsgText = QString("Development Error: triggerType = %1").arg(triggerTypeString); 
            errMsgText += "\n\n";
            errMsgText += "currently only triggerType=Internal supported";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        // --------------------------------------------------------------------
        // TO DO - Check if trigger type is allowed and if so set to new value
        // --------------------------------------------------------------------

        return true;
    }


    bool CameraWindow::setLoggingFromMap(QVariantMap loggingMap)
    {
        QString errMsgTitle("Load Configuration Error (Logging)");

        // Get "Enabled" value
        // -------------------
        if (!loggingMap.contains("Enabled"))
        {
            QString errMsgText("Logging configuration \"Enabled\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!loggingMap["Enabled"].canConvert<bool>())
        {
            QString errMsgText("Logging configuration unable to convert \"Enabled\" to bool");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        logging_ = loggingMap["Enabled"].toBool();

        // Get "Format" value
        // -------------------
        if (!loggingMap.contains("Format"))
        {
            QString errMsgText("Logging configuration \"Format\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!loggingMap["Format"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " \"Format\" to string";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        QString formatString = loggingMap["Format"].toString();
        VideoFileFormat format = convertStringToVideoFileFormat(formatString);
        if (format == VIDEOFILE_FORMAT_UNSPECIFIED)
        {
            QString errMsgText = QString(
                    "Logging configuration - unknown video file format %1"
                    ).arg(formatString); 
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoFileFormat_ = format;

        // Get "Directory" value
        // ----------------------
        if (!loggingMap.contains("Directory"))
        {
            QString errMsgText( "Logging configuration \"Directory\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!loggingMap["Directory"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " \"Directory\" to string";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        QString directoryString = loggingMap["Directory"].toString();
        QDir directory = QDir(directoryString);
        if (!directory.exists())
        {
            QString errMsgText("Logging configuration \"Directory\" does not exist");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        currentVideoFileDir_ = directory;


        // Get "File Name" value
        // ---------------------
        if (!loggingMap.contains("File Name"))
        {
            QString errMsgText("Logging configuration \"File Name\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!loggingMap["File Name"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " \"File Name\" to string";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        QString fileNameString = loggingMap["File Name"].toString();
        currentVideoFileName_ = fileNameString;

        // Set the logging format settings
        // -------------------------------
        QVariantMap formatSettingsMap = loggingMap["Settings"].toMap();
        if (formatSettingsMap.isEmpty())
        { 
            QString errMsgText("Logging configuration \"Settings\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!setFormatSettingsFromMap(formatSettingsMap))
        {
            return false;
        }

        return true;
    }


    bool CameraWindow::setTimerFromMap(QVariantMap timerMap)
    {
        QString errMsgTitle("Load Configuration Error (Timer)");

        // Set "Enabled" value
        if (!timerMap.contains("Enabled"))
        {
            QString errMsgText("Timer configuration - \"Enabled\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!timerMap["Enabled"].canConvert<bool>())
        {
            QString errMsgText("Timer configuration - unable to ");
            errMsgText += " convert \"Enabled\" to bool";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        bool timerEnabled = timerMap["Enabled"].toBool();
        actionTimerEnabledPtr_ -> setChecked(timerEnabled);

        // Get Settings map
        QVariantMap settingsMap = timerMap["Settings"].toMap();
        if (settingsMap.isEmpty())
        {
            QString errMsgText("Timer configuration - \"Settings\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // Set "Duration" value
        if (!settingsMap.contains("Duration"))
        {
            QString errMsgText("Timer configuration - \"Settings\"");
            errMsgText += " \"Duration\" is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!settingsMap["Duration"].canConvert<unsigned long long>())
        {
            QString errMsgText("Timer configuration - unable to convert");
            errMsgText += " \"Settings\" \"Duration\" to unsigned long";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        captureDurationSec_ = (unsigned long)(settingsMap["Duration"].toULongLong());
        return true;
    }


    bool CameraWindow::setDisplayFromMap(QVariantMap displayMap)
    {
        QString errMsgTitle("Load Congifuration Error (Display)");

        // Get Orientation map
        QVariantMap orientMap = displayMap["Orientation"].toMap();
        if (orientMap.isEmpty())
        {
            QString errMsgText("Display configuration - \"Orientation\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // Set Orientation Flip Vertical
        if (!orientMap.contains("Flip Vertical"))
        {
            QString errMsgText("Display configuration - \"Orientation\"");
            errMsgText += " \"Flip Vertical\" is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!orientMap["Flip Vertical"].canConvert<bool>())
        {
            QString errMsgText("Display configuration - unable to convert");
            errMsgText += " \"Orientation\" \"Flip Vertical\" to bool";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        flipVert_ = orientMap["Flip Vertical"].toBool();
        std::cout << "flipVert_ " << flipVert_ << std::endl;

        // Set Orientation Flip Horizontal
        if (!orientMap.contains("Flip Horizontal"))
        {
            QString errMsgText("Display configuration - \"Orientation\"");
            errMsgText += " \"Flip Horizontal\" is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!orientMap["Flip Horizontal"].canConvert<bool>())
        {
            QString errMsgText("Display configuration - unable to convert");
            errMsgText += " \"Orientation\" \"Flip Horizontal\" to bool";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        flipHorz_ = orientMap["Flip Horizontal"].toBool();
        std::cout << "flipHorz_ " << flipHorz_ << std::endl;

        // Set Rotation
        if (!displayMap.contains("Rotation"))
        {
            QString errMsgText("Display configuration - \"Rotation\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!displayMap["Rotation"].canConvert<unsigned int>())
        {
            QString errMsgText("Display configuration - unable to convert");
            errMsgText += " \"Rotation\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int rotationUInt = displayMap["Rotation"].toUInt();
        if ( 
                (rotationUInt != IMAGE_ROTATION_0 )  && 
                (rotationUInt != IMAGE_ROTATION_90)  &&
                (rotationUInt != IMAGE_ROTATION_180) &&
                (rotationUInt != IMAGE_ROTATION_270) 
           )
        {
            QString errMsgText("Display configuration - \"Rotation\"");
            errMsgText += " must be 0, 90, 180, or 270";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        imageRotation_ = ImageRotationType(rotationUInt);

        // Set Image Display frequency
        if (!displayMap.contains("Update Frequency"))
        {
            QString errMsgText("Display configuration - \"Update Frequency\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!displayMap["Update Frequency"].canConvert<double>())
        {
            QString errMsgText("Display configuration - unable to convert");
            errMsgText += " \"Update Frequency\" to double";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        double displayFreq = displayMap["Update Frequency"].toDouble();
        if (displayFreq < MIN_IMAGE_DISPLAY_FREQ)
        {
            QString errMsgText("Display configuration - \"Update Frequency\"");
            errMsgText += QString(" must be greater than or equal to %1").arg(
                    MIN_IMAGE_DISPLAY_FREQ
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (displayFreq > MAX_IMAGE_DISPLAY_FREQ)
        {
            QString errMsgText("Display configuration - \"Update Frequency\"");
            errMsgText += QString(" must be less than or equal to %1").arg(
                    MIN_IMAGE_DISPLAY_FREQ
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        imageDisplayFreq_ = displayFreq;

        return true;
    }


    bool CameraWindow::setConfigFileFromMap(QVariantMap configFileMap)
    {
        QString errMsgTitle("Load Configuration Error (File)");

        // Set Directory
        if (!configFileMap.contains("Directory"))
        {
            QString errMsgText("Configuration file - \"Directory\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!configFileMap["Directory"].canConvert<QString>())
        {
            QString errMsgText("Configuratoin file - unable to convert");
            errMsgText += " \"Directory\" to string";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        QString directoryString = configFileMap["Directory"].toString();
        QDir directory = QDir(directoryString);
        if (!directory.exists())
        {
            QString errMsgText("Configuration file - \"Directory\"");
            errMsgText += " does not exist";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        currentConfigFileDir_ = directory;

        // Set File Name
        if (!configFileMap.contains("File Name"))
        {
            QString errMsgText("Configuration file - \"File Name\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!configFileMap["File Name"].canConvert<QString>())
        {
            QString errMsgText("Configuratoin file - unable to convert");
            errMsgText += " \"File Name\" to string";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        QString fileName = configFileMap["File Name"].toString();
        currentConfigFileName_ = fileName;

        return true;
    }


    bool CameraWindow::setCameraPropertyFromMap(QVariantMap propValueMap, PropertyInfo propInfo)
    {
        Property newProp;
        newProp.type = propInfo.type;
        QString errMsgTitle("Load Configuration Error (Camera Property)");
        QString name = QString::fromStdString(getPropertyTypeString(propInfo.type));

        // Get value for "Present"
        if (!propValueMap.contains("Present"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no value for \"Present\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["Present"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to cast \"Present\" value to bool"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.present =  propValueMap["Present"].toBool();
        if (newProp.present != propInfo.present)
        {
            QString errMsgText = QString(
                    "Camera property %1 \"Present\" value does not match that in property info"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // Get value for "Absolute Control"
        if (!propValueMap.contains("Absolute Control"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no value for \"Absolute Control\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["Absolute Control"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to convedrt \"Absolute Control\" to bool"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.absoluteControl = propValueMap["Absolute Control"].toBool();
        if (newProp.absoluteControl && !propInfo.absoluteCapable)
        {
            QString errMsgText = QString(
                    "Camera property %1 is not capable of \"Absolute Control\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // Get value for "One Push"
        if (!propValueMap.contains("One Push"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no value for \"One Push\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["One Push"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to convert \"One Push\" to bool"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.onePush = propValueMap["One Push"].toBool();
        if (newProp.onePush && !propInfo.onePushCapable)
        {
            QString errMsgText = QString(
                    "Camera property %1 is not capable of \"One Push\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // Get value for "On"
        if (!propValueMap.contains("On"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no value for \"On\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["On"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to convert \"On\" to bool"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.on = propValueMap["On"].toBool();

        // Get Value for "Auto Active"
        if (!propValueMap.contains("Auto Active"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no value for \"Auto Active\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["Auto Active"].canConvert<bool>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to convert \"Auto Active\" to bool"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.autoActive = propValueMap["Auto Active"].toBool();
        if (newProp.autoActive && !propInfo.autoCapable)
        {
            QString errMsgText = QString(
                    "Camera property %1 is not auto capable"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // Get "Value" 
        if (!propValueMap.contains("Value"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no \"Value\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["Value"].canConvert<unsigned int>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to convert \"Value\" to unsigned int"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.value = propValueMap["Value"].toUInt();
        if (!newProp.absoluteControl) 
        {
            if (newProp.value < propInfo.minValue)
            {
                QString errMsgText = QString(
                        "Value for camera property %1 is out of range (too low)"
                        ).arg(name);
                QMessageBox::critical(this,errMsgTitle,errMsgText);
                return false;
            }
            else if (newProp.value > propInfo.maxValue)
            {
                QString errMsgText = QString(
                        "Value for camera property %1 is out of range (too high)"
                        ).arg(name);
                QMessageBox::critical(this,errMsgTitle,errMsgText);
                return false;
            }
        }

        // Get "Absolute Value"
        if (!propValueMap.contains("Absolute Value"))
        {
            QString errMsgText = QString(
                    "Camera property %1 has no \"Absolute Value\""
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!propValueMap["Absolute Value"].canConvert<float>())
        {
            QString errMsgText = QString(
                    "Camera property %1 unable to convert \"Absolute Value\" to float"
                    ).arg(name);
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        newProp.absoluteValue = propValueMap["Absolute Value"].toFloat();
        if (newProp.absoluteControl)
        {
            if (newProp.absoluteValue < propInfo.minAbsoluteValue)
            {
                QString errMsgText = QString(
                        "Absolute value for camera property %1 is out of range (too low)"
                        ).arg(name);
                QMessageBox::critical(this,errMsgTitle,errMsgText);
                return false;
            }
            else if (newProp.absoluteValue > propInfo.maxAbsoluteValue)
            {
                QString errMsgText = QString(
                        "Absolute value for camera property %1 is out of range (too high)"
                        ).arg(name);
                QMessageBox::critical(this,errMsgTitle,errMsgText);
                return false;
            }
        }

        // Set value in camera
        if (propInfo.present) 
        {
            bool error = false;
            unsigned int errorId;
            QString errorMsg;

            cameraPtr_ -> acquireLock();
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

            if (error)
            {
                QString msgTitle("Set Camera Property Error");
                QString msgText = QString("Error setting camera property %1.\n\nError ID: ").arg(name);
                msgText += QString::number(errorId);
                msgText += "\n\n";
                msgText += errorMsg;
                QMessageBox::critical(this, msgTitle, msgText);
                QByteArray emptyByteArray = QByteArray();
                return false;
            }
        }
        return true;
    }


    bool CameraWindow::setFormatSettingsFromMap(QVariantMap settingsMap)
    {
        QString errMsgTitle("Load Configuration Error (Format Settings)");

        // Get avi values
        // --------------
        QVariantMap aviMap = settingsMap["avi"].toMap();
        if (aviMap.isEmpty())
        {
            QString errMsgText("Logging configuration - \"avi\" settings not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!aviMap.contains("Frame Skip"))
        {
            QString errMsgText("Logging configuration - avi \"Frame Skip\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!aviMap["Frame Skip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " \"Frame Skip\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int aviFrameSkip = aviMap["Frame Skip"].toUInt();
        if (aviFrameSkip == 0)
        {
            QString errMsgText("Logging configuration - avi \"Frame Skip\"");
            errMsgText += " must be greater than zero"; 
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.avi.frameSkip = aviFrameSkip;

        if (!aviMap.contains("Codec"))
        {
            QString errMsgText("Logging configuration - avi \"Codec\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!aviMap["Codec"].canConvert<QString>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " avi \"Codec\" to string";
            QMessageBox::critical(this, errMsgTitle,errMsgText);
            return false;
        }
        QString aviCodec = aviMap["Codec"].toString();
        if (!VideoWriter_avi::isAllowedCodec(aviCodec))
        {
            QString errMsgText = QString("Logging configuration - avi \"Codec\" %1").arg(aviCodec);
            errMsgText += " is not allowed";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.avi.codec = aviCodec;


        // Get bmp values
        // --------------
        QVariantMap bmpMap = settingsMap["bmp"].toMap();
        if (bmpMap.isEmpty())
        {
            QString errMsgText("Logging configuration - \"bmp\" settings not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!bmpMap.contains("Frame Skip"))
        {
            QString errMsgText("Logging configuration - bmp \"Frame Skip\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!bmpMap["Frame Skip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - bmp unable to convert");
            errMsgText += " \"Frame Skip\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int bmpFrameSkip = bmpMap["Frame Skip"].toUInt();
        if (bmpFrameSkip == 0)
        {
            QString errMsgText("Logging configuration - bmp \"Frame Skip\" must");
            errMsgText += " be greater than zero";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.bmp.frameSkip = bmpFrameSkip;

        // Get fmf values
        // --------------
        QVariantMap fmfMap = settingsMap["fmf"].toMap();
        if (fmfMap.isEmpty())
        {
            QString errMsgText("Logging configuration - \"fmf\" settings not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!fmfMap.contains("Frame Skip"))
        {
            QString errMsgText("Logging configuration - fmf \"Frame Skip\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!fmfMap["Frame Skip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - fmf unable to convert");
            errMsgText += " \"Frame Skip\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int fmfFrameSkip = fmfMap["Frame Skip"].toUInt();
        if (fmfFrameSkip == 0)
        {
            QString errMsgText("Logging configuration - fmf \"Frame Skip\" must");
            errMsgText += " be greater than zero";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.fmf.frameSkip = fmfFrameSkip;
        
        // Get ufmf values
        // ---------------
        QVariantMap ufmfMap = settingsMap["ufmf"].toMap();
        if (ufmfMap.isEmpty())
        {
            QString errMsgText("Logging configuration - \"ufmf\" settings not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        // ufmf Frame Skip
        if (!ufmfMap.contains("Frame Skip"))
        {
            QString errMsgText("Logging configuration - ufmf \"Frame Skip\" not present");
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfMap["Frame Skip"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - ufmf unable to convert");
            errMsgText += " \"Frame Skip\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfFrameSkip = ufmfMap["Frame Skip"].toUInt();
        if (ufmfFrameSkip == 0)
        {
            QString errMsgText("Logging configuration - ufmf \"Frame Skip\" must");
            errMsgText += " be greater than zero";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.ufmf.frameSkip = ufmfFrameSkip;

        // ufmf Background threshold
        if (!ufmfMap.contains("Background Threshold"))
        {
            QString errMsgText("Logging configuration - ufmf"); 
            errMsgText += " \"Background Threshold\" not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfMap["Background Threshold"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - ufmf unable");
            errMsgText += " to convert \"Background Threshold\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfBackgroundThreshold = ufmfMap["Background Threshold"].toUInt();
        if (ufmfBackgroundThreshold < VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD)
        {
            QString errMsgText("Logging configuration - ufmf \"Background Threshold\""); 
            errMsgText += QString(" must be greater than %1").arg(
                    VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (ufmfBackgroundThreshold > VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD)
        {
            QString errMsgText("Logging configuration - ufmf \"Background Threshold\"");
            errMsgText += QString(" msut be less than %1").arg(
                    VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.ufmf.backgroundThreshold = ufmfBackgroundThreshold;

        // ufmf Box length
        if (!ufmfMap.contains("Box Length"))
        {
            QString errMsgText("Logging configuration - ufmf"); 
            errMsgText += " \"Box Length\" not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfMap["Box Length"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - ufmf unable");
            errMsgText += " to convert \"Box Length\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfBoxLength = ufmfMap["Box Length"].toUInt();
        if (ufmfBoxLength < VideoWriter_ufmf::MIN_BOX_LENGTH)
        {
            QString errMsgText("Logging configuration - ufmf \"Box Length\""); 
            errMsgText += QString(" must be greater than %1").arg(
                    VideoWriter_ufmf::MIN_BOX_LENGTH
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (ufmfBoxLength > VideoWriter_ufmf::MAX_BOX_LENGTH)
        {
            QString errMsgText("Logging configuration - ufmf \"Box Length\"");
            errMsgText += QString(" must be less than %1").arg(
                    VideoWriter_ufmf::MAX_BOX_LENGTH
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.ufmf.boxLength = ufmfBoxLength;

        // ufmf Compression Threads
        if (!ufmfMap.contains("Compression Threads"))
        {
            QString errMsgText("Logging configuration - ufmf"); 
            errMsgText += " \"Compression Threads\" not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfMap["Compression Threads"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - ufmf unable");
            errMsgText += " to convert \"Compression Threads\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfCompressionThreads = ufmfMap["Compression Threads"].toUInt();
        if (ufmfCompressionThreads < VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS)
        {
            QString errMsgText("Logging configuration - ufmf \"Compression Threads\""); 
            errMsgText += QString(" must be greater than %1").arg(
                    VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.ufmf.numberOfCompressors = ufmfCompressionThreads;

        // ufmf median update count
        if (!ufmfMap.contains("Median Update Count"))
        {
            QString errMsgText("Logging configuration - ufmf"); 
            errMsgText += " \"Median Update Count\" not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfMap["Median Update Count"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - ufmf unable");
            errMsgText += " to convert \"Median Update Count\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfMedianUpdateCount = ufmfMap["Median Update Count"].toUInt();
        if (ufmfMedianUpdateCount < BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT )
        {
            QString errMsgText("Logging configuration - ufmf \"Median Update Count\""); 
            errMsgText += QString(" must be greater than %1").arg(
                    BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.ufmf.medianUpdateCount = ufmfMedianUpdateCount;

        // ufmf median update interval
        if (!ufmfMap.contains("Median Update Interval"))
        {
            QString errMsgText("Logging configuration - ufmf");
            errMsgText += " \"Median Update Interval\" not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfMap["Median Update Interval"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - ufmf unable");
            errMsgText += " to convert \"Median Update Interval\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfMedianUpdateInterval = ufmfMap["Median Update Interval"].toUInt();
        if (ufmfMedianUpdateInterval < BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL )
        {
            QString errMsgText("Logging configuration - ufmf \"Median Update Interval\"");
            errMsgText += QString(" must be greater than %1").arg(
                    BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL
                    );
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        videoWriterParams_.ufmf.medianUpdateInterval = ufmfMedianUpdateInterval;

        // ufmf Dilate
        QVariantMap ufmfDilateMap = ufmfMap["Dilate"].toMap();
        if (ufmfDilateMap.isEmpty())
        {
            QString errMsgText("Logging configuration - ufmf \"Dilate\"");
            errMsgText += " is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }

        // ufmf Dilate On
        if (!ufmfDilateMap.contains("On"))
        {
            QString errMsgText("Logging configuration - ufmf \"Dilate\"");
            errMsgText += " \"On\" is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfDilateMap["On"].canConvert<bool>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " ufmf \"Dilate\" \"On\" to bool";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        bool ufmfDilateOn = ufmfDilateMap["On"].toBool();
        videoWriterParams_.ufmf.dilateState = ufmfDilateOn;


        // ufmf Dilate Window Size
        if (!ufmfDilateMap.contains("Window Size"))
        {
            QString errMsgText("Logging configuration - ufmf \"Dilate\"");
            errMsgText += " \"Window Size\" is not present";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        if (!ufmfDilateMap["Window Size"].canConvert<unsigned int>())
        {
            QString errMsgText("Logging configuration - unable to convert");
            errMsgText += " ufmf \"Dilate\" \"Window Size\" to unsigned int";
            QMessageBox::critical(this,errMsgTitle,errMsgText);
            return false;
        }
        unsigned int ufmfDilateWindowSize = ufmfDilateMap["Window Size"].toUInt();
        // ---------------------------------------------------------------------
        // TO DO ... add ufmf check of ufmf Dilate window size range
        // ----------------------------------------------------------------------
        videoWriterParams_.ufmf.dilateWindowSize = ufmfDilateWindowSize;

        return true;
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
        // Week and kludgey json pretty printer.
        QByteArray jsonArrayNew;
        unsigned int pos = 0;
        unsigned int indentLevel = 0;
        unsigned int indentStep = JSON_INDENT_STEP;
        unsigned int indent = 0;

        while (pos < jsonArray.size()) 
        {
            bool isSpecialChar = false;
            if (jsonArray[pos] == '}' || jsonArray[pos] == ']')
            {
                addNewLineToByteArray(jsonArrayNew);
                indentLevel -= 1;
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
                jsonArrayNew.append(jsonArray[pos]);
                isSpecialChar = true;
            }
            else if ((jsonArray[pos] =='{') || (jsonArray[pos] == '[') )
            {
                jsonArrayNew.append(jsonArray[pos]);
                addNewLineToByteArray(jsonArrayNew);
                indentLevel += 1;
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
                isSpecialChar = true;
            }
            else if (jsonArray[pos] == ',')
            {
                jsonArrayNew.append(jsonArray[pos]);
                addNewLineToByteArray(jsonArrayNew);
                indent = indentLevel*indentStep;
                addIndentToByteArray(jsonArrayNew, indent);
                isSpecialChar = true;
            }
            else
            {
                jsonArrayNew.append(jsonArray[pos]);
            }


            if (isSpecialChar)
            {
                pos++;
                while (jsonArray[pos] == ' ')
                {
                    pos++;
                }

            }
            else
            {
                pos++;
            }
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

} // namespace bias

       

