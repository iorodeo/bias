#include "basic_http_server.hpp"
#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
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
            QStringList tokens = requestString.split(QRegExp("[ \r\n][ \r\n]*"));
            if (tokens[0] == "GET") 
            {
                handleGetRequest(socketPtr, tokens);
                socketPtr -> close();
                if (socketPtr -> state() == QTcpSocket::UnconnectedState)
                {
                    delete socketPtr;
                }
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
        QVariantMap respMap;
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
            respMap.insert(name,cmdMap);
        }

        // Send response
        bool ok;
        QByteArray jsonResp = QtJson::serialize(respMap,ok);
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
        else if (name == QString("set-config"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
        }
        else if (name == QString("enable-logging"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
        }
        else if (name == QString("disable-logging"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
        }
        else if (name == QString("set-config-file"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
        }
        else if (name == QString("get-frame-count"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
        }
        else if (name == QString("get-camera-guid"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
        }
        else if (name == QString("get-status"))
        {
            cmdMap.insert("success", true);
            cmdMap.insert("message", "");
            cmdMap.insert("value", "");
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
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleConnectRequest()
    { 
        QVariantMap cmdMap;
        cameraWindowPtr_ -> connectCamera();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleDisconnectRequest()
    { 
        QVariantMap cmdMap;
        cameraWindowPtr_ -> disconnectCamera();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleStartCaptureRequest()
    {
        QVariantMap cmdMap;
        cameraWindowPtr_ -> startImageCapture();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", "");
        return cmdMap;
    }


    QVariantMap BasicHttpServer::handleStopCaptureRequest()
    {
        QVariantMap cmdMap;
        cameraWindowPtr_ -> stopImageCapture();
        cmdMap.insert("success", true);
        cmdMap.insert("message", "");
        cmdMap.insert("value", "");
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
}
