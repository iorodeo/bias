#include "parameters.hpp"

// ImageGrabberParam
// ----------------------------------------------------------------------------

const float ImageGrabberParam::DEFAULT_FRAMERATE = 10.0;

ImageGrabberParam::ImageGrabberParam()
{
    frameRate = DEFAULT_FRAMERATE;
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


// FlySorterParam
// ----------------------------------------------------------------------------

FlySorterParam::FlySorterParam()
{
    server = ServerParam();
    imageGrabber = ImageGrabberParam();
    blobFinder = BlobFinderParam();
}
