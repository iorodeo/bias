#ifdef WITH_SPIN
#ifndef BIAS_UTILS_SPIN_HPP
#define BIAS_UTILS_SPIN_HPP

#include "basic_types.hpp"
#include "property.hpp"
#include "format7.hpp"
#include <string>
#include <vector>
#include "SpinnakerC.h"

namespace bias 
{
    struct ImageInfo_spin
    {
        size_t xpad;
        size_t ypad;
        size_t cols;
        size_t rows;
        size_t stride;
        size_t dataSize; 
        void *dataPtr;
        spinPixelFormatEnums pixelFormat;
    };


    spinPixelFormatEnums getImagePixelFormat_spin(spinImage hImage);

    ImageInfo_spin getImageInfo_spin(spinImage hImage);

    std::vector<spinPixelFormatEnums> getAllowedPixelFormats_spin();

    std::vector<spinPixelFormatEnums> getAllowedColorPixelFormats_spin();

    std::vector<spinPixelFormatEnums> getAllowedMonoPixelFormats_spin();

    bool isAllowedPixelFormat_spin(spinPixelFormatEnums pixelFormat_spin);

    std::vector<PropertyType> getSpinSupportedPropertyTypes();

    bool isSpinSupportedPropertyType(PropertyType propType);

    spinPixelFormatEnums getSuitablePixelFormat(spinPixelFormatEnums pixFormat);

    int getCompatibleOpencvFormat(spinPixelFormatEnums pixFormat);

    spinPixelFormatEnums convertPixelFormat_to_spin(PixelFormat pixFormat);

    PixelFormat convertPixelFormat_from_spin(spinPixelFormatEnums pixFormat_spin);

    std::string getPixelFormatString_spin(spinPixelFormatEnums format);

    void printAllPixelFormatStrings_spin();


} // namespace BIAS

#endif // #ifndef BIAS_UTILS_SPIN_HPP
#endif // #ifdef WITH_SPIN
