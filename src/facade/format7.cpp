#include "format7.hpp"
#include "utils.hpp"
#include <sstream>
#include <iostream>

namespace bias
{

    // Format7Settings
    // ------------------------------------------------------------------------

    Format7Settings::Format7Settings()
    {
        mode = IMAGEMODE_UNSPECIFIED;
        offsetX = 0;
        offsetY = 0;
        width = 0;
        height = 0;
        pixelFormat = PIXEL_FORMAT_UNSPECIFIED;
    }


    std::string Format7Settings::toString()
    {
        std::stringstream ss;
        ss << "mode:        " << getImageModeString(mode) << std::endl;
        ss << "pixelFormat: " << getPixelFormatString(pixelFormat) << std::endl;
        ss << "offsetX:     " << offsetX << std::endl;
        ss << "offsetY:     " << offsetY << std::endl;
        ss << "width:       " << width << std::endl;
        ss << "height:      " << height << std::endl;
        ss << std::endl;
        return ss.str();
    }


    void Format7Settings::print()
    {
        std::cout << toString();
    }


    // Format7Info
    // ------------------------------------------------------------------------
    
    Format7Info::Format7Info()
    {
        mode = IMAGEMODE_UNSPECIFIED;
        supported = false;
        maxWidth = 0;
        maxHeight = 0;
        offsetHStepSize = 0;
        offsetVStepSize = 0;
        imageHStepSize = 0;
        imageVStepSize = 0;
        pixelFormatBitField = 0;
        vendorPixelFormatBitField = 0;
        packetSize = 0;
        minPacketSize = 0;
        maxPacketSize = 0;
        percentage = 0.0;
    }


    Format7Info::Format7Info(ImageMode imgMode) : Format7Info()
    {
        mode = imgMode;
    }


    std::string Format7Info::toString()
    {
        std::stringstream ss;
        ss << "mode:                       " << getImageModeString(mode) << std::endl;
        ss << "supported:                  " << supported << std::endl;
        ss << "maxWidth:                   " << maxWidth << std::endl;
        ss << "maxHeight:                  " << maxHeight << std::endl;
        ss << "offsetHStepSize:            " << offsetHStepSize << std::endl;
        ss << "offsetVStepSize:            " << offsetVStepSize << std::endl;
        ss << "imageHStepSize:             " << imageHStepSize << std::endl;
        ss << "imageVStepSize:             " << imageVStepSize << std::endl;
        ss << "pixelFormatBitField:        " << pixelFormatBitField << std::endl;
        ss << "vendorPixelFormatBitField:  " << vendorPixelFormatBitField << std::endl;
        ss << "packetSize:                 " << packetSize << std::endl;
        ss << "minPacketSize:              " << minPacketSize << std::endl;
        ss << "maxPacketSize:              " << maxPacketSize << std::endl;
        ss << "percentage:                 " << percentage << std::endl;
        return ss.str();
    }


    void Format7Info::print()
    {
        std::cout << toString();
    }


} // namespace bias
