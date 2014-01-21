#ifndef EXT_CTL_HTTP_SERVER_HPP
#define Ext_CTL_HTTP_SERVER_HPP
#include<QVariantMap>
#include "basic_http_server.hpp"

class FlySorterWindow;

class ExtCtlHttpServer : public bias::BasicHttpServer
{
    Q_OBJECT

    public:
        ExtCtlHttpServer(FlySorterWindow *flySorterWindow, QObject *parent=0);

    protected:
        virtual QVariantMap paramsRequestSwitchYard(QString name, QString value);

    protected slots:
        virtual void readClient();

    private:
        bool closeFlag_;
        QPointer<FlySorterWindow> flySorterWindowPtr_;
        QVariantMap handleStartReq();
        QVariantMap handleStopReq();
        QVariantMap handleGetStatusReq();
        QVariantMap handleCloseReq();
};

#endif
