#include "video_writer.hpp"
#include "Stamped_image.hpp"
#include <iostream>

namespace bias
{
    const unsigned int DEFAULT_FRAME_SKIP = 1;

    VideoWriter::VideoWriter() 
    {
        fileName_ = QString("filename_not_set");
        size_ = cv::Size(0,0);
        frameCount_ = 0;
        frameSkip_ = DEFAULT_FRAME_SKIP;
    }


    VideoWriter::VideoWriter(QString fileName)
    {
        setFileName(fileName);
        size_ = cv::Size(0,0);
        frameCount_ = 0;
        frameSkip_ = DEFAULT_FRAME_SKIP;
    }

    VideoWriter::~VideoWriter() {}


    void VideoWriter::setFileName(QString fileName)
    {
        fileName_ = fileName;
    }


    void VideoWriter::setSize(cv::Size size)
    {
        size_ = size;
    }


    void VideoWriter::setFrameSkip(unsigned int frameSkip)
    {
        frameSkip_ = frameSkip;
    }

    void VideoWriter::addFrame(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__;
        if (frameCount_%frameSkip_ == 0) 
        {
            std::cout << ", added frame: " << frameCount_;
            std::cout << ", w/ timeStamp: " << stampedImg.timeStamp;
        }
        else 
        {
            std::cout << ", skipped frame: " << frameCount_; 
        }
        std::cout << std::endl;
        frameCount_++;
    }


} // namespace bias
