#include "basic_http_server.hpp"
#include <QMap>
#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QVariantList>
#include <iostream>
#include "camera_window.hpp"
#include "json.hpp"

namespace bias
{
    // Constants
    // ------------------------------------------------------------------------
    static QMap<QString,QString> createEscapeToCharMap()
    {
        QMap<QString,QString> map;
        map[QString("%20")] = QString(" ");
        map[QString("%24")] = QString("$");
        map[QString("%26")] = QString("&");
        map[QString("%60")] = QString("`");
        map[QString("%3A")] = QString(":");
        map[QString("%3C")] = QString("<");
        map[QString("%3E")] = QString(">");
        map[QString("%5B")] = QString("[");
        map[QString("%5D")] = QString("]");
        map[QString("%7B")] = QString("{");
        map[QString("%7D")] = QString("}");
        map[QString("%22")] = QString("\"");
        map[QString("%23")] = QString("#");
        map[QString("%25")] = QString("%");
        map[QString("%40")] = QString("@");
        map[QString("%2F")] = QString("/");
        map[QString("%3B")] = QString(";");
        map[QString("%3D")] = QString("=");
        map[QString("%3F")] = QString("?");
        map[QString("%5C")] = QString("\\"); 
        map[QString("%5E")] = QString("^");
        map[QString("%7C")] = QString("|");
        map[QString("%7E")] = QString("~"); 
        map[QString("%27")] = QString("'");
        map[QString("%5C")] = QString("\\"); 
        map[QString("%2C")] = QString(",");
        return map;
    }
    QMap<QString,QString> ESCAPE_TO_CHAR_MAP = createEscapeToCharMap();


    // Methods - public
    // -------------------------------------------------------------------------
    BasicHttpServer::BasicHttpServer(CameraWindow *cameraWindow, QObject *parent)
        : QTcpServer(parent)
    { 
        cameraWindowPtr_ = QPointer<CameraWindow>(cameraWindow);
        closeFlag_ = false;
    }


    void BasicHttpServer::incomingConnection(int socket) 
    { 
        QTcpSocket* s = new QTcpSocket(this); 
        connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
        connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
        s->setSocketDescriptor(socket);
    }


    // Protected methods
    // ------------------------------------------------------------------------
    void BasicHttpServer::handleGetRequest(QTcpSocket *socketPtr, QStringList &tokens)
    { 
        QTextStream os(socketPtr);
        os.setAutoDetectUnicode(true);

        // Examine tokens
        if (tokens.size() < 2)
        {
            sendBadRequestResp(os,"not enought tokens");
            return;
        }

        // Parse tokens
        QString paramsString = tokens[1];
        paramsString = replaceEscapeChars(paramsString);

        if (paramsString.length() == 1)
        {
            sendRunningResp(os);
            return;
        }
        else if (paramsString.length() > 1)
        {
            // Check for request parameters character '?'
            QChar secondChar = paramsString[1];
            if (secondChar != QChar('?'))
            {
                sendBadRequestResp(os, "no ? character preceeding parameters");
                return;
            }

            paramsString.remove(0,2);
            QStringList paramsList = paramsString.split("&",QString::SkipEmptyParts);
            if (!paramsList.isEmpty())
            {
                // We have some parameters - send appropriate response
                handleParamsRequest(os, paramsList);
                return;
            }
            else
            {
                // No parameters follow '?' character
                sendBadRequestResp(os,"not parameters following ? char");
                return;
            }
        }
    }
    

    void BasicHttpServer::handleParamsRequest(QTextStream &os, QStringList &paramsList)
    { 
        os << "HTTP/1.0 200 Ok\r\n";
        os << "Content-Type: application/json; charset=\"utf-8\"\r\n\r\n";

        // Handle requests
        QVariantList respList;
        QVariantMap cmdMap;
        for (unsigned int i=0; i<paramsList.size(); i++)
        {
            QString name;
            QString value;

            QStringList parts = paramsList[i].split("=",QString::SkipEmptyParts);
            if (parts.size() == 0)
            {
                // Nothing here - just skip it
                continue;
            }
            name = parts[0];

            if (parts.size() == 1)
            {
                // Just command name 
                cmdMap = paramsRequestSwitchYard(name, QString(""));
            }
            else if (parts.size() == 2)
            {
                // Command name + parameters
                cmdMap = paramsRequestSwitchYard(name,parts[1]);
            }
            else
            {
                // Error unable to parse command
                cmdMap = QVariantMap();
                cmdMap.insert("success", false);
                cmdMap.insert("message", "unable to parse command");
            }
            //respMap.insert(name,cmdMap);
            respList.append(cmdMap);
        }

        // Send response
        bool ok;
        //QByteArray jsonResp = QtJson::serialize(respMap,ok);
        QByteArray jsonResp = QtJson::serialize(respList,ok);
        os << QString(jsonResp) << "\n";
    }


    void BasicHttpServer::sendBadRequestResp(QTextStream &os, QString msg)
    { 
        os << "HTTP/1.0 400 Bad Request\r\n";
        os << "Content-Type: text/html; charset=\"utf-8\"\r\n\r\n";
        os << "<html>\n";
        os << "<body>\n";
        os << "<h1>BIAS External Control Server</h1>\n";
        os << "Bad request: " << msg << "\n";
        os << "</body>\n";
        os << "</html>\n";
    }


    void BasicHttpServer::sendRunningResp(QTextStream &os)
    { 
        os << "HTTP/1.0 200 Ok\r\n";
        os << "Content-Type: text/html; charset=\"utf-8\"\r\n\r\n";
        os << "<html>\n";
        os << "<body>\n";
        os << "<h1>BIAS Server Running</h1>\n";
        os << QDateTime::currentDateTime().toString() << "\n";
        os << "</body>\n";
        os << "</html>\n";
    }


