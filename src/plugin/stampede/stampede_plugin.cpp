#include "stampede_plugin.hpp"
#include <opencv2/core/core.hpp>
#include "camera_window.hpp"
#include "json_utils.hpp"
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSerialPortInfo>

namespace bias
{

    const QString StampedePlugin::PLUGIN_NAME = QString("stampede");
    const QString StampedePlugin::PLUGIN_DISPLAY_NAME = QString("Stampede");
    const QString StampedePlugin::DEFAULT_CONFIG_FILENAME = QString("stampede_config");
    const QString StampedePlugin::CONFIG_FILE_EXTENSION = QString("json");
    const QString StampedePlugin::LOG_FILE_EXTENSION = QString("txt");
    const QString StampedePlugin::LOG_FILE_POSTFIX = QString("stampede_log");
    const QList<int> StampedePlugin::DEFAULT_VIBRATION_PIN_LIST = QList<int>({0,1});

    // Public Methods
    // ------------------------------------------------------------------------
    StampedePlugin::StampedePlugin(QWidget *parent) : BiasPlugin(parent)
    {
        setupUi(this);
        initialize();
        connectWidgets();
    }

    void StampedePlugin::reset()
    {
        resetEventStates();
    }

    void StampedePlugin::stop()
    {
        if (vibrationDev_.isOpen())
        {
            vibrationDev_.stopAll();
        }
        if (displayDev_.isOpen())
        {
        }
    }

    void StampedePlugin::setActive(bool value)
    {
        BiasPlugin::setActive(value);
        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        cameraWindowPtr -> setCaptureDuration(config_.duration());
    }

    void StampedePlugin::processFrames(QList<StampedImage> frameList)
    {
        double timeStamp;
        unsigned long frameCount;

        // -----------------------------------------------
        // Note: called by separate thread (from main gui)
        // -----------------------------------------------

        acquireLock();
        StampedImage latestFrame = frameList.back();
        frameList.clear();
        currentImage_ = latestFrame.image;
        timeStamp_ = latestFrame.timeStamp;
        frameCount_ = latestFrame.frameCount;
        //qDebug() << timeStamp_;
        releaseLock();

        processEvents();
    }

    void StampedePlugin::processEvents()
    {
        // -----------------------------------------------
        // Note: called by separate thread (from main gui)
        // -----------------------------------------------

        processVibrationEvents();
        processDisplayEvents();
    }


    void StampedePlugin::processVibrationEvents()
    {
        // -----------------------------------------------
        // Note: called by separate thread (from main gui)
        // -----------------------------------------------

        acquireLock();
        QList<VibrationEvent> vibrationEventList= config_.vibrationEventList();
        for (auto i=0; i<vibrationEventList.size(); i++) 
        {
            VibrationEvent event = vibrationEventList[i];
            double startTime = event.startTime();
            double stopTime = event.stopTime();

            if  (startTime < timeStamp_) 
            {
                switch (vibrationEventStateList_[i])
                {
                    case WAITING:
                        vibrationEventStateList_[i] = RUNNING;
                        emit startVibrationEvent(i,event);
                        break;

                    case RUNNING:
                        if (stopTime < timeStamp_)
                        {
                            vibrationEventStateList_[i] = COMPLETE;
                            emit stopVibrationEvent(i,event);
                        }
                        break;

                    default:
                        break;
                }
            }
        } // for (auto i=0 ...
        releaseLock();
    }

    void StampedePlugin::processDisplayEvents()
    {
        // -----------------------------------------------
        // Note: called by separate thread (from main gui)
        // -----------------------------------------------

        acquireLock();
        QList<DisplayEvent> displayEventList = config_.displayEventList();
        for (auto i=0; i<displayEventList.size(); i++)
        {
            DisplayEvent event = displayEventList[i];
            double startTime = event.startTime();
            double stopTime = event.stopTime();

            if  (startTime < timeStamp_) 
            {
                switch (displayEventStateList_[i])
                {
                    case WAITING:
                        displayEventStateList_[i] = RUNNING;
                        emit startDisplayEvent(i,event);
                        break;

                    case RUNNING:
                        if (stopTime < timeStamp_)
                        {
                            displayEventStateList_[i] = COMPLETE;
                            emit stopDisplayEvent(i,event);
                        }
                        break;

                    default: 
                        break;
                }
            }
        } // for (auto i=0 ...
        releaseLock();
    }




