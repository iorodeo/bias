#include "video_writer_ufmf.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <QFileInfo>
#include <QDir>
#include <iostream>

namespace bias
{
    const QString DUMMY_FILENAME("dummy.ufmf");

    const unsigned int VideoWriter_ufmf::DEFAULT_FRAME_SKIP = 1;

    VideoWriter_ufmf::VideoWriter_ufmf() : VideoWriter_ufmf(DUMMY_FILENAME) {}

    VideoWriter_ufmf::~VideoWriter_ufmf() {};

    VideoWriter_ufmf::VideoWriter_ufmf(QString fileName) : VideoWriter(fileName)
    {
        isFirst_ = true;
        setFrameSkip(DEFAULT_FRAME_SKIP);
    }

    void VideoWriter_ufmf::addFrame(StampedImage stampedImg) 
    {
        if (isFirst_)
        {
            setupOutput(stampedImg);
            isFirst_ = false;
        }
        backgroundModel_.addFrame(stampedImg);

    }

    void VideoWriter_ufmf::setupOutput(StampedImage stampedImg) 
    {
        if (stampedImg.image.channels() != 1)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer ufmf setup failed:\n\n"); 
            errorMsg += "images must be single channel";
            throw RuntimeError(errorId,errorMsg);
        }

        int imageDepth = stampedImg.image.depth();
        if (imageDepth != CV_8U)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer ufmf setup failed:\n\n"); 
            errorMsg += "image depth must be CV_8U or CV_16U";
            throw RuntimeError(errorId,errorMsg);

        }
        size_ = stampedImg.image.size();

    }

} // namespace bias
