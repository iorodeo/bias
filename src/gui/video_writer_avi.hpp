#ifndef BIAS_VIDEO_WRITER_AVI_HPP
#define BIAS_VIDEO_WRITER_AVI_HPP

#include "video_writer.hpp"
#include <opencv2/highgui/highgui.hpp>

namespace bias
{
    class VideoWriter_avi : public VideoWriter
    {
        public:

            VideoWriter_avi();
            explicit VideoWriter_avi(QString fileName);
            virtual ~VideoWriter_avi();
            virtual void addFrame(StampedImage stampedImg);

            static const int DEFAULT_FOURCC;
            static const double DEFAULT_FPS;
            static const unsigned int DEFAULT_FRAME_SKIP;

        protected:

            int fourcc_;
            double fps_;
            bool isFirst_;
            cv::VideoWriter videoWriter_;
            void initialize(StampedImage stampedImage);
            
    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_AVI_HPP

