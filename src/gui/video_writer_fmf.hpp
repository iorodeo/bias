#ifndef BIAS_VIDEO_WRITER_FMF_HPP
#define BIAS_VIDEO_WRITER_FMF_HPP

#include "video_writer.hpp"
#include <fstream>

namespace bias 
{
    class VideoWriter_fmf : public VideoWriter
    {
        public:
            VideoWriter_fmf(QObject *parent=0);
            VideoWriter_fmf(QString fileName, QObject *paraent=0);
            ~VideoWriter_fmf();
            virtual void finish();
            virtual void addFrame(StampedImage stampedImg);

            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int FMF_VERSION;

        private:
            bool isFirst_;
            std::fstream file_;
            uint64_t numWritten_;
            void setupOutput(StampedImage stampImg);
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_FMF_HPP