    // Protected slots
    // ------------------------------------------------------------------------
    void BasicHttpServer::readClient()
    {
        QTcpSocket* socketPtr = (QTcpSocket*) sender();
        if (socketPtr->canReadLine()) 
        {
            QString requestString = QString(socketPtr->readLine());
            QStringList tokens = splitRequestString(requestString);
            if (!tokens.isEmpty()) 
            {
                if (tokens[0] == "GET") 
                {
                    handleGetRequest(socketPtr, tokens);
                } 
            }
            socketPtr -> close();
            if (socketPtr -> state() == QTcpSocket::UnconnectedState)
            {
                delete socketPtr;
            }
        } 
        if (closeFlag_)
        {
            cameraWindowPtr_ -> close();
        }
    }


    void BasicHttpServer::discardClient()
    {
        QTcpSocket* socketPtr = (QTcpSocket*)sender(); 
        socketPtr->deleteLater();
    }


    // Private methods
    // ------------------------------------------------------------------------
    QVariantMap BasicHttpServer::paramsRequestSwitchYard(QString name, QString value)
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
        else 
        {
            cmdMap.insert("success", false);
            cmdMap.insert("message", "unknown command");
            cmdMap.insert("value", "");
        }
        cmdMap.insert("command", name);
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleConnectRequest()
    { 
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> connectCamera(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleDisconnectRequest()
    { 
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> disconnectCamera(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleStartCaptureRequest()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> startImageCapture(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleStopCaptureRequest()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> stopImageCapture(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }

    
    QVariantMap BasicHttpServer::handleGetConfiguration()
    {
        QVariantMap cmdMap;
        RtnStatus status;
        QVariantMap configMap = cameraWindowPtr_ -> getConfigurationMap(status,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", configMap);
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleSetConfiguration(QString jsonConfig)
    {
        QVariantMap cmdMap;
        QByteArray jsonArray = jsonConfig.toLatin1();
        RtnStatus status = cameraWindowPtr_ -> setConfigurationFromJson(jsonArray);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleLoggingEnable()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> enableLogging(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleLoggingDisable()
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> disableLogging(false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleSaveConfiguration(QString fileName)
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> saveConfiguration(fileName,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleLoadConfiguration(QString fileName)
    {
        QVariantMap cmdMap;

        RtnStatus status = cameraWindowPtr_ -> loadConfiguration(fileName,false);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }

    
    QVariantMap BasicHttpServer::handleGetFrameCount()
    {
        QVariantMap cmdMap;
        unsigned long frameCount = cameraWindowPtr_ -> getFrameCount();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", qulonglong(frameCount));
        return cmdMap;
    }

    QVariantMap BasicHttpServer::handleGetCameraGuid()
    {
        QVariantMap cmdMap;
        RtnStatus status; 
        QString guid = cameraWindowPtr_ -> getCameraGuidString(status);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        if (status.success)
        {
            cmdMap.insert("value", guid);
        }
        else
        {
            cmdMap.insert("value", "");
        }
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleGetStatus()
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


    QVariantMap BasicHttpServer::handleSetVideoFile(QString fileName)
    {
        QVariantMap cmdMap;
        RtnStatus status = cameraWindowPtr_ -> setVideoFile(fileName);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleGetVideoFile()
    {
        QVariantMap cmdMap;
        QString fileName = cameraWindowPtr_ -> getVideoFileFullPath();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", fileName);
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleGetTimeStamp()
    {
        QVariantMap cmdMap;
        double timeStamp = cameraWindowPtr_ -> getTimeStamp();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", timeStamp);
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleGetFramesPerSec()
    {
        QVariantMap cmdMap;
        double framesPerSec = cameraWindowPtr_ -> getFramesPerSec();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", framesPerSec);
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleSetCameraName(QString cameraName)
    {
        QVariantMap cmdMap;
        cameraWindowPtr_ -> setUserCameraName(cameraName);
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleSetWindowGeometry(QString jsonGeom)
    {
        QVariantMap cmdMap;
        QByteArray jsonGeomArray = jsonGeom.toLatin1();
        RtnStatus status = cameraWindowPtr_ -> setWindowGeometryFromJson(jsonGeomArray);
        cmdMap.insert("success", status.success);
        cmdMap.insert("message", status.message);
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleGetWindowGeometry()
    {
        QVariantMap cmdMap;
        QVariantMap windowGeomMap = cameraWindowPtr_ -> getWindowGeometryMap();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", windowGeomMap);
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleClose()
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


    // Utility functions
    // ------------------------------------------------------------------------
    QStringList splitRequestString(QString reqString)
    {
        QStringList reqList;
        if (reqString.isEmpty())
        {
            return reqList;
        }
        int len = reqString.length();
        int n0 = reqString.indexOf(' ');
        int n1 = reqString.lastIndexOf(' ');
        if ((n0 == -1) || (n1 == -1) || (n0==n1))
        {
            return reqList;
        }
        QString token0 = reqString.left(n0); 
        QString token1 = reqString.mid(n0+1,n1-n0-1).trimmed();
        QString token2 = reqString.right(len-n1-1);
        reqList.append(token0);
        reqList.append(token1);
        reqList.append(token2);
        return reqList;
    }

    QString replaceEscapeChars(QString input)
    {
        QString output(input);
        QMap<QString,QString>::iterator it;
        for (it=ESCAPE_TO_CHAR_MAP.begin(); it!=ESCAPE_TO_CHAR_MAP.end(); it++)
        {
            output.replace(it.key(), it.value());
        }
        return output;
    }

} // namespace bias
