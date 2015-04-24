#ifndef BIAS_BASIC_HTTP_SERVER_HPP
#define BIAS_BASIC_HTTP_SERVER_HPP

#include <QTcpServer>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVariantMap>

class QTcpSockect;

namespace bias
{


    class BasicHttpServer : public QTcpServer
    {

        Q_OBJECT

        public:
            BasicHttpServer(QObject *parent=0);
            virtual void incomingConnection(qintptr socket);

        protected:
            virtual void handleGetRequest(QTcpSocket *socket, QStringList &tokens);
            virtual void handleParamsRequest(QTextStream &os, QStringList &paramsList);
            virtual void sendBadRequestResp(QTextStream &os, QString msg);
            virtual void sendRunningResp(QTextStream &os);
            virtual QVariantMap paramsRequestSwitchYard(QString name, QString value);

        protected slots:
            virtual void readClient();
            virtual void discardClient();
    };

    QStringList splitRequestString(QString reqString);
    QString replaceEscapeChars(QString input);


} // namespace bias


#endif // #ifndef BIAS_BASIC_HTTP_SERVER_HPP
