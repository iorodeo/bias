#include "ext_ctl_http_server.hpp"
#include "camera_window.hpp"
#include <QtDebug>

namespace bias
{
    ExtCtlHttpServer::ExtCtlHttpServer(CameraWindow *cameraWindow, QObject *parent)
        : BasicHttpServer(parent)
    { 
        closeFlag_ = false;
        cameraWindowPtr_ = QPointer<CameraWindow>(cameraWindow);
    }

    // Protected slots
    // ------------------------------------------------------------------------------
    void ExtCtlHttpServer::readClient()
    {
        BasicHttpServer::readClient();
        if (closeFlag_)
        {
            cameraWindowPtr_ -> close();
        }
    }

    // Protected methods
    // ------------------------------------------------------------------------------
    QVariantMap ExtCtlHttpServer::paramsRequestSwitchYard(QString name, QString value)
    {
        QVariantMap cmdMap;

        if (name == QString("connect"))
        {
            cmdMap = handleConnectRequest();
        }
        else if (name == QString("disconnect"))
        {
            cmdMap = handleDisconnectRequest();
        }
        else if (name == QString("start-capture"))
        {
            cmdMap = handleStartCaptureRequest();
        }
        else if (name == QString("stop-capture"))
        {
            cmdMap = handleStopCaptureRequest();
        }
        else if (name == QString("get-configuration"))
        {
            cmdMap = handleGetConfiguration();
        }
        else if (name == QString("set-configuration"))
        {
            cmdMap = handleSetConfiguration(value);
        }
        else if (name == QString("enable-logging"))
        {
            cmdMap = handleLoggingEnable();
        }
        else if (name == QString("disable-logging"))
        {
            cmdMap = handleLoggingDisable();
        }
        else if (name == QString("load-configuration"))
        {
            cmdMap = handleLoadConfiguration(value);
        }
        else if (name == QString("save-configuration"))
        {
            cmdMap = handleSaveConfiguration(value);
        }
        else if (name == QString("get-frame-count"))
        {
            cmdMap = handleGetFrameCount();
        }
        else if (name == QString("get-camera-guid"))
        {
            cmdMap = handleGetCameraGuid();
        }
        else if (name == QString("get-status"))
        {
            cmdMap = handleGetStatus();
        }
        else if (name == QString("set-video-file"))
        {
            cmdMap = handleSetVideoFile(value);
        }
        else if (name == QString("get-video-file"))
        {
            cmdMap = handleGetVideoFile();
        }
        else if (name == QString("get-time-stamp"))
        {
            cmdMap = handleGetTimeStamp();
        }
        else if (name == QString("get-frames-per-sec"))
        {
            cmdMap = handleGetFramesPerSec();
        }
        else if (name == QString("set-camera-name"))
        {
            cmdMap = handleSetCameraName(value);
        }
        else if (name == QString("set-window-geometry"))
        {
            cmdMap = handleSetWindowGeometry(value);
        }
        else if (name == QString("get-window-geometry"))
        {
            cmdMap = handleGetWindowGeometry();
        }
        else if (name == QString("close"))
        {
            cmdMap = handleClose();
        }
        else if (name == QString("plugin-cmd"))
        {
            cmdMap = handlePluginCmd(value);
        }
        else 
        {
            cmdMap.insert("success", false);
            cmdMap.insert("message", "unknown command");
            cmdMap.insert("value", "");
        }
        cmdMap.insert("command", name);
        return cmdMap;
    }


