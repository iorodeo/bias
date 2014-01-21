#include "ext_ctl_http_server.hpp"
#include "fly_sorter_window.hpp"

ExtCtlHttpServer::ExtCtlHttpServer(FlySorterWindow *flySorterWindow, QObject *parent)
    : BasicHttpServer(parent)
{ 
    closeFlag_ = false;
    flySorterWindowPtr_ = QPointer<FlySorterWindow>(flySorterWindow);
}


// Protected slots
// ------------------------------------------------------------------------------
void ExtCtlHttpServer::readClient()
{
    BasicHttpServer::readClient();
    if (closeFlag_)
    {
        flySorterWindowPtr_ -> close();
    }
}


// Protected methods
// ------------------------------------------------------------------------------
QVariantMap ExtCtlHttpServer::paramsRequestSwitchYard(QString name, QString value)
{
    QVariantMap cmdMap;

    if (name == QString("start"))
    {
        cmdMap = handleStartReq();
    }
    else if (name == QString("stop"))
    {
        cmdMap = handleStopReq();
    }
    else if (name == QString("get-status"))
    {
        cmdMap = handleGetStatusReq();
    }
    else if (name == QString("close"))
    {
        cmdMap = handleCloseReq();
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


// Private Methods
// ------------------------------------------------------------------------
QVariantMap ExtCtlHttpServer::handleStartReq()
{ 
    QVariantMap cmdMap;
    RtnStatus status = flySorterWindowPtr_ -> startRunning();
    cmdMap.insert("success", status.success);
    cmdMap.insert("message", status.message);
    cmdMap.insert("value", "");
    return cmdMap;
}


QVariantMap ExtCtlHttpServer::handleStopReq()
{ 
    QVariantMap cmdMap;
    RtnStatus status = flySorterWindowPtr_ -> stopRunning();
    cmdMap.insert("success", status.success);
    cmdMap.insert("message", status.message);
    cmdMap.insert("value", "");
    return cmdMap;
}


QVariantMap ExtCtlHttpServer::handleGetStatusReq()
{
    QVariantMap cmdMap;
    //RtnStatus status = flySorterWindowPtr_ -> getStatus();;
    //cmdMap.insert("success", status.success);
    //cmdMap.insert("message", status.message);
    //cmdMap.insert("value", "");
    return cmdMap;
}


QVariantMap ExtCtlHttpServer::handleCloseReq()
{
    QVariantMap cmdMap;
    //RtnStatus status = flySorterWindowPtr_ -> close();
    //cmdMap.insert("success", status.success);
    //cmdMap.insert("message", status.message);
    //cmdMap.insert("value", "");
    return cmdMap;
}

