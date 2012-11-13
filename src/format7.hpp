#ifndef BIAS_FORMAT7_HPP
#define BIAS_FORMAT7_HPP

#include "basic_types.hpp"
#include <string>

namespace bias
{
    struct Format7Settings
    {
        ImageMode mode;
        unsigned int offsetX;
        unsigned int offsetY;
        unsigned int width;
        unsigned int height;
        PixelFormat pixelFormat;

        std::string toString();
        void print();
    };

    struct Format7Info
    {
        ImageMode mode;
        unsigned int maxWidth;
        unsigned int maxHeight;
        unsigned int offsetHStepSize;
        unsigned int offsetVStepSize;
        unsigned int imageHStepSize;
        unsigned int imageVStepSize;

        // ------------------------------------------------
        // What about these ???
        // ------------------------------------------------
        unsigned int pixelFormatBitField;
        unsigned int vendorPixelFormatBitField;
        // ------------------------------------------------

        unsigned int packetSize;
        unsigned int minPacketSize;
        unsigned int maxPacketSize;
        float percentage;
    };

}

#endif // #ifndef BAIS_FORMAT7_HPP
