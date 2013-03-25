#ifndef BIAS_BASIC_HTTP_SERVER_HPP
#define BIAS_BASIC_HTTP_SERVER_HPP

#include <QTcpServer>
#include <QPointer>
#include <QString>
#include <QMap>

class QTcpSockect;

namespace bias
{

    class BasicHttpServer : public QTcpServer
    {

        Q_OBJECT

        public:
            BasicHttpServer(QObject *parent=0);
            void incomingConnection(int socket);

        signals:
            void httpRequest(QMap<QString,QString> paramsMap);

        private slots:
            void readClient();
            void discardClient();
            void handleGetRequest(QTcpSocket *socket, QStringList &tokens);

        private:
            QPointer<QTcpServer> socketPtr_;

    };


} // namespace bias


#endif // #ifndef BIAS_BASIC_HTTP_SERVER_HPP
