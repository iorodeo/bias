#ifndef BIAS_VIDEO_WRITER_BMP_HPP
#define BIAS_VIDEO_WRITER_BMP_HPP
#include "video_writer.hpp"
#include <QDir>
#include <QString>
#include <vector>

namespace bias
{
    class VideoWriter_bmp : public VideoWriter
    {
        public:

            VideoWriter_bmp();
            explicit VideoWriter_bmp(QString fileName);
            virtual ~VideoWriter_bmp();
            virtual void addFrame(StampedImage stampedImg);

            static const QString IMAGE_FILE_BASE;
            static const QString IMAGE_FILE_EXT;
            static const unsigned int DEFAULT_FRAME_SKIP;

        protected:
            void initialize();
            bool isFirst_;
            QDir baseDir_;
            QDir logDir_;
            QString baseName_;
    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_BMP_HPP
