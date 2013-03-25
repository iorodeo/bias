#include "basic_http_server.hpp"
#include <QTcpSocket>
#include <QTextStream>
#include <QStringList>
#include <QDateTime>
#include <iostream>

namespace bias
{

    BasicHttpServer::BasicHttpServer(QObject *parent)
        : QTcpServer(parent)
    { }

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
                socketPtr->close();
                if (socketPtr->state() == QTcpSocket::UnconnectedState) 
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
        os << "HTTP/1.0 200 Ok\r\n";
        os << "Content-Type: text/html; charset=\"utf-8\"\r\n\r\n";
        os << "<html>\n";
        os << "<body>\n";
        os << "<h1>BIAS External Control Server</h1>\n";

        if (tokens.size() < 2)
        {
            os << "no enought tokens\n";
            os << "</body>\n";
            os << "</html>\n";
            return;
        }

        QString paramsString = tokens[1];
        if (paramsString.length() == 1)
        {
            os << "Running - ";
            os << QDateTime::currentDateTime().toString() << "\n";
            os << "</body>\n";
            os << "</html>\n";
        }
        else if (paramsString.length() > 1)
        {
            QChar secondChar = paramsString[1];
            if (secondChar != QChar('?'))
            {
                os << "no ? character preceding parameters\n";
                os << "</body>\n";
                os << "</html>\n";
                return;
            }

            paramsString.remove(0,2);
            QStringList paramsList = paramsString.split("&",QString::SkipEmptyParts);

            if (!paramsList.isEmpty())
            {
                os << QString("request = %1").arg(paramsList[0]) << "\n";
                os << "</body>\n";
                os << "</html>\n";

                // TEMPORARY - need to deal with requests with values 
                QMap<QString, QString> paramsMap;
                paramsMap.insert(paramsList[0], QString(""));
                emit httpRequest(paramsMap);
                return;
            }
            else
            {
                os << QString("no argument");
                os << "</body>\n";
                os << "</html>\n";
                return;
            }
        }
    }
}
