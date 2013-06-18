#include "blob_finder_param.hpp"
#include <sstream>
#include <iostream>

const float  BlobFinderParam::DEFAULT_FRAMERATE         = 10.0;
const float  BlobFinderParam::DEFAULT_DISPLAY_SCALE     = 0.5;
const double BlobFinderParam::DEFAULT_THRESHOLD         = 100.0;
const double BlobFinderParam::DEFAULT_THRESHOLD_MAXVAL  = 255.0;
const double BlobFinderParam::DEFAULT_MINIMUM_AREA      = 100.0;
const double BlobFinderParam::DEFAULT_MAXIMUM_AREA      = 640.0*480.0;
const unsigned int BlobFinderParam::DEFAULT_SERVER_PORT = 8080;
const std::string BlobFinderParam::DEFAULT_SERVER_ADDR  = std::string("127.0.0.1");
const bool BlobFinderParam::DEFAULT_SERVER_ENABLED      = false;


BlobFinderParam::BlobFinderParam()
{
    frameRate = DEFAULT_FRAMERATE;
    displayScale = DEFAULT_DISPLAY_SCALE;
    threshold = DEFAULT_THRESHOLD;
    thresholdMaxVal = DEFAULT_THRESHOLD_MAXVAL;
    minimumArea = DEFAULT_MINIMUM_AREA;
    maximumArea = DEFAULT_MAXIMUM_AREA;
    serverPort = DEFAULT_SERVER_PORT;
    serverAddress = DEFAULT_SERVER_ADDR;
    serverEnabled = DEFAULT_SERVER_ENABLED;
}


std::string BlobFinderParam::toStdString()
{
    std::stringstream ss;
    ss << "frameRate:       " << frameRate        << std::endl;
    ss << "displayScale:    " << displayScale     << std::endl;
    ss << "threshold:       " << threshold        << std::endl;
    ss << "thresholdMaxVal: " << thresholdMaxVal  << std::endl;
    ss << "minimumArea:     " << minimumArea      << std::endl;
    ss << "maximumArea:     " << maximumArea      << std::endl;
    ss << "serverAddress:   " << serverAddress    << std::endl;
    return ss.str();
}


void BlobFinderParam::print()
{
    std::cout << toStdString();
}


