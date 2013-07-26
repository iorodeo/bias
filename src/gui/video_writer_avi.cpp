#include "video_writer_avi.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <QFileInfo>
#include <QDir>
#include <iostream>

namespace bias
{ 
    const QString DUMMY_FILENAME("dummy.avi");
    const double VideoWriter_avi::DEFAULT_FPS = 30.0;
    const double VideoWriter_avi::MIN_ALLOWED_DT_ESTIMATE = 0.00001; 
    const unsigned int VideoWriter_avi::DEFAULT_FRAME_SKIP = 1;
    const int VideoWriter_avi::DEFAULT_FOURCC = CV_FOURCC('X','V','I','D');
    //const int VideoWriter_avi::DEFAULT_FOURCC = CV_FOURCC('D','I','B',' ');
    const VideoWriterParams_avi VideoWriter_avi::DEFAULT_PARAMS = 
        VideoWriterParams_avi();


    VideoWriter_avi::VideoWriter_avi(QObject *parent) 
        : VideoWriter_avi(DEFAULT_PARAMS,DUMMY_FILENAME,parent) 
    {}


    VideoWriter_avi::VideoWriter_avi(
            VideoWriterParams_avi params,
            QString fileName, 
            QObject *parent
            ) 
        : VideoWriter(fileName,parent)
    {
        fourcc_ = qStringToFourcc(params.codec);
        setFrameSkip(params.frameSkip);

        isFirst_ = true;
        fps_ = DEFAULT_FPS;
    }


    VideoWriter_avi::~VideoWriter_avi() {};


    void VideoWriter_avi::addFrame(StampedImage stampedImg)
    {
        if (isFirst_)
        {
            setupOutput(stampedImg);
            isFirst_ = false;
        }
        if (frameCount_%frameSkip_==0)
        {
            videoWriter_ << stampedImg.image;
        }
        frameCount_++;
    }


    void VideoWriter_avi::setupOutput(StampedImage stampedImg)
    {
        std::string incrFileName = getUniqueFileName().toStdString();
        
        setSize(stampedImg.image.size());

        bool openOK= true;

        if (stampedImg.dtEstimate > MIN_ALLOWED_DT_ESTIMATE)
        {
            fps_ = 1.0/(stampedImg.dtEstimate*frameSkip_);
        }
        else
        {
            fps_ = 1.0;
        }

        bool isColorImage;
        if (stampedImg.image.channels() > 1)
        {
            isColorImage = true;
        }
        else
        {
            isColorImage = false;
        }

        try
        {
            openOK = videoWriter_.open(
                    incrFileName,
                    fourcc_,
                    fps_,
                    size_,
                    isColorImage
                    );
        }
        catch (cv::Exception &e)
        {
            isFirst_ = false;
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += e.what();
            throw RuntimeError(errorId, errorMsg); 
        }

        if (!openOK)
        {
            isFirst_ = false;
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += "returned false";
            throw RuntimeError(errorId, errorMsg); 
        }

        if (!videoWriter_.isOpened())
        {
            isFirst_ = false;
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += "no exception thrown, but file not opened??";
            throw RuntimeError(errorId, errorMsg); 
        }
    }


    // ----------------------------------------------------------------------
    // TO DO .... currently only allow XVID. Will need change when dealing 
    // with more codecs this is currently just a quick hack.
    // ----------------------------------------------------------------------
    bool VideoWriter_avi::isAllowedCodec(QString codecString)
    {
        if (codecString == QString("XVID"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // -----------------------------------------------------------------------
    // TO DO ... if we start dealing with auto detection of codecs, etc. these
    // funcions will need to be improved - they should be viewed as temporary.
    // ------------------------------------------------------------------------


    QString fourccToQString(unsigned int fourcc)
    {
        QString fourccQString;
        if (fourcc == CV_FOURCC('X','V','I','D'))
        {
            fourccQString = QString("XVID");
        }
        else
        {
            fourccQString = QString("unknown");
        }
        return fourccQString;
    }


    unsigned int qStringToFourcc(QString fourccQString)
    {
        unsigned int fourcc;
        if (fourccQString == QString("XVID"))
        {
            fourcc = CV_FOURCC('X','V','I','D'); 
        }
        else
        {
            // Set some default codec
            fourcc = CV_FOURCC('D','I','B',' ');
            // TO DO - better way to deal with codec not found ? 
            // ------------------------------------------------------------------
            std::cout << "Warning - codec not found set to default" << std::endl;
            // -------------------------------------------------------------------
        }
        return fourcc;
    }


} // namespace bias

