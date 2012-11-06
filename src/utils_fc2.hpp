#ifdef WITH_FC2
#ifndef BIAS_UTILS_FC2_HPP
#define BIAS_UTILS_FC2_HPP

#include <string>
#include "C/FlyCapture2_C.h"

namespace bias {

    std::string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType);

    std::string getPixelFormatString_fc2(fc2PixelFormat foramt);

    std::string getBayerTileFormatString_fc2(fc2BayerTileFormat bayerFormat);

    void printImageInfo_fc2(fc2Image &image);
}

#endif // #ifndef BIAS_UTILS_FC2_HPP
#endif // #ifdef WITH_FC2
