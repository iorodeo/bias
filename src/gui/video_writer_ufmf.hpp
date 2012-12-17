#ifndef BIAS_VIDEO_WRITER_UFMF_HPP
#define BIAS_VIDEO_WRITER_UFMF_HPP

#include "video_writer.hpp"
#include "background_model_ufmf.hpp"
#include <opencv2/highgui/highgui.hpp>

namespace bias
{
    class VideoWriter_ufmf : public VideoWriter
    {
        public:

            VideoWriter_ufmf();
            explicit VideoWriter_ufmf(QString fileName);
            virtual ~VideoWriter_ufmf();
            virtual void addFrame(StampedImage stampedImg);

            static const unsigned int DEFAULT_FRAME_SKIP;

        protected:

            bool isFirst_;
            BackgroundModel_ufmf backgroundModel_;
            void setupOutput(StampedImage stampedImg);
            
    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_UFMF_HPP
