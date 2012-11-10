#ifdef WITH_FC2
#ifndef BIAS_UTILS_FC2_HPP
#define BIAS_UTILS_FC2_HPP

#include "basic_types.hpp"
#include <string>
#include "C/FlyCapture2_C.h"

namespace bias {

    // Conversion from BIAS enumeration types to FlyCapture2 enumeration types
    // ------------------------------------------------------------------------
    
    fc2PropertyType convertPropertyType_to_fc2(PropertyType propertyType);

    fc2FrameRate convertFrameRate_to_fc2(FrameRate frameRate);

    fc2VideoMode convertVideoMode_to_fc2(VideoMode videoMode);

    fc2Mode convertImageMode_to_fc2(ImageMode imageMode);

    // Conversion from FlyCapture2 enumeration types to BIAS enumeration types
    // ------------------------------------------------------------------------

    VideoMode convertVideoMode_from_fc2(fc2VideoMode videoMode_fc2);

    FrameRate convertFrameRate_from_fc2(fc2FrameRate frameRate_fc2);

    ImageMode convertImageMode_from_fc2(fc2Mode mode_fc2);


    // Print functions for FlyCapture2 configurations, settings and info
    //-------------------------------------------------------------------------
    
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

    void printPropertyInfo_fc2(fc2PropertyInfo &propInfo);

    void printProperty_fc2(fc2Property &property);


    // FlyCapture2 enumeration to string converstions
    // ------------------------------------------------------------------------
    
    std::string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType);

    std::string getPixelFormatString_fc2(fc2PixelFormat foramt);

    std::string getBayerTileFormatString_fc2(fc2BayerTileFormat bayerFormat);

    std::string getVideoModeString_fc2(fc2VideoMode videoMode);

    std::string getFrameRateString_fc2(fc2FrameRate frameRate);

    std::string getModeString_fc2(fc2Mode mode);

    std::string getPropertyTypeString_fc2(fc2PropertyType propType);

}

#endif // #ifndef BIAS_UTILS_FC2_HPP
#endif // #ifdef WITH_FC2
