#include "video_writer_bmp.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <QFileInfo>
#include <stdexcept>
#include <opencv2/highgui/highgui.hpp>

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

    void VideoWriter_bmp::setupOutput()
    {
        QFileInfo fileInfo(fileName_);
        baseName_ = fileInfo.baseName();
        baseDir_ = fileInfo.dir();

        if (!baseDir_.exists())
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("base log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            errorMsg += std::string("does not exist");
            throw RuntimeError(errorId, errorMsg);
        }

        logDir_ = QDir(baseDir_.absolutePath() + "/" + baseName_);
        if (logDir_.exists()) 
        {
            // Log directory exists - increment baseName until we find
            // one which doesn't a bit kludgey, but easy.
            QString baseNameTemp;

            unsigned int cnt = 2;
            while (logDir_.exists())
            {
                baseNameTemp =  baseName_ + "_v" + QString::number(cnt);
                logDir_ = QDir(baseDir_.absolutePath() + "/" + baseNameTemp);
                cnt++;
            }

            baseName_ = baseNameTemp;
        }

        if (!baseDir_.mkdir(baseName_))
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("unable to create log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            throw RuntimeError(errorId, errorMsg);
        }

    }

} // namespace bias
