#ifndef FRAME_DATA_HPP
#define FRAME_DATA_HPP

#include <cv.h>

namespace bias
{
    class FrameData
    {

        public:

            FrameData() {};

            int count;
            cv::Mat image;
    };

}


#endif
