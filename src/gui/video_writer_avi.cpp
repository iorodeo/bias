#include "video_writer_avi.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <QFileInfo>
#include <QDir>
#include <iostream>

namespace bias
{ 
    // ----------------------------------------------------------
    // Temporary ... need to estimate this before starting video
    const double DEFAULT_AVI_FPS = 30.0; 
    // ----------------------------------------------------------

    const unsigned int VideoWriter_avi::DEFAULT_FRAME_SKIP = 5;

    VideoWriter_avi::VideoWriter_avi() : VideoWriter() 
    {
        isFirst_ = true;
        setFrameSkip(DEFAULT_FRAME_SKIP);
    }

    VideoWriter_avi::VideoWriter_avi(QString fileName) : VideoWriter(fileName)
    {
        isFirst_ = true;
        setFrameSkip(DEFAULT_FRAME_SKIP);
    }

    VideoWriter_avi::~VideoWriter_avi() {};

    void VideoWriter_avi::addFrame(StampedImage stampedImg)
    {
        if (isFirst_)
        {
            initialize(stampedImg);
            isFirst_ = false;
        }
        if (frameCount_%frameSkip_==0)
        {
            videoWriter_ << stampedImg.image;
        }
        frameCount_++;
    }

    void VideoWriter_avi::initialize(StampedImage stampedImg)
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

        try
        {
            videoWriter_.open(
                    incrFileName.toStdString(), 
                    CV_FOURCC('D','I','V', 'X'), 
                    DEFAULT_AVI_FPS, 
                    stampedImg.image.size(), 
                    false 
                    );
        }
        catch (cv::Exception &e)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += e.what();
            throw RuntimeError(errorId, errorMsg); 
        }

        if (!videoWriter_.isOpened())
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += "no exception thrown, but file not opened??";
            throw RuntimeError(errorId, errorMsg); 
        }
        setSize(stampedImg.image.size());
    }

} // namespace bias