    // Private Methods
    // ------------------------------------------------------------------------
    QVariantMap ExtCtlHttpServer::handleConnectRequest()
    { 
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> connectCamera(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleDisconnectRequest()
    { 
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> disconnectCamera(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleStartCaptureRequest()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> startImageCapture(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleStopCaptureRequest()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> stopImageCapture(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }

    
    QVariantMap ExtCtlHttpServer::handleGetConfiguration()
    {
        QVariantMap cmdMap;
        RtnStatus status;
        QVariantMap configMap = cameraWindowPtr_ -> getConfigurationMap(status,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", configMap);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleSetConfiguration(QString jsonConfig)
    {
        QVariantMap cmdMap;
        QByteArray jsonArray = jsonConfig.toLatin1();
        RtnStatus status = cameraWindowPtr_ -> setConfigurationFromJson(jsonArray,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleLoggingEnable()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> enableLogging(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleLoggingDisable()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> disableLogging(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleSaveConfiguration(QString fileName)
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> saveConfiguration(fileName,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleLoadConfiguration(QString fileName)
    {
        QVariantMap cmdMap;

        RtnStatus status = cameraWindowPtr_ -> loadConfiguration(fileName,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }

    
    QVariantMap ExtCtlHttpServer::handleGetFrameCount()
    {
        QVariantMap cmdMap;
        unsigned long frameCount = cameraWindowPtr_ -> getFrameCount();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", qulonglong(frameCount));
        return cmdMap;
    }

    QVariantMap ExtCtlHttpServer::handleGetCameraGuid()
    {
        QVariantMap cmdMap;
        QString guid = cameraWindowPtr_ -> getCameraGuidString();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", guid);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleGetStatus()
    {
        QVariantMap cmdMap;
        QVariantMap statusMap;
        bool connected = cameraWindowPtr_ -> isConnected();
        bool capturing = cameraWindowPtr_ -> isCapturing();
        bool logging = cameraWindowPtr_ -> isLoggingEnabled();
        unsigned long frameCount = cameraWindowPtr_ -> getFrameCount();
        double framesPerSec = cameraWindowPtr_ -> getFramesPerSec();
        double timeStamp = cameraWindowPtr_ -> getTimeStamp();
        statusMap.insert("connected", connected);
        statusMap.insert("capturing", capturing);
        statusMap.insert("logging", logging);
        statusMap.insert("frameCount", qulonglong(frameCount));
        statusMap.insert("framesPerSec", framesPerSec);
        statusMap.insert("timeStamp", timeStamp);
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", statusMap);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleSetVideoFile(QString fileName)
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> setVideoFile(fileName);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleGetVideoFile()
    {
        QVariantMap cmdMap;
        QString fileName = cameraWindowPtr_ -> getVideoFileFullPath();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", fileName);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleGetTimeStamp()
    {
        QVariantMap cmdMap;
        double timeStamp = cameraWindowPtr_ -> getTimeStamp();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", timeStamp);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleGetFramesPerSec()
    {
        QVariantMap cmdMap;
        double framesPerSec = cameraWindowPtr_ -> getFramesPerSec();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", framesPerSec);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleSetCameraName(QString cameraName)
    {
        QVariantMap cmdMap;
        cameraWindowPtr_ -> setUserCameraName(cameraName);
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleSetWindowGeometry(QString jsonGeom)
    {
        QVariantMap cmdMap;
        QByteArray jsonGeomArray = jsonGeom.toLatin1();
        RtnStatus status = cameraWindowPtr_ -> setWindowGeometryFromJson(jsonGeomArray);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleGetWindowGeometry()
    {
        QVariantMap cmdMap;
        QVariantMap windowGeomMap = cameraWindowPtr_ -> getWindowGeometryMap();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", windowGeomMap);
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handlePluginCmd(QString jsonPluginCmd)
    {
        qDebug() << __PRETTY_FUNCTION__ << jsonPluginCmd;

        QVariantMap cmdMap;
        QByteArray jsonPluginCmdArray = jsonPluginCmd.toLatin1();
        RtnStatus status = cameraWindowPtr_ -> runPluginCmd(jsonPluginCmdArray,false);

        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap ExtCtlHttpServer::handleClose()
    {
        QVariantMap cmdMap;
        if (cameraWindowPtr_ -> isCapturing())
        {
            cmdMap.insert("success", false);
            cmdMap.insert("message", "unable to close window, capturing images");
            cmdMap.insert("value", "");
        }
        else
        {
            closeFlag_ = true; // Delay closing window until after we send the response
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value","");
        }
        return cmdMap;
    }
}
