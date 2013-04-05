#ifndef BIAS_BASIC_HTTP_SERVER_HPP
#define BIAS_BASIC_HTTP_SERVER_HPP

#include <QTcpServer>
#include <QPointer>
#include <QString>
#include <QStringList>
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
            QVariantMap handleGetConfiguration();
            QVariantMap handleSetConfiguration(QString jsonConfig);
            QVariantMap handleLoggingEnable();
            QVariantMap handleLoggingDisable();
            QVariantMap handleSaveConfiguration(QString fileName);
            QVariantMap handleLoadConfiguration(QString fileName);
            QVariantMap handleGetCameraGuid();
            QVariantMap handleGetFrameCount();
            QVariantMap handleGetStatus();
            QVariantMap handleSetVideoFile(QString fileName);
            QVariantMap handleGetVideoFile();
            QVariantMap handleGetTimeStamp();
            QVariantMap handleGetFramesPerSec();

            void sendBadRequestResp(QTextStream &os, QString msg);
            void sendRunningResp(QTextStream &os);

    };

    QStringList splitRequestString(QString reqString);


} // namespace bias


#endif // #ifndef BIAS_BASIC_HTTP_SERVER_HPP
