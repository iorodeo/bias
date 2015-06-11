#include "video_writer_bmp.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <QFileInfo>
#include <stdexcept>
#include <opencv2/highgui/highgui.hpp>
#include <QtDebug>

namespace bias
{

    const QString VideoWriter_bmp::IMAGE_FILE_BASE = QString("image_");
    const QString VideoWriter_bmp::IMAGE_FILE_EXT = QString(".bmp");
    const QString DUMMY_FILENAME("dummy.bmp");
    const unsigned int VideoWriter_bmp::DEFAULT_FRAME_SKIP = 1;
    const VideoWriterParams_bmp VideoWriter_bmp::DEFAULT_PARAMS = 
        VideoWriterParams_bmp();

    // VideoWriter_bmp methods
    VideoWriter_bmp::VideoWriter_bmp(QObject *parent) 
        : VideoWriter_bmp(DEFAULT_PARAMS,DUMMY_FILENAME,0,parent) 
    {}

    VideoWriter_bmp::VideoWriter_bmp(
            VideoWriterParams_bmp params,
            QString fileName, 
            unsigned int cameraNumber,
            QObject *parent
            ) : VideoWriter(fileName,cameraNumber,parent) 
    {
        isFirst_ = true;
        setFrameSkip(params.frameSkip);
    }

    VideoWriter_bmp::~VideoWriter_bmp() {}


    void VideoWriter_bmp::setFileName(QString fileName)
    {
        fileName_ = fileName;
        QFileInfo fileInfo(fileName_);
        baseName_ = fileInfo.baseName();
        baseDir_ = fileInfo.dir();
    }

    void VideoWriter_bmp::addFrame(StampedImage stampedImg)
    {
        if (isFirst_)
        {
            setupOutput();
            isFirst_= false;
        }

        QString imageFileName = IMAGE_FILE_BASE;  
        imageFileName += QString::number(frameCount_);
        imageFileName += IMAGE_FILE_EXT;
        QFileInfo imageFileInfo(logDir_,imageFileName);
        QString fullPathName = imageFileInfo.absoluteFilePath();
        if (frameCount_%frameSkip_==0) 
        {
            try
            {
                cv::imwrite(fullPathName.toStdString(), stampedImg.image);
            }
            catch (cv::Exception &exc)
            {
                unsigned int errorId = ERROR_VIDEO_WRITER_ADD_FRAME;
                std::string errorMsg("adding frame failed - "); 
                errorMsg += exc.what();
                throw RuntimeError(errorId, errorMsg);
            }
        }

        frameCount_++;
    }

    unsigned int VideoWriter_bmp::getNextVersionNumber()
    {
        unsigned int nextVerNum = 0;
        if (addVersionNumber_)
        {
            bool done = false;
            QDir testLogDir;
            while (!done)
            {
                nextVerNum++;
                testLogDir = getLogDir(nextVerNum);
                if (!testLogDir.exists())
                {
                    done = true;
                }
            }
        }
        return nextVerNum;
    }

    void VideoWriter_bmp::setupOutput()
    {
        if (!baseDir_.exists())
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("base log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            errorMsg += std::string("does not exist");
            throw RuntimeError(errorId, errorMsg);
        }

        unsigned int verNum = getNextVersionNumber();
        QString logDirName = getLogDirName(verNum);
        logDir_ = getLogDir(verNum);

        if (!baseDir_.mkdir(logDirName))
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("unable to create log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            throw RuntimeError(errorId, errorMsg);
        }

    }

    QString VideoWriter_bmp::getUniqueDirName()
    {
        unsigned int nextVerNum = getNextVersionNumber();
        QString logDirName = getLogDirName(nextVerNum);
        return logDirName;
    }

    QString VideoWriter_bmp::getLogDirName(unsigned int verNum)
    {
        QString logDirName;
        if (verNum > 0)
        { 
            QString verStr = QString("_v%1").arg(verNum,3,10,QChar('0'));
            logDirName =  baseName_ + verStr;
        }
        else
        {
            logDirName = baseName_;
        }
        return logDirName;
    }

    QDir VideoWriter_bmp::getLogDir(unsigned int verNum)
    {
        QString logDirName = getLogDirName(verNum);
        QDir logDir = QDir(baseDir_.absolutePath() + "/" + logDirName);
        return logDir;
    }

} // namespace bias
