#ifndef BIAS_VIDEO_WRITER_PARAMS_HPP
#define BIAS_VIDEO_WRITER_PARAMS_HPP

namespace bias
{
    struct VideoWriterParams_ufmf 
    {
        unsigned int backgroundThreshold;
        unsigned int boxLength;
        unsigned int frameSkip;
        unsigned int numberOfCompressors;
        unsigned int medianUpdateCount;
        VideoWriterParams_ufmf();
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_PARAMS_HPP
