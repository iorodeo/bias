#ifndef BIAS_VIDEO_WRITER_FMF_HPP
#define BIAS_VIDEO_WRITER_FMF_HPP

#include "video_writer.hpp"

namespace bias 
{
    class VideoWriter_fmf : public VideoWriter
    {
        public:
            VideoWriter_fmf();
            VideoWriter_fmf(QString fileName);
            ~VideoWriter_fmf();
            virtual void addFrame(StampedImage stampedImg);

            static const unsigned int DEFAULT_FRAME_SKIP;

        private:
            bool isFirst_;
            void initialize(StampedImage stampImg);
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_FMF_HPP
