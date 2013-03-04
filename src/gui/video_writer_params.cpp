#include "video_writer_params.hpp"
#include "video_writer_bmp.hpp"
#include "video_writer_avi.hpp"
#include "video_writer_fmf.hpp"
#include "video_writer_ufmf.hpp"
#include "background_histogram_ufmf.hpp"

namespace bias
{

    VideoWriterParams_bmp::VideoWriterParams_bmp()
    {
        frameSkip = VideoWriter_bmp::DEFAULT_FRAME_SKIP;
    }


    VideoWriterParams_avi::VideoWriterParams_avi()
    {
        frameSkip = VideoWriter_avi::DEFAULT_FRAME_SKIP;
        unsigned int fourcc = VideoWriter_avi::DEFAULT_FOURCC;
        codec = fourccToQString(fourcc); 
    }


    VideoWriterParams_fmf::VideoWriterParams_fmf()
    {
        frameSkip = VideoWriter_fmf::DEFAULT_FRAME_SKIP;
    }

        
    VideoWriterParams_ufmf::VideoWriterParams_ufmf()
    {
        backgroundThreshold = VideoWriter_ufmf::DEFAULT_BACKGROUND_THRESHOLD;
        frameSkip = VideoWriter_ufmf::DEFAULT_FRAME_SKIP;
        boxLength = VideoWriter_ufmf::DEFAULT_BOX_LENGTH;
        numberOfCompressors = VideoWriter_ufmf::DEFAULT_NUMBER_OF_COMPRESSORS;
        medianUpdateCount = BackgroundHistogram_ufmf::DEFAULT_MEDIAN_UPDATE_COUNT; 
    }

} // namespace bias
