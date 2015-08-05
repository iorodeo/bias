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

    ImageModeList getListOfImageModes(unsigned int number);

    PropertyTypeList getListOfPropertyTypes();

    PixelFormatList getListOfPixelFormats();

    TriggerTypeList getListOfTriggerTypes(); 

    // Functions for converting enumerations to strings
    // ------------------------------------------------------------------------
    std::string getVideoModeString(VideoMode vidMode);

    std::string getFrameRateString(FrameRate frmRate);

    std::string getPropertyTypeString(PropertyType propType);

    std::string getPixelFormatString(PixelFormat pixFormat);

    std::string getImageInfoString(ImageInfo imgInfo);

    std::string getTriggerTypeString(TriggerType trigType);

    std::string getImageModeString(ImageMode mode);

    // ------------------------------------------------------------------------
    float getFrameRateAsFloat(FrameRate frmRate);

}

#endif // #ifndef BIAS_UTILS_HPP
