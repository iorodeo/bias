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
        frameCount_++;
        std::cout << __PRETTY_FUNCTION__;
        std::cout << ", added frame: " << frameCount_;
        std::cout << ", w/ timeStamp: " << stampedImg.timeStamp << std::endl;
    }


    void VideoWriter::start()
    {
        frameCount_ = 0;
        std::cout << __PRETTY_FUNCTION__;
        std::cout << ", start, width = " << size_.width; 
        std::cout << ", height = " << size_.height << std::endl;
    }


    void VideoWriter::start(cv::Size size)
    {
        setSize(size);
        start();
    }


    void VideoWriter::start(QString fileName, cv::Size size)
    {
        setFileName(fileName);
        setSize(size);
        start();
    }


    void VideoWriter::stop()
    {
        std::cout << __PRETTY_FUNCTION__;
        std::cout << ", stop, frame count = "<< frameCount_  << std::endl;
    }


} // namespace bias
