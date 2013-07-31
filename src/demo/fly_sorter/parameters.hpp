#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QFile>
#include "blob_finder_param.hpp"
#include "rtn_status.hpp"

using namespace bias;

class ImageGrabberParam
{
    public:
        static const float DEFAULT_FRAMERATE;
        static const float MINIMUM_FRAMERATE;
        static const float MAXIMUM_FRAMERATE;

        static const float DEFAULT_GAIN;
        static const float MINIMUM_GAIN;
        static const float MAXIMUM_GAIN;

        static const float DEFAULT_SHUTTER;
        static const float MINIMUM_SHUTTER;
        static const float MAXIMUM_SHUTTER;

        static const unsigned int DEFAULT_BRIGHTNESS;
        static const unsigned int MAXIMUM_BRIGHTNESS;

        static const QStringList  ALLOWED_CAPTURE_MODES;
        static const QString DEFAULT_CAPTURE_INPUT_FILE;

        float frameRate;
        float gain;
        float shutter;
        unsigned int brightness;
        QString captureMode;
        QString captureInputFile;

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


class IdentityTrackerParam
{
    public:

        static const float DEFAULT_RADIUS;
        static const float DEFAULT_VELOCITY_X;
        static const float DEFAULT_VELOCITY_Y;

        float radius;
        float velocityX;
        float velocityY; 

        IdentityTrackerParam();
        QVariantMap toMap();
        RtnStatus fromMap(QVariantMap paramMap);
};


// Devel
// ----------------------
enum GenderMode {
    GenderModeMaleOnly,
    GenderModeEveryOther,
    GenderModeRandom,
    GenderModeNotFound
};
// -----------------------

class FlySorterParam
{
    public:
        
        ServerParam server;
        ImageGrabberParam imageGrabber;
        BlobFinderParam blobFinder;
        // Devel
        // -------------------
        GenderMode genderMode;
        // -------------------

        FlySorterParam();
        QVariantMap toMap();
        QByteArray toJson();

        RtnStatus fromJson(QByteArray paramJson);
        RtnStatus fromMap(QVariantMap paramMap);
};

#endif // #ifndef PARAMETERS_HPP
