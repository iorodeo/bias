#ifndef BIAS_UTILS_HPP
#define BIAS_UTILS_HPP

#include "basic_types.hpp"
#include <string>

namespace bias
{


    // Functions which return lists of enumeration types
    // ------------------------------------------------------------------------

    VideoModeList getListOfVideoModes();

    FrameRateList getListOfFrameRates();

    ImageModeList getListOfImageModes();

    PropertyTypeList getListOfPropertyTypes();

    PixelFormatList getListOfPixelFormats();

    // Functions for converting enumerations to strings
    // ------------------------------------------------------------------------
    std::string getVideoModeString(VideoMode vidMode);

    std::string getFrameRateString(FrameRate frmRate);

    std::string getPropertyTypeString(PropertyType propType);

    std::string getPixelFormatString(PixelFormat pixFormat);

    std::string getImageInfoString(ImageInfo imgInfo);

    // ------------------------------------------------------------------------
    float getFrameRateAsFloat(FrameRate frmRate);


}

#endif // #ifndef BIAS_UTILS_HPP
