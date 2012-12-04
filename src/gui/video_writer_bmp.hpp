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

        private:
            void initialize();
            bool isFirst_;
            QDir baseDir_;
            QDir logDir_;
            QString baseName_;
    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_PPM_HPP
