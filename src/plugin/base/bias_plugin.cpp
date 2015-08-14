#include "bias_plugin.hpp"
#include <QtDebug>
#include <opencv2/core/core.hpp>
#include "camera_window.hpp"

namespace bias
{

    const QString BiasPlugin::PLUGIN_NAME = QString("basePlugin"); 
    const QString BiasPlugin::PLUGIN_DISPLAY_NAME = QString("Base Plugin"); 
    const QString BiasPlugin::LOG_FILE_EXTENSION = QString("txt");
    const QString BiasPlugin::LOG_FILE_POSTFIX = QString("plugin_log");

    // Pulbic
    // ------------------------------------------------------------------------

    BiasPlugin::BiasPlugin(QWidget *parent) : QDialog(parent) 
    { 
        active_ = false;
        setRequireTimer(false);
    }

    void BiasPlugin::reset()
    { }

    void BiasPlugin::setFileAutoNamingString(QString autoNamingString)
    {
        fileAutoNamingString_ = autoNamingString;
    }

    void BiasPlugin::setFileVersionNumber(unsigned verNum)
    {
        fileVersionNumber_ = verNum;
    }

    void BiasPlugin::stop()
    { }

    void BiasPlugin::setActive(bool value)
    {
        active_ = value;
    }


    bool BiasPlugin::isActive()
    {
        return active_;
    }


    bool BiasPlugin::requireTimer()
    {
        return requireTimer_;
    }

    void BiasPlugin::processFrames(QList<StampedImage> frameList) 
    { 
        acquireLock();
        StampedImage latestFrame = frameList.back();
        frameList.clear();
        currentImage_ = latestFrame.image;
        timeStamp_ = latestFrame.timeStamp;
        frameCount_ = latestFrame.frameCount;
        releaseLock();
    } 


    cv::Mat BiasPlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImageCopy = currentImage_.clone();
        releaseLock();
        return currentImageCopy;
    }


    QString BiasPlugin::getName()
    {
        return PLUGIN_NAME;
    }


    QString BiasPlugin::getDisplayName()
    {
        return PLUGIN_DISPLAY_NAME;
    }


    QPointer<CameraWindow> BiasPlugin::getCameraWindow()
    {
        QPointer<CameraWindow> cameraWindowPtr = (CameraWindow*)(parent());
        return cameraWindowPtr;
    }


    RtnStatus BiasPlugin::runCmdFromMap(QVariantMap cmdMap, bool showErrorDlg)
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        return rtnStatus;
    }

    QVariantMap BiasPlugin::getConfigAsMap()  
    {
        QVariantMap configMap;
        return configMap;
    }

    RtnStatus BiasPlugin::setConfigFromMap(QVariantMap configMap)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }

    RtnStatus BiasPlugin::setConfigFromJson(QByteArray jsonArray)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }

    bool BiasPlugin::pluginsEnabled()
    {
        return getCameraWindow() -> isPluginEnabled();
    }


    void BiasPlugin::setPluginsEnabled(bool value)
    {
        getCameraWindow() -> setPluginEnabled(value);
    }


    QString BiasPlugin::getLogFileExtension()
    {
        return LOG_FILE_EXTENSION;
    }

    QString BiasPlugin::getLogFilePostfix()
    {
        return LOG_FILE_POSTFIX;
    }

    QString BiasPlugin::getLogFileName(bool includeAutoNaming)
    {
        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        QString logFileName = cameraWindowPtr -> getVideoFileName() + QString("_") + getLogFilePostfix();
        if (includeAutoNaming)
        {
            if (!fileAutoNamingString_.isEmpty())
            {
                logFileName += QString("_") + fileAutoNamingString_;
            }
            if (fileVersionNumber_ != 0)
            {
                QString verStr = QString("_v%1").arg(fileVersionNumber_,3,10,QChar('0'));
                logFileName += verStr;
            }
        }
        logFileName += QString(".") + getLogFileExtension();
        return logFileName;
    }


    QString BiasPlugin::getLogFileFullPath(bool includeAutoNaming)
    {
        QString logFileName = getLogFileName(includeAutoNaming);
        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        logFileDir_ = cameraWindowPtr -> getVideoFileDir();
        QString logFileFullPath = logFileDir_.absoluteFilePath(logFileName);
        return logFileFullPath;
    }

    // Protected methods
    // ------------------------------------------------------------------------

    void BiasPlugin::setRequireTimer(bool value)
    {
        requireTimer_ = value;
    }


    void BiasPlugin::openLogFile()
    {
        loggingEnabled_ = getCameraWindow() -> isLoggingEnabled();
        if (loggingEnabled_)
        {
            QString logFileFullPath = getLogFileFullPath(true);
            qDebug() << logFileFullPath;
            logFile_.setFileName(logFileFullPath);
            bool isOpen = logFile_.open(QIODevice::WriteOnly | QIODevice::Text);
            if (isOpen)
            {
                logStream_.setDevice(&logFile_);
            }
        }
    }


    void BiasPlugin::closeLogFile()
    {
        if (loggingEnabled_ && logFile_.isOpen())
        {
            logStream_.flush();
            logFile_.close();
        }
    }

}