    cv::Mat StampedePlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImageCopy = currentImage_.clone();
        releaseLock();
        return currentImageCopy;
    }

    QString StampedePlugin::getName()
    {
        return PLUGIN_NAME;
    }


    QString StampedePlugin::getDisplayName()
    {
        return PLUGIN_DISPLAY_NAME;
    }

    RtnStatus StampedePlugin::runCmdFromMap(QVariantMap cmdMap,bool showErrorDlg)
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        return rtnStatus;
    }

    RtnStatus StampedePlugin::loadConfigFromFile(QString fileName, bool showErrorDlg)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        QString errMsgTitle("Load Configuration Error");
        QFile configFile(fileName);

        if (!configFile.exists())
        {
            QString errMsgText = QString("Configuration file, %1, does not exist").arg(fileName);
            if (showErrorDlg)
            {
                QMessageBox::critical(this, errMsgTitle, errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }

        bool ok = configFile.open(QIODevice::ReadOnly);
        if (!ok)
        {
            QString errMsgText = QString("Unable to open configuration file %1").arg(fileName);
            if (showErrorDlg)
            {
                QMessageBox::critical(this, errMsgTitle, errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.message = errMsgText;
            return rtnStatus;
        }
        QByteArray jsonConfig = configFile.readAll();
        configFile.close();

        acquireLock();
        RtnStatus rtnFromJson = config_.fromJson(jsonConfig);
        releaseLock();

        qDebug() << rtnFromJson.success << " " << rtnFromJson.message;
        if (!rtnFromJson.success)
        {
            QString errMsgText = QString("Unable to set configaration from file %1").arg(fileName);
            if (showErrorDlg)
            {
                QMessageBox::critical(this, errMsgTitle, errMsgText);
            }
            rtnStatus.success = false;
            rtnStatus.appendMessage(errMsgText);
            rtnStatus.appendMessage(rtnFromJson.message);
        }
        else
        {
            qDebug() << configFile.fileName();
            QFileInfo configFileInfo(configFile.fileName());
            configFileName_ = configFileInfo.baseName();
            configFileDir_ = configFileInfo.dir();

            QString labelString = QString("File: %1").arg(configFileInfo.absoluteFilePath());
            configFileNameLabelPtr -> setText(labelString);
        }
        updateConfigEditText();
        return rtnStatus;
    }

    RtnStatus StampedePlugin::connectVibrationDev() 
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (vibrationDev_.isOpen())
        {
            return rtnStatus;
        }

        bool portFound = checkForSerialPort(config_.vibrationPortName());
        if (!portFound)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("device not found");
            return rtnStatus;
        }

        vibrationDevStatusLabelPtr -> setText(QString("connecting ..."));
        vibrationDevStatusLabelPtr -> repaint();

        QSerialPortInfo  serialInfo(config_.vibrationPortName());
        vibrationDev_.setPort(serialInfo);
        vibrationDev_.open();

        if (!vibrationDev_.isOpen())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable to open device %1").arg(config_.vibrationPortName());

        }
        else
        {
            for (int i=0; i<vibrationDev_.NUM_CHANNELS; i++)
            {
                vibrationDev_.setNumPulse(i,0);
            }
        }
        updateConnectionStatusLabels();
        updateWidgetsEnabled();
        updateConnectPushButtonText();

        return rtnStatus;
    } 

    RtnStatus StampedePlugin::disconnectVibrationDev() 
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (!vibrationDev_.isOpen())
        {
            return rtnStatus;
        }

        vibrationDevStatusLabelPtr -> setText(QString("disconnecting ..."));
        vibrationDevStatusLabelPtr -> repaint();
        vibrationDev_.close();

        if (vibrationDev_.isOpen())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("error disconnecting from device");
        }

        updateConnectionStatusLabels();
        updateWidgetsEnabled();
        updateConnectPushButtonText();

        return rtnStatus;
    } 

    RtnStatus StampedePlugin::connectDisplayDev() 
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (displayDev_.isOpen())
        {
            return rtnStatus;
        }

        bool portFound = checkForSerialPort(config_.displayPortName());
        if (!portFound)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("device not found");
            return rtnStatus;
        }

        displayDevStatusLabelPtr -> setText(QString("connecting ..."));
        displayDevStatusLabelPtr -> repaint();

        QSerialPortInfo  serialInfo(config_.displayPortName());
        displayDev_.setPort(serialInfo);
        displayDev_.open();

        if (!displayDev_.isOpen())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable to open device %1").arg(config_.displayPortName());
        }
        updateConnectionStatusLabels();
        updateWidgetsEnabled();
        updateConnectPushButtonText();

        return rtnStatus;
    } 

    RtnStatus StampedePlugin::disconnectDisplayDev() 
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (!displayDev_.isOpen())
        {
            return rtnStatus;
        }

        displayDevStatusLabelPtr -> setText(QString("disconnecting ..."));
        displayDevStatusLabelPtr -> repaint();
        displayDev_.close();

        if (displayDev_.isOpen())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("error disconnecting from device");
        }

        updateConnectionStatusLabels();
        updateWidgetsEnabled();
        updateConnectPushButtonText();

        return rtnStatus;
    } 

    RtnStatus StampedePlugin::connectAll() 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (!vibrationDev_.isOpen())
        {
            RtnStatus rtnConnectVibDev = connectVibrationDev();
            if (!rtnConnectVibDev.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnConnectVibDev.message);
            }
        }
        if (!displayDev_.isOpen())
        {
            RtnStatus rtnConnectDspDev = connectDisplayDev();
            if (!rtnConnectDspDev.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnConnectDspDev.message);
            }
        }
        return rtnStatus;
    }

    RtnStatus StampedePlugin::disconnectAll() 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (vibrationDev_.isOpen())
        {
            RtnStatus rtnDisconnectVibDev = disconnectVibrationDev();
            if (!rtnDisconnectVibDev.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnDisconnectVibDev.message);
            }
        }

        if (displayDev_.isOpen())
        {
            RtnStatus rtnDisconnectDspDev = disconnectDisplayDev();
            if (!rtnDisconnectDspDev.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnDisconnectDspDev.message);
            }
        }
        return rtnStatus;
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedePlugin::initialize()
    {

        QFont monoSpaceFont("Monospace");
        monoSpaceFont.setStyleHint(QFont::TypeWriter);
        configTextEditPtr -> setFont(monoSpaceFont);

        vibrationPinList_ = DEFAULT_VIBRATION_PIN_LIST;

        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        defaultConfigFileDir_ = cameraWindowPtr -> getDefaultConfigFileDir();
        configFileDir_ = defaultConfigFileDir_;
        configFileName_ = DEFAULT_CONFIG_FILENAME;
        configFileNameLabelPtr -> setText(QString("Default Configuration"));

        setRequireTimer(true);
        acquireLock();
        config_.setToDefaultConfig();
        releaseLock();
        updateConfigEditText();
        updateWidgetsEnabled();
        updateConnectionStatusLabels();
        updateConnectPushButtonText();
        updateLogFileName();
    }

    void StampedePlugin::connectWidgets()
    {
        connect(
                vibrationDevConnectPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onVibrationDevConnectClicked())
            );

        connect(
                vibrationDevTestPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onVibrationDevTestClicked())
               );

        connect(
                displayDevConnectPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onDisplayDevConnectClicked())
               );

        connect(
                displayDevBlinkPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onDisplayDevBlinkClicked())
               );

        connect(
                devConnectAllPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onDevConnectAllClicked())
               );

        connect(
                loadConfigPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onLoadConfigClicked())
               );

        connect(
                parent(),
                SIGNAL(timerDurationChanged(unsigned long)),
                this,
                SLOT(onTimerDurationChanged(unsigned long))
               );

        connect(
                parent(), 
                SIGNAL(videoFileChanged()), 
                this, 
                SLOT(onVideoFileChanged())
                );

        qRegisterMetaType<VibrationEvent>("VibrationEvent");
        qRegisterMetaType<DisplayEvent>("DisplayEvent");

        connect(
                this, 
                SIGNAL(startVibrationEvent(int,VibrationEvent)), 
                this, 
                SLOT(onStartVibrationEvent(int,VibrationEvent))
                );

        connect(
                this, 
                SIGNAL(stopVibrationEvent(int,VibrationEvent)),  
                this, SLOT(onStopVibrationEvent(int,VibrationEvent))
                );

        connect(
                this, 
                SIGNAL(startDisplayEvent(int,DisplayEvent)),   
                this, 
                SLOT(onStartDisplayEvent(int,DisplayEvent))
                ); 

        connect(
                this, 
                SIGNAL(stopDisplayEvent(int,DisplayEvent)),   
                this, 
                SLOT(onStopDisplayEvent(int,DisplayEvent))
                );
    }


    void StampedePlugin::updateConfigEditText()
    {
        acquireLock();
        configTextEditPtr -> setText(config_.toString());
        releaseLock();
    }

    QString StampedePlugin::getConfigFileFullPath()
    {
        QString fileName = configFileName_ + "." + CONFIG_FILE_EXTENSION;
        if (!configFileDir_.exists())
        {
            configFileDir_ = defaultConfigFileDir_;
        }
        QFileInfo configFileInfo(configFileDir_, fileName);
        QString configFileFullPath = configFileInfo.absoluteFilePath();
        return configFileFullPath;

    }

    void StampedePlugin::updateWidgetsEnabled()
    {
        vibrationDevTestPushButtonPtr -> setEnabled(vibrationDev_.isOpen());
        displayDevBlinkPushButtonPtr -> setEnabled(displayDev_.isOpen());
    }

    void StampedePlugin::updateConnectionStatusLabels()
    {
        if (vibrationDev_.isOpen())
        {
            vibrationDevStatusLabelPtr -> setText("connected");
        }
        else
        {
            vibrationDevStatusLabelPtr -> setText("not connected");
        }

        if (displayDev_.isOpen())
        {
            displayDevStatusLabelPtr -> setText("connected");
        }
        else
        {
            displayDevStatusLabelPtr -> setText("not connected");
        }
    }

    void StampedePlugin::updateConnectPushButtonText()
    {
        if (vibrationDev_.isOpen())
        {
            vibrationDevConnectPushButtonPtr -> setText("Disconnect");
        }
        else
        {
            vibrationDevConnectPushButtonPtr -> setText("Connect");
        }

        if (displayDev_.isOpen())
        {
            displayDevConnectPushButtonPtr -> setText("Disconnect");
        }
        else
        {
            displayDevConnectPushButtonPtr -> setText("Connect");
        }

        if (vibrationDev_.isOpen() && displayDev_.isOpen())
        {
            devConnectAllPushButtonPtr -> setText("Disconnect All");
        }
        else
        {
            devConnectAllPushButtonPtr -> setText("Connect All");
        }
    }


    void StampedePlugin::updateLogFileName()
    {
        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        logFileDir_ = cameraWindowPtr -> getVideoFileDir();

        logFileName_ = cameraWindowPtr -> getVideoFileName() + QString("_") + LOG_FILE_POSTFIX;;
        logFileName_ += QString(".") + LOG_FILE_EXTENSION;

        logFileFullPath_ = logFileDir_.absoluteFilePath(logFileName_);
        
        logFileNameLabelPtr -> setText(logFileFullPath_);
    }


    bool StampedePlugin::checkForSerialPort(QString portName)
    {
        bool found = false;
        QList<QSerialPortInfo> serialInfoList = QSerialPortInfo::availablePorts();
        for (auto serialInfo : serialInfoList)
        {
            if (serialInfo.portName() == portName)
            {
               found = true;
            } 
        }
        return found;
    }

    void StampedePlugin::resetEventStates()
    {
        acquireLock();
        vibrationEventStateList_.clear();
        for (auto event : config_.vibrationEventList())
        {
            vibrationEventStateList_.append(WAITING);
        }

        displayEventStateList_.clear();
        for (auto event : config_.displayEventList())
        {
            displayEventStateList_.append(WAITING);
        } 
        releaseLock();
    }

    // Private slots
    // ------------------------------------------------------------------------

    void StampedePlugin::onVibrationDevConnectClicked()
    {
        RtnStatus rtnStatus;
        if (vibrationDev_.isOpen())
        {
            rtnStatus = disconnectVibrationDev();
        }
        else
        {
            rtnStatus = connectVibrationDev();
        }
        qDebug() << rtnStatus.success << " " << rtnStatus.message;
    }

    void StampedePlugin::onVibrationDevTestClicked() 
    {
        qDebug() << __PRETTY_FUNCTION__;
    }

    void StampedePlugin::onDisplayDevConnectClicked()
    {
        RtnStatus rtnStatus;

        if (displayDev_.isOpen())
        {
            rtnStatus = disconnectDisplayDev();
        }
        else
        {
            rtnStatus = connectDisplayDev();
        }
    }

    void StampedePlugin::onDisplayDevBlinkClicked()
    {
        qDebug() << __PRETTY_FUNCTION__;
    }

    void StampedePlugin::onDevConnectAllClicked()
    {
        RtnStatus rtnStatus;
        if ( vibrationDev_.isOpen() && displayDev_.isOpen())
        {
            rtnStatus = disconnectAll();
        }
        else
        {
            rtnStatus = connectAll();
        }
        
    }

    void StampedePlugin::onLoadConfigClicked()
    {
        QString configFileFullPath = getConfigFileFullPath();

        QString configFileString = QFileDialog::getOpenFileName(
                this, 
                QString("Load Configuration File"),
                configFileFullPath
                );

        if (!configFileString.isEmpty())
        {
            loadConfigFromFile(configFileString);
        }
    }

    void StampedePlugin::onTimerDurationChanged(unsigned long duration)
    {
        qDebug() << "duration = " << duration;
    }


    void StampedePlugin::onVideoFileChanged()
    {
        updateLogFileName();
    }


    void StampedePlugin::onStartVibrationEvent(int index,VibrationEvent event) 
    {
        qDebug() << "start vibration";
        qDebug() << "  index         " << index;
        qDebug() << "  startTime     " << event.startTime();
        qDebug() << "  period        " << event.period();
        qDebug() << "  number        " << event.number();


        if (vibrationDev_.isOpen())
        {
            for (auto pin : vibrationPinList_)
            {
                int periodMS = int(1000*event.period());
                vibrationDev_.setPeriod(pin,periodMS);
                vibrationDev_.setNumPulse(pin,event.number());
                vibrationDev_.startAll();
            }
        }
        else
        {
            qDebug() << "** device not connected";
        }

        qDebug();
    }


    void StampedePlugin::onStopVibrationEvent(int index,VibrationEvent event) 
    {
        qDebug() << "stop vibration";
        qDebug() << "  index         " << index;

        if (vibrationDev_.isOpen())
        {
            vibrationDev_.stopAll();
        }
        else
        {
            qDebug() << "** device not connected";
        }
        qDebug();
    }


    void StampedePlugin::onStartDisplayEvent(int index,DisplayEvent event) 
    {
        qDebug() << "start display";
        qDebug() << "  index         " << index;
        qDebug() << "  patternId     " << event.patternId();
        qDebug() << "  startTime     " << event.startTime();
        qDebug() << "  stopTime      " << event.stopTime();
        qDebug() << "  controalBias  " << event.controlBias();

        if (displayDev_.isOpen())
        {


        }
        else
        {
            qDebug() << "** device not connected";
        }
        qDebug();
    }


    void StampedePlugin::onStopDisplayEvent(int index,DisplayEvent event) 
    {
        qDebug() << "stop display";
        qDebug() << "  index         " << index;

        if (displayDev_.isOpen())
        {

        }
        else
        {
            qDebug() << "** device not connected";
        }
        qDebug();
    }

}
