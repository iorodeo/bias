#ifndef BIAS_VIDEO_WRITER_PARAMS_HPP
#define BIAS_VIDEO_WRITER_PARAMS_HPP
#include <QString>

namespace bias
{


    struct VideoWriterParams_bmp
    {
        unsigned int frameSkip;
        VideoWriterParams_bmp();
    };


    struct VideoWriterParams_avi
    {
        unsigned int frameSkip;
        QString codec;
        VideoWriterParams_avi();
    };


    struct VideoWriterParams_fmf
    {
        unsigned int frameSkip;
        VideoWriterParams_fmf();
    };


    struct VideoWriterParams_ufmf 
    {
        unsigned int backgroundThreshold;
        unsigned int boxLength;
        unsigned int frameSkip;
        unsigned int numberOfCompressors;
        unsigned int medianUpdateCount;
        VideoWriterParams_ufmf();
    };


    struct VideoWriterParams
    {
        VideoWriterParams_bmp bmp;
        VideoWriterParams_avi avi;
        VideoWriterParams_fmf fmf;
        VideoWriterParams_ufmf ufmf;
    };


} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_PARAMS_HPP
