#include "video_writer_fmf.hpp"
#include <iostream>
#include <stdint.h>

namespace bias
{
    const unsigned int VideoWriter_fmf::DEFAULT_FRAME_SKIP = 5;

    VideoWriter_fmf::VideoWriter_fmf() : VideoWriter() 
    {
        isFirst_ = true;
        setFrameSkip(DEFAULT_FRAME_SKIP);
    }

    VideoWriter_fmf::VideoWriter_fmf(QString fileName) : VideoWriter(fileName)
    {
        isFirst_ = true;
        setFrameSkip(DEFAULT_FRAME_SKIP);
    }


    VideoWriter_fmf::~VideoWriter_fmf()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    void VideoWriter_fmf::addFrame(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (isFirst_)
        {
            initialize(stampedImg);
            isFirst_ = false;
        }
        if (frameCount_%frameSkip_==0)
        {
            // Add frame to file
        }
        frameCount_++;
    }

    void VideoWriter_fmf::initialize(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }


} // namespace bias
