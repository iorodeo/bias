#ifndef BIAS_STAMPED_IMAGE_HPP
#define BIAS_STAMPED_IMAGE_HPP 

#include <opencv2/core/core.hpp>

namespace bias
{
    struct StampedImage
    {
        cv::Mat image;
        double timeStamp;
        double dtEstimate;
        unsigned long frameCount;
    };

}

#endif // #ifndef BIAS_STAMPED_IMAGE_HPP
