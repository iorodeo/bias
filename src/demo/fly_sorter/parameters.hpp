#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP
#include <QString>
#include <QVariantMap>
#include <QFile>
#include "blob_finder_param.hpp"
#include "rtn_status.hpp"

using namespace bias;

class ImageGrabberParam
{
    public:
        static const float DEFAULT_FRAMERATE;
        float frameRate;

        ImageGrabberParam();
        QVariantMap toMap();
        RtnStatus fromMap(QVariantMap paramMap);
};


class ServerParam
{ 
    public:
        static const unsigned int DEFAULT_PORT;
        static const QString DEFAULT_ADDRESS;
        static const bool DEFAULT_ENABLED;
        bool enabled;
        QString address;
        unsigned int port;

        ServerParam();
        QVariantMap toMap();
        RtnStatus fromMap(QVariantMap paramMap);
};

class FlySorterParam
{
    public:
        ServerParam server;
        ImageGrabberParam imageGrabber;
        BlobFinderParam blobFinder;

        FlySorterParam();
        QVariantMap toMap();
        QByteArray toJson();

        RtnStatus fromJson(QByteArray paramJson);
        RtnStatus fromMap(QVariantMap paramMap);
        RtnStatus fromFile(QFile file);
        RtnStatus toFile(QFile file);
};

#endif // #ifndef PARAMETERS_HPP
