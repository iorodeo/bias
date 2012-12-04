#include "video_writer.hpp"
#include "Stamped_image.hpp"
#include <iostream>

namespace bias
{
    VideoWriter::VideoWriter() 
    {
        fileName_ = QString("filename_not_set");
        size_ = cv::Size(0,0);
        frameCount_ = 0;
    }


    VideoWriter::VideoWriter(QString fileName)
    {
        setFileName(fileName);
        size_ = cv::Size(0,0);
        frameCount_ = 0;
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


    void VideoWriter::addFrame(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__;
        std::cout << ", added frame: " << frameCount_;
        std::cout << ", w/ timeStamp: " << stampedImg.timeStamp << std::endl;
        frameCount_++;
    }


} // namespace bias
