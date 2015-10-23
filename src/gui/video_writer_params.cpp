#include "video_writer_params.hpp"
#include "video_writer_bmp.hpp"
#include "video_writer_jpg.hpp"
#include "video_writer_avi.hpp"
#include "video_writer_fmf.hpp"
#include "video_writer_ufmf.hpp"
#include "background_histogram_ufmf.hpp"
#include <sstream>

namespace bias
{

    VideoWriterParams_bmp::VideoWriterParams_bmp()
    {
        frameSkip = VideoWriter_bmp::DEFAULT_FRAME_SKIP;
    }

    std::string VideoWriterParams_bmp::toString()
    {
        std::stringstream ss;
        ss << "frameSkip: " << frameSkip << std::endl;
        return ss.str();
    }


    VideoWriterParams_jpg::VideoWriterParams_jpg()
    {
        frameSkip = VideoWriter_jpg::DEFAULT_FRAME_SKIP;
        quality = VideoWriter_jpg::DEFAULT_QUALITY;
        numberOfCompressors = VideoWriter_jpg::DEFAULT_NUMBER_OF_COMPRESSORS;
    }

    
    std::string VideoWriterParams_jpg::toString()
    {
        std::stringstream ss;
        ss << "frameSkip: " << frameSkip << std::endl;
        ss << "quality: " << quality << std::endl;
        ss << "numberOfCompressors: " << numberOfCompressors << std::endl;
        return ss.str();
    }


    VideoWriterParams_avi::VideoWriterParams_avi()
    {
        frameSkip = VideoWriter_avi::DEFAULT_FRAME_SKIP;
        unsigned int fourcc = VideoWriter_avi::DEFAULT_FOURCC;
        //codec = fourccToQString(fourcc); 
        codec = VideoWriter_avi::fourccToString(fourcc); 
    }

    std::string VideoWriterParams_avi::toString()
    {
        std::stringstream ss;
        ss << "frameSkip: " << frameSkip << std::endl;
        ss << "codec: " << codec.toStdString() << std::endl;
        return ss.str();
    }


    VideoWriterParams_fmf::VideoWriterParams_fmf()
    {
        frameSkip = VideoWriter_fmf::DEFAULT_FRAME_SKIP;
    }


    std::string VideoWriterParams_fmf::toString()
    {
        std::stringstream ss;
        ss << "frameSkip: " << frameSkip << std::endl;
        return ss.str();
    }

        
    VideoWriterParams_ufmf::VideoWriterParams_ufmf()
    {
        frameSkip = VideoWriter_ufmf::DEFAULT_FRAME_SKIP;
        backgroundThreshold = VideoWriter_ufmf::DEFAULT_BACKGROUND_THRESHOLD;
        boxLength = VideoWriter_ufmf::DEFAULT_BOX_LENGTH;
        medianUpdateCount = BackgroundHistogram_ufmf::DEFAULT_MEDIAN_UPDATE_COUNT; 
        medianUpdateInterval = BackgroundHistogram_ufmf::DEFAULT_MEDIAN_UPDATE_INTERVAL;
        numberOfCompressors = VideoWriter_ufmf::DEFAULT_NUMBER_OF_COMPRESSORS;
        dilateState = VideoWriter_ufmf::DEFAULT_DILATE_STATE;
        dilateWindowSize = VideoWriter_ufmf::DEFAULT_DILATE_WINDOW_SIZE;
    }


    std::string VideoWriterParams_ufmf::toString()
    {
        std::stringstream ss;
        ss << "frameSkip: " << frameSkip << std::endl;
        ss << "backgroundThreshold: " << backgroundThreshold << std::endl;
        ss << "boxLength: " << boxLength << std::endl;
        ss << "meidanUpdateCount: " << medianUpdateCount << std::endl;
        ss << "numberOfCompressors: " << numberOfCompressors << std::endl;
        ss << "dilateState: " << std::boolalpha << dilateState << std::noboolalpha << std::endl;
        ss << "dilateWindowSize: " << dilateWindowSize << std::endl;
        return ss.str();
    }


    std::string VideoWriterParams::toString()
    {
        std::stringstream ss; 
        std::string sepString = "----------------------------------";
        ss << "bmp" << std::endl;
        ss << sepString << std::endl;
        ss << bmp.toString() << std::endl;

        ss << "jpg" << std::endl;
        ss << sepString << std::endl;
        ss << jpg.toString() << std::endl;

        ss << "avi" << std::endl;
        ss << sepString << std::endl;
        ss << avi.toString() << std::endl;

        ss << "fmf" << std::endl;
        ss << sepString << std::endl;
        ss << fmf.toString() << std::endl;

        ss << "ufmf" << std::endl;
        ss << sepString << std::endl;
        ss << ufmf.toString() << std::endl;

        return ss.str();

    }


} // namespace bias
