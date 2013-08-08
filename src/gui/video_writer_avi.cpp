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
        //fourcc_ = qStringToFourcc(params.codec);
        fourcc_ = stringToFourcc(params.codec);
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

        bool openOK= true;
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


    // ---------------------------------------------------------------------
    // TO DO ... the best way to do this would be to query the codecs which 
    // are installed on the system and available to opencv and then return
    // as list of the them ... and somehow create the string to codec map. 
    // ----------------------------------------------------------------------
    QMap<unsigned int, QString> VideoWriter_avi::getFourccToStringMap()
    {
        QMap<unsigned int, QString> map;
        map[CV_FOURCC('X','V','I','D')] = QString("XVID");
        map[CV_FOURCC('D','I','B',' ')] = QString("DIB"); 
        map[CV_FOURCC('I','4','2','0')] = QString("I420");
        map[CV_FOURCC('I','Y','U','V')] = QString("IYUV");
        map[CV_FOURCC('M','P','4','2')] = QString("MP42");
        map[CV_FOURCC('D','I','V','3')] = QString("DIV3");
        map[CV_FOURCC('D','I','V','X')] = QString("DIVX");
        map[CV_FOURCC('U','2','6','3')] = QString("H263");
        map[CV_FOURCC('F','L','V','1')] = QString("FLV1");
        return map;
    }

    QMap<QString, unsigned int> VideoWriter_avi::getStringToFourccMap()
    {
        QMap<unsigned int, QString> fourccToStringMap = getFourccToStringMap();
        QMap<QString, unsigned int> stringToFourccMap;
        QMap<unsigned int, QString>::iterator it;
        for (it=fourccToStringMap.begin(); it!=fourccToStringMap.end(); it++)
        {
            unsigned int fourcc = it.key();
            QString fourccString = it.value();
            stringToFourccMap[fourccString] = fourcc;
        }
        return stringToFourccMap;
    }

    QStringList VideoWriter_avi::getListOfAllowedCodecs()
    {
        QMap<unsigned int ,QString> codecMap = getFourccToStringMap();
        QStringList codecList = codecMap.values();
        return codecList;
    }

    bool VideoWriter_avi::isAllowedCodec(QString codecString)
    {
        QStringList allowedCodecList = getListOfAllowedCodecs();
        return allowedCodecList.contains(codecString);
    }


    QString VideoWriter_avi::fourccToString(unsigned int fourcc)
    {
        QMap<unsigned int, QString> codecMap = getFourccToStringMap();
        if (codecMap.contains(fourcc))
        {
            return codecMap[fourcc];
        }
        else
        {
            return QString("unknown");
        }
    }


    unsigned int VideoWriter_avi::stringToFourcc(QString fourccString)
    {
        QMap<QString, unsigned int> stringToFourccMap = getStringToFourccMap();
        if (stringToFourccMap.contains(fourccString))
        {
            return stringToFourccMap[fourccString];
        }
        else
        {
            return CV_FOURCC('D','I','B',' ');
        }
    }

} // namespace bias

