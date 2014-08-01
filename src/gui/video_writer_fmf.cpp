#include "video_writer_fmf.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <stdint.h>
#include <stdexcept>

namespace bias
{
    const unsigned int VideoWriter_fmf::DEFAULT_FRAME_SKIP = 1;
    const unsigned int VideoWriter_fmf::FMF_VERSION = 1;
    const QString DUMMY_FILENAME("dummy.fmf");
    const VideoWriterParams_fmf VideoWriter_fmf::DEFAULT_PARAMS =
        VideoWriterParams_fmf();

    VideoWriter_fmf::VideoWriter_fmf(QObject *parent) 
        : VideoWriter_fmf(DEFAULT_PARAMS, DUMMY_FILENAME, 0, parent) 
    {}

    VideoWriter_fmf::VideoWriter_fmf(
            VideoWriterParams_fmf params,
            QString fileName, 
            unsigned int cameraNumber,
            QObject *parent
            ) : VideoWriter(fileName, cameraNumber, parent)
    {
        numWritten_ = 0;
        isFirst_ = true;
        setFrameSkip(params.frameSkip);
    }

    VideoWriter_fmf::~VideoWriter_fmf()
    {
        file_.close();
    }

    void VideoWriter_fmf::finish()
    {
        try
        {
            file_.seekp(20);
            file_.write((char*) &numWritten_, sizeof(uint64_t));
        }
        catch (std::ifstream::failure &exc)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_FINISH;
            std::string errorMsg("video writer finish - failed");
            errorMsg +=  "to write number frames:\n\n"; 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg); 
        }
    }

    void VideoWriter_fmf::addFrame(StampedImage stampedImg)
    {
        if (isFirst_)
        {
            setupOutput(stampedImg);
            isFirst_ = false;
        }
        if (frameCount_%frameSkip_==0)
        {
            try
            {
                file_.write((char*) &stampedImg.timeStamp, sizeof(double));
                file_.write((char*) stampedImg.image.data, size_.width*size_.height*sizeof(char)); 
            }
            catch (std::ifstream::failure &exc)
            {
                unsigned int errorId = ERROR_VIDEO_WRITER_ADD_FRAME;
                std::string errorMsg("video writer add frame failed:\n\n"); 
                errorMsg += exc.what();
                throw RuntimeError(errorId, errorMsg); 
            }
            numWritten_++;
        }
        else 
        {
            //std::cout << "skip" << std::endl;
        }
        frameCount_++;
    }

    void VideoWriter_fmf::setupOutput(StampedImage stampedImg)
    {
        // Check image format - must be CV_8UC1
        if (stampedImg.image.channels() != 1)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer fmf setup failed:\n\n"); 
            errorMsg += "images must be single channel";
            throw RuntimeError(errorId,errorMsg);
        }

        if (stampedImg.image.depth() != CV_8U)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer fmf setup failed:\n\n"); 
            errorMsg += "image depth must be CV_8U";
            throw RuntimeError(errorId,errorMsg);
        }

        // Set error control state, set exceptions mask
        file_.clear();
        file_.exceptions(std::ifstream::failbit | std::ifstream::badbit);
       
        // Get unique name for file and open for reading
        QString incrFileName = getUniqueFileName();

        try
        {
            file_.open(incrFileName.toStdString(), std::ios::binary | std::ios::out);
        }
        catch (std::ifstream::failure &exc)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg); 
        }

        if (!file_.is_open())
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += "no exception thrown";
            throw RuntimeError(errorId, errorMsg); 
        }

        setSize(stampedImg.image.size());

        // Cast values to integers with specific widths
        uint32_t fmfVersion = uint32_t(FMF_VERSION);
        uint32_t width = uint32_t(size_.width);
        uint32_t height = uint32_t(size_.height);
        uint64_t bytesPerChunk = uint64_t(width)*uint64_t(height) + sizeof(double);

        // Add fmf header to file
        try 
        {
            file_.write((char*) &fmfVersion, sizeof(uint32_t));
            file_.write((char*) &height, sizeof(uint32_t));
            file_.write((char*) &width, sizeof(uint32_t));
            file_.write((char*) &bytesPerChunk, sizeof(uint64_t));
            file_.write((char*) &numWritten_, sizeof(uint64_t));
        }
        catch (std::ifstream::failure &exc)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to write fmf header:\n\n"); 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg); 
        }

    }


} // namespace bias
