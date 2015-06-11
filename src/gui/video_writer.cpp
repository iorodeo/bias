#include "video_writer.hpp"
#include "stamped_image.hpp"
#include <iostream>
#include <QDir>
#include <QtDebug>

namespace bias
{
    const unsigned int DEFAULT_FRAME_SKIP = 1;
    const QString DUMMY_FILENAME("dummy_filename");

    VideoWriter::VideoWriter(QObject *parent) 
        : VideoWriter(DUMMY_FILENAME,0, parent) 
    {}

    VideoWriter::VideoWriter(QString fileName, unsigned int cameraNumber, QObject *parent) 
        : QObject(parent)
    {
        setFileName(fileName);
        cameraNumber_ = cameraNumber;
        size_ = cv::Size(0,0);
        frameCount_ = 0;
        frameSkip_ = DEFAULT_FRAME_SKIP;
        addVersionNumber_ = true;
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

    void VideoWriter::setVersioning(bool value)
    {
        addVersionNumber_ = value;
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
    
    QString VideoWriter::getFileName() const
    {
        return fileName_;
    }

    cv::Size VideoWriter::getSize() const
    {
        return size_;
    }

    unsigned int VideoWriter::getFrameSkip() const
    {
        return frameSkip_;
    }

    void VideoWriter::finish() {};

    unsigned int VideoWriter::getNextVersionNumber()
    {
        unsigned int nextVerNum = 0;

        if (addVersionNumber_)
        {
            bool done = false;
            nextVerNum  = 1;
            while (!done)
            {
                QFileInfo fileInfo = getFileInfo(nextVerNum);
                if (!fileInfo.exists())
                {
                    done =  true;
                }
                else
                {
                    nextVerNum++;
                }
            }
        }
        return nextVerNum;
    }

    QString VideoWriter::getUniqueFileName()
    {
        unsigned int verNum = getNextVersionNumber();
        QFileInfo fileInfo = getFileInfo(verNum);
        return fileInfo.absoluteFilePath();
    }

    QFileInfo VideoWriter::getFileInfo(unsigned int verNum)
    {
        QFileInfo fileInfo(fileName_);
        QDir filePath = QDir(fileInfo.absolutePath());
        QString baseName = fileInfo.baseName();
        QString ext = fileInfo.suffix();

        if (addVersionNumber_ && (verNum > 0))
        {
            QString verStr = QString("_v%1").arg(verNum,3,10,QChar('0'));
            fileInfo = QFileInfo(filePath, baseName + verStr + "." + ext);
        }
        return fileInfo;
    }


} // namespace bias
