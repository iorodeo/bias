#include "video_writer.hpp"
#include "Stamped_image.hpp"
#include <iostream>
#include <QFileInfo>
#include <QDir>

namespace bias
{
    const unsigned int DEFAULT_FRAME_SKIP = 1;
    const QString DUMMY_FILENAME("dummy_filename");

    VideoWriter::VideoWriter(QObject *parent) 
        : VideoWriter(DUMMY_FILENAME,parent) 
    {}

    VideoWriter::VideoWriter(QString fileName, QObject *parent) 
        : QObject(parent)
    {
        setFileName(fileName);
        size_ = cv::Size(0,0);
        frameCount_ = 0;
        frameSkip_ = DEFAULT_FRAME_SKIP;
    }

    VideoWriter::~VideoWriter() 
    {}


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
    
    QString VideoWriter::getFileName()
    {
        return fileName_;
    }

    cv::Size VideoWriter::getSize()
    {
        return size_;
    }

    unsigned int VideoWriter::getFrameSkip()
    {
        return frameSkip_;
    }

    void VideoWriter::finish() {};

    QString VideoWriter::getUniqueFileName()
    {
        QFileInfo fileInfo(fileName_);
        QString incrFileName = fileName_;

        if (fileInfo.exists())
        {
            QDir filePath = QDir(fileInfo.absolutePath());
            QString baseName = fileInfo.baseName();
            QString ext = fileInfo.suffix();

            unsigned int cnt = 2;
            while(fileInfo.exists())
            {
                QString ver = "_v" + QString::number(cnt);
                fileInfo = QFileInfo(filePath, baseName + ver + "." + ext);
                incrFileName = fileInfo.absoluteFilePath();
                cnt++;
            }
        }

        return incrFileName;
    }


} // namespace bias
