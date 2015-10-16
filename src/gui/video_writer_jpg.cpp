#include "video_writer_jpg.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <QFileInfo>
#include <stdexcept>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <QtDebug>

namespace bias
{

    const QString VideoWriter_jpg::IMAGE_FILE_BASE = QString("image_");
    const QString VideoWriter_jpg::IMAGE_FILE_EXT = QString(".jpg");
    const QString DUMMY_FILENAME("dummy.jpg");
    const unsigned int VideoWriter_jpg::DEFAULT_FRAME_SKIP = 1;
    const unsigned int VideoWriter_jpg::DEFAULT_QUALITY = 90;
    const unsigned int VideoWriter_jpg::MIN_QUALITY = 0;
    const unsigned int VideoWriter_jpg::MAX_QUALITY = 100;
    const VideoWriterParams_jpg VideoWriter_jpg::DEFAULT_PARAMS = VideoWriterParams_jpg();

    // VideoWriter_jpg methods
    VideoWriter_jpg::VideoWriter_jpg(QObject *parent) 
        : VideoWriter_jpg(DEFAULT_PARAMS,DUMMY_FILENAME,0,parent) 
    {}

    VideoWriter_jpg::VideoWriter_jpg(
            VideoWriterParams_jpg params,
            QString fileName, 
            unsigned int cameraNumber,
            QObject *parent
            ) : VideoWriter(fileName,cameraNumber,parent) 
    {
        isFirst_ = true;
        setFrameSkip(params.frameSkip);
        quality_ = params.quality;
    }

    VideoWriter_jpg::~VideoWriter_jpg() {}


    void VideoWriter_jpg::setFileName(QString fileName)
    {
        fileName_ = fileName;
        QFileInfo fileInfo(fileName_);
        baseName_ = fileInfo.baseName();
        baseDir_ = fileInfo.dir();
    }

    void VideoWriter_jpg::addFrame(StampedImage stampedImg)
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
            std::vector<int> compressionParams;
            compressionParams.push_back(CV_IMWRITE_JPEG_QUALITY);
            compressionParams.push_back(int(quality_));

            try
            {
                cv::imwrite(fullPathName.toStdString(), stampedImg.image, compressionParams);
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

    unsigned int VideoWriter_jpg::getNextVersionNumber()
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

    void VideoWriter_jpg::setupOutput()
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

        if (baseDir_.exists(logDirName) && !addVersionNumber_)
        {
            QString dirCopyName = logDirName + QString("_copy");
            if (baseDir_.exists(dirCopyName))
            {
                bool done = false;
                unsigned int cnt = 2;
                while (!done)
                {
                    dirCopyName = logDirName + QString("_copy_%1").arg(cnt);
                    if (!baseDir_.exists(dirCopyName))
                    {
                        done = true;
                    }
                    cnt++;
                }
            }
            baseDir_.rename(logDirName,  dirCopyName);
        }

        if (!baseDir_.mkdir(logDirName))
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("unable to create log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            throw RuntimeError(errorId, errorMsg);
        }

    }

    QString VideoWriter_jpg::getUniqueDirName()
    {
        unsigned int nextVerNum = getNextVersionNumber();
        QString logDirName = getLogDirName(nextVerNum);
        return logDirName;
    }

    QString VideoWriter_jpg::getLogDirName(unsigned int verNum)
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

    QDir VideoWriter_jpg::getLogDir(unsigned int verNum)
    {
        QString logDirName = getLogDirName(verNum);
        QDir logDir = QDir(baseDir_.absolutePath() + "/" + logDirName);
        return logDir;
    }

} // namespace bias
