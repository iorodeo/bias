#include "basic_http_server.hpp"
#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QVariantList>
#include <iostream>
#include "camera_window.hpp"
#include "json.hpp"

namespace bias
{

    BasicHttpServer::BasicHttpServer(CameraWindow *cameraWindow, QObject *parent)
        : QTcpServer(parent)
    { 
        cameraWindowPtr_ = QPointer<CameraWindow>(cameraWindow);
    }

    void BasicHttpServer::incomingConnection(int socket) 
    { 
        QTcpSocket* s = new QTcpSocket(this); 
        connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
        connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
        s->setSocketDescriptor(socket);
    }


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
    }


    void BasicHttpServer::discardClient()
    {
        QTcpSocket* socketPtr = (QTcpSocket*)sender(); 
        socketPtr->deleteLater();
    }


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
        else if (name == QString("close"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
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
        cmdMap.insert("message", QString("Frame count retrieved successfully"));
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
        statusMap.insert("connected", connected);
        statusMap.insert("capturing", capturing);
        statusMap.insert("logging", logging);
        statusMap.insert("frameCount", qulonglong(frameCount));
        cmdMap.insert("success", true);
        cmdMap.insert("message", QString("Status retrieved successfully"));
        cmdMap.insert("value", statusMap);
        return cmdMap;
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

} // namespace bias
