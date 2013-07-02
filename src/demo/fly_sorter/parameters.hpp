#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP
#include <QString>
#include <QVariantMap>
#include <QFile>
#include "blob_finder_param.hpp"
#include "common.hpp"

class ImageGrabberParam
{
    public:
        static const float DEFAULT_FRAMERATE;
        float frameRate;
        ImageGrabberParam();
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
