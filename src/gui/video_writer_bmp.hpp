#ifndef BIAS_VIDEO_WRITER_BMP_HPP
#define BIAS_VIDEO_WRITER_BMP_HPP
#include "video_writer.hpp"
#include "video_writer_params.hpp"
#include <QDir>
#include <QString>
#include <vector>

namespace bias
{
    class VideoWriter_bmp : public VideoWriter
    {
        public:

            VideoWriter_bmp(QObject *parent=0);
            explicit VideoWriter_bmp(
                    VideoWriterParams_bmp params,
                    QString fileName, 
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );
            virtual ~VideoWriter_bmp();
            virtual void setFileName(QString fileName);
            virtual void addFrame(StampedImage stampedImg);
            virtual unsigned int getNextVersionNumber();

            static const QString IMAGE_FILE_BASE;
            static const QString IMAGE_FILE_EXT;
            static const unsigned int DEFAULT_FRAME_SKIP;
            static const VideoWriterParams_bmp DEFAULT_PARAMS;

        protected:

            bool isFirst_;
            QDir baseDir_;
            QDir logDir_;
            QString baseName_;
            void setupOutput();
            QString getUniqueDirName();
            QString getLogDirName(unsigned int verNum);
            QDir getLogDir(unsigned int verNum);

    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_BMP_HPP
