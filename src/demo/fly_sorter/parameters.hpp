#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP
#include <QString>
#include "blob_finder_param.hpp"

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
};

#endif // #ifndef PARAMETERS_HPP
