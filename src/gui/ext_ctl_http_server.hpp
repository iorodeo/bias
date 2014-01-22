#ifndef EXT_CTL_HTTP_SERVER_HPP
#define EXT_CTL_HTTP_SERVER_HPP
#include<QVariantMap>
#include "basic_http_server.hpp"

namespace bias
{
    class CameraWindow;

    class ExtCtlHttpServer : public BasicHttpServer
    {
        Q_OBJECT

        public:
            ExtCtlHttpServer(CameraWindow *cameraWindow, QObject *parent=0);

        protected:
            virtual QVariantMap paramsRequestSwitchYard(QString name, QString value);

        protected slots:
            virtual void readClient();

        private:
            bool closeFlag_;
            QPointer<CameraWindow> cameraWindowPtr_;
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
            QVariantMap handleSetCameraName(QString cameaName);
            QVariantMap handleSetWindowGeometry(QString jsonGeom);
            QVariantMap handleGetWindowGeometry();
            QVariantMap handleClose();
    };

}
#endif
