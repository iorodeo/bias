#ifndef BIAS_UTILS_HPP
#define BIAS_UTILS_HPP

#include "basic_types.hpp"
#include <string>

namespace bias
{
    std::string getVideoModeString(VideoMode videoMode);

    std::string getFrameRateString(FrameRate frameRate);
}

#endif // #ifndef BIAS_UTILS_HPP
