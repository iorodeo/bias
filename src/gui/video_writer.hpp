#ifndef BIAS_VIDEO_WRITER_HPP
#define BIAS_VIDEO_WRITER_HPP
#include <QString>
#include <opencv2/core/core.hpp>
//#include <opencv2/core/types_c.h>

namespace bias
{
    class StampedImage;

    class VideoWriter
    {
        public:

            VideoWriter(); 
            explicit VideoWriter(QString fileName);
            virtual void setFileName(QString fileName);
            virtual void setSize(cv::Size size);
            virtual void addFrame(StampedImage stampedImg);
            virtual void start();
            virtual void start(cv::Size size);
            virtual void start(QString fileName, cv::Size size);
            virtual void stop();

        private:

            cv::Size size_;
            QString fileName_;
            unsigned long frameCount_;
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_HPP
