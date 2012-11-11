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

    // Functions for converting enumerations to strings
    // ------------------------------------------------------------------------
    std::string getVideoModeString(VideoMode videoMode);

    std::string getFrameRateString(FrameRate frameRate);

    std::string getPropertyTypeString(PropertyType propertyType);
}

#endif // #ifndef BIAS_UTILS_HPP
