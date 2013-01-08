#ifndef BIAS_VIDEO_WRITER_IFMF_HPP
#define BIAS_VIDEO_WRITER_IFMF_HPP

#include "video_writer.hpp"
#include <fstream>

namespace bias 
{
    class VideoWriter_ifmf : public VideoWriter
    {
        public:
            VideoWriter_ifmf(QObject *parent=0);
            VideoWriter_ifmf(QString fileName, QObject *paraent=0);
            ~VideoWriter_ifmf();
            virtual void finish();
            virtual void addFrame(StampedImage stampedImg);

            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int FMF_VERSION;

        private:
            bool isFirst_;
            cv::Mat buffer_;
            std::fstream file_;
            uint64_t numWritten_;
            unsigned int rowOffset_;
            unsigned int colOffset_;
            void setupOutput(StampedImage stampImg);
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_IFMF_HPP
