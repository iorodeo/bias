#include "parameters.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include <iostream>

// ImageGrabberParam
// ----------------------------------------------------------------------------

const float ImageGrabberParam::DEFAULT_FRAMERATE = 10.0;

ImageGrabberParam::ImageGrabberParam()
{
    frameRate = DEFAULT_FRAMERATE;
}

QVariantMap ImageGrabberParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("frameRate", frameRate);
    return paramMap;
}

// ServerParam
// ----------------------------------------------------------------------------

const unsigned int ServerParam::DEFAULT_PORT = 8080;
const QString ServerParam::DEFAULT_ADDRESS = QString("127.0.0.1");
const bool ServerParam::DEFAULT_ENABLED = false;

ServerParam::ServerParam()
{
    enabled = DEFAULT_ENABLED;
    address = DEFAULT_ADDRESS;
    port = DEFAULT_PORT;
}

QVariantMap ServerParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("enabled", enabled);
    paramMap.insert("address", address);
    paramMap.insert("port", port);
    return paramMap;
}


// FlySorterParam
// ----------------------------------------------------------------------------

FlySorterParam::FlySorterParam()
{
    server = ServerParam();
    imageGrabber = ImageGrabberParam();
    blobFinder = BlobFinderParam();
}


QVariantMap FlySorterParam::toMap()
{
    QVariantMap paramMap;
    QVariantMap serverParamMap = server.toMap();
    QVariantMap imageGrabberParamMap = imageGrabber.toMap();
    QVariantMap blobFinderParamMap = blobFinder.toMap();

    paramMap.insert("server", serverParamMap);
    paramMap.insert("imageGrabber", imageGrabberParamMap);
    paramMap.insert("blobFinder", blobFinderParamMap);

    return paramMap;
}


QByteArray FlySorterParam::toJson()
{
    bool ok;
    QVariantMap paramMap = toMap();
    QByteArray json = QtJson::serialize(paramMap,ok);

    return json;
}
