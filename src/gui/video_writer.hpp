#ifndef BIAS_VIDEO_WRITER_HPP
#define BIAS_VIDEO_WRITER_HPP
#include "stamped_image.hpp"
#include <QString>
#include <opencv2/core/core.hpp>

namespace bias
{
    class VideoWriter
    {
        public:

            VideoWriter(); 
            explicit VideoWriter(QString fileName);
            virtual ~VideoWriter();
            virtual void setFileName(QString fileName);
            virtual void setSize(cv::Size size);
            virtual void setFrameSkip(unsigned int frameSkip);
            virtual void addFrame(StampedImage stampedImg);
            virtual QString getFileName();
            virtual cv::Size getSize();
            virtual unsigned int getFrameSkip();

        protected:

            cv::Size size_;
            QString fileName_;
            unsigned long frameCount_;
            unsigned int frameSkip_;
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_HPP
