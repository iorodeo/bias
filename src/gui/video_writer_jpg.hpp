#ifndef BIAS_VIDEO_WRITER_JPG_HPP
#define BIAS_VIDEO_WRITER_JPG_HPP
#include "video_writer.hpp"
#include "video_writer_params.hpp"
#include <QDir>
#include <QString>
#include <vector>

namespace bias
{
    class VideoWriter_jpg : public VideoWriter
    {
        public:

            VideoWriter_jpg(QObject *parent=0);
            explicit VideoWriter_jpg(
                    VideoWriterParams_jpg params,
                    QString fileName, 
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );
            virtual ~VideoWriter_jpg();
            virtual void setFileName(QString fileName);
            virtual void addFrame(StampedImage stampedImg);
            virtual unsigned int getNextVersionNumber();

            static const QString IMAGE_FILE_BASE;
            static const QString IMAGE_FILE_EXT;
            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int DEFAULT_QUALITY;
            static const unsigned int MIN_QUALITY;
            static const unsigned int MAX_QUALITY;
            static const VideoWriterParams_jpg DEFAULT_PARAMS;

        protected:

            bool isFirst_;
            unsigned int quality_;
            QDir baseDir_;
            QDir logDir_;
            QString baseName_;
            void setupOutput();
            QString getUniqueDirName();
            QString getLogDirName(unsigned int verNum);
            QDir getLogDir(unsigned int verNum);

    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_JPG_HPP
