#ifndef BIAS_BASIC_HTTP_SERVER_HPP
#define BIAS_BASIC_HTTP_SERVER_HPP

#include <QTcpServer>
#include <QPointer>
#include <QString>
#include <QTextStream>
#include <QMap>
#include <QVariantMap>

class QTcpSockect;

namespace bias
{

    class CameraWindow;

    class BasicHttpServer : public QTcpServer
    {

        Q_OBJECT

        public:
            BasicHttpServer(CameraWindow *cameraWindow, QObject *parent=0);
            void incomingConnection(int socket);

        signals:
            void httpRequest(QMap<QString,QString> paramsMap);

        private slots:
            void readClient();
            void discardClient();

        private:
            QPointer<CameraWindow> cameraWindowPtr_;
            void handleGetRequest(QTcpSocket *socket, QStringList &tokens);
            void handleParamsRequest(QTextStream &os, QStringList &paramsList);
            QVariantMap paramsRequestSwitchYard(QString name, QString value);

            QVariantMap handleConnectRequest();
            QVariantMap handleDisconnectRequest();
            QVariantMap handleStartCaptureRequest();
            QVariantMap handleStopCaptureRequest();


            void sendBadRequestResp(QTextStream &os, QString msg);
            void sendRunningResp(QTextStream &os);

    };


} // namespace bias


#endif // #ifndef BIAS_BASIC_HTTP_SERVER_HPP
