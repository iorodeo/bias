#include "basic_http_server.hpp"
#include <QMap>
#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QVariantList>
#include <iostream>
//#include "camera_window.hpp"
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
    BasicHttpServer::BasicHttpServer(QObject *parent)
        : QTcpServer(parent)
    { 
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


    QVariantMap BasicHttpServer::paramsRequestSwitchYard(QString name, QString value)
    {
        QVariantMap cmdMap;
        cmdMap.insert("command", name);
        cmdMap.insert("success", true);
        cmdMap.insert("message", "test response");
        cmdMap.insert("value", "hello world");
        return cmdMap;
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
    }


    void BasicHttpServer::discardClient()
    {
        QTcpSocket* socketPtr = (QTcpSocket*)sender(); 
        socketPtr->deleteLater();
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
