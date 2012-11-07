#ifdef WITH_FC2
#ifndef BIAS_UTILS_FC2_HPP
#define BIAS_UTILS_FC2_HPP

#include <string>
#include "C/FlyCapture2_C.h"

namespace bias {


    void printImageInfo_fc2(fc2Image &image);

    void printFormat7ImageSettings_fc2(fc2Format7ImageSettings &imageSettings);

    void printFormat7PacketSettings_fc2(unsigned int packetSize, float percentage);

    void printFormat7Configuration_fc2(
            fc2Format7ImageSettings &imageSettings,
            unsigned int packetSize,
            float percentage
            );

    void printFormat7Info_fc2(fc2Format7Info &format7Info);

    void printFormat7PacketInfo_fc2(fc2Format7PacketInfo &packetInfo);

    // Convert enumeration values to human readable strings
    std::string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType);

    std::string getPixelFormatString_fc2(fc2PixelFormat foramt);

    std::string getBayerTileFormatString_fc2(fc2BayerTileFormat bayerFormat);

    std::string getVideoModeString_fc2(fc2VideoMode videoMode);

    std::string getFrameRateString_fc2(fc2FrameRate frameRate);

    std::string getModeString_fc2(fc2Mode mode);

}

#endif // #ifndef BIAS_UTILS_FC2_HPP
#endif // #ifdef WITH_FC2
