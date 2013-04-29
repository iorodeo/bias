#ifndef BIAS_FORMAT7_HPP
#define BIAS_FORMAT7_HPP

#include "basic_types.hpp"
#include "format7_fwd.hpp"
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

        Format7Settings();
        std::string toString();
        void print();
    };

    struct Format7Info
    {
        ImageMode mode;
        bool supported;
        unsigned int maxWidth;
        unsigned int maxHeight;
        unsigned int offsetHStepSize;
        unsigned int offsetVStepSize;
        unsigned int imageHStepSize;
        unsigned int imageVStepSize;
        unsigned int pixelFormatBitField;
        unsigned int vendorPixelFormatBitField;
        unsigned int packetSize;
        unsigned int minPacketSize;
        unsigned int maxPacketSize;
        float percentage;

        Format7Info();
        Format7Info(ImageMode imgMode);
        std::string toString();
        void print();

    };

}

#endif // #ifndef BAIS_FORMAT7_HPP
