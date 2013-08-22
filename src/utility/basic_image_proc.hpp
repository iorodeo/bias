#ifndef BASIC_IMAGE_PROC_HPP
#define BASIC_IAMGE_PROC_HPP
#include <opencv2/core/core.hpp>

namespace bias
{
    cv::Mat bwAreaOpen(cv::Mat img, unsigned int areaThreshold);
    cv::Mat imCloseWithDiskElem(cv::Mat img, unsigned int radius);
}

#endif // #ifndef BASIC_IMAGE_PROC_HPP
