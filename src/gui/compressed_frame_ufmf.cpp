#include "compressed_frame_ufmf.hpp"

namespace bias
{
    const unsigned int CompressedFrame_ufmf::DEFAULT_BOX_LENGTH = 30; 
    const double CompressedFrame_ufmf::DEFAULT_FG_MAX_FRAC_COMPRESS = 1.0;

    CompressedFrame_ufmf::CompressedFrame_ufmf() 
        : CompressedFrame_ufmf(DEFAULT_BOX_LENGTH, DEFAULT_FG_MAX_FRAC_COMPRESS)
    { }

    CompressedFrame_ufmf::CompressedFrame_ufmf(
            unsigned int boxLength, 
            double fgMaxFracCompress
            )
    {
        haveData_ = false;
        isCompressed_ = false;
        numForeground_ = 0;
        numPixWritten_ = 0;
        boxLength_ = boxLength;
        boxArea_ = boxLength*boxLength;
        fgMaxFracCompress_ = fgMaxFracCompress;
    }

    void CompressedFrame_ufmf::setData(
            StampedImage stampedImg, 
            cv::Mat bgLowerBound,
            cv::Mat bgUpperBound
            )
    {
        stampedImg_ = stampedImg;
        cv::inRange(stampedImg.image, bgLowerBound, bgUpperBound, bgMembership_);

    }

    void CompressedFrame_ufmf::allocateBuffers()
    {
    }

}
