#include "compressed_frame_ufmf.hpp"

namespace bias
{
    static const unsigned int CompressedFrame_ufmf::DEFAULT_BOX_LENGTH = 30; 
    static const double CompressedFrame_ufmf::DEFAULT_FG_MAX_FRAC_COMPRESS = 1.0;

    CompressedFrame_ufmf::CompressedFrame_ufmf() 
        : CompressFrame_ufmf(DEFAULT_BOX_LENGTH, DEFAULT_FG_MAX_FRAC_COMPRESS)
    { }

    CompressedFrame_ufmf::CompressedFrame_ufmf(
            unsigned int boxLength, 
            double fgMaxFracCompress
            )
    {
        haveData_ = false;
    }

    CompressedFrame_ufmf::setData(
            StampedImage stampedImg, 
            cv::Mat bgLowerBound,
            cv::Mat bgUpperBound
            )
    {
        stampedImg_ = stampedImg;
        
        cv::inRange(stampedImg, bgLowerBound, bgUpperBound, bgMembership_);

    }

}
