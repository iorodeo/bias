#ifndef BIAS_VIDEO_WRITER_PARAMS_HPP
#define BIAS_VIDEO_WRITER_PARAMS_HPP
#include <QString>
#include <string>

namespace bias
{


    struct VideoWriterParams_bmp
    {
        unsigned int frameSkip;
        VideoWriterParams_bmp();
        std::string toString();
    };


    struct VideoWriterParams_jpg
    {
        unsigned int frameSkip;
        unsigned int quality;
        unsigned int numberOfCompressors;
        VideoWriterParams_jpg();
        std::string toString();
    };


    struct VideoWriterParams_avi
    {
        unsigned int frameSkip;
        QString codec;
        VideoWriterParams_avi();
        std::string toString();
    };


    struct VideoWriterParams_fmf
    {
        unsigned int frameSkip;
        VideoWriterParams_fmf();
        std::string toString();
    };


    struct VideoWriterParams_ufmf 
    {
        unsigned int backgroundThreshold;
        unsigned int boxLength;
        unsigned int frameSkip;
        unsigned int numberOfCompressors;
        unsigned int medianUpdateCount;
        unsigned int medianUpdateInterval;
        unsigned int dilateWindowSize;
        bool dilateState;
        VideoWriterParams_ufmf();
        std::string toString();
    };


    struct VideoWriterParams
    {
        VideoWriterParams_bmp bmp;
        VideoWriterParams_jpg jpg;
        VideoWriterParams_avi avi;
        VideoWriterParams_fmf fmf;
        VideoWriterParams_ufmf ufmf;
        std::string toString();
    };


} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_PARAMS_HPP
