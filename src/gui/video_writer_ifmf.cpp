#include "video_writer_ifmf.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <stdint.h>
#include <stdexcept>

namespace bias
{
    const unsigned int VideoWriter_ifmf::DEFAULT_FRAME_SKIP = 2;
    const unsigned int VideoWriter_ifmf::FMF_VERSION = 1;
    const QString DUMMY_FILENAME("dummy.ifmf");

    VideoWriter_ifmf::VideoWriter_ifmf(QObject *parent) 
        : VideoWriter_ifmf(DUMMY_FILENAME, parent) 
    {}

    VideoWriter_ifmf::VideoWriter_ifmf(QString fileName, QObject *parent) 
        : VideoWriter(fileName, parent)
    {
        numWritten_ = 0;
        isFirst_ = true;
        rowOffset_ = 0;
        colOffset_ = 0;
        setFrameSkip(DEFAULT_FRAME_SKIP);
    }

    VideoWriter_ifmf::~VideoWriter_ifmf()
    {
        file_.close();
    }

    void VideoWriter_ifmf::finish()
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

    void VideoWriter_ifmf::addFrame(StampedImage stampedImg)
    {
        if (isFirst_)
        {
            setupOutput(stampedImg);
            isFirst_ = false;
            buffer_.create(stampedImg.image.rows, stampedImg.image.cols,CV_8UC1);
        }
        if (frameCount_%frameSkip_==0)
        {
            //std::cout << "write" << std::endl;

            // Fill data buffer with every other pixel
            uint8_t *imgPtr = (uint8_t*) stampedImg.image.data;
            uint8_t *bufPtr = (uint8_t*) buffer_.data;
            unsigned int imgInd;
            unsigned int bufInd;
            for (unsigned int i=rowOffset_; i<stampedImg.image.rows; i+=2)
            {
                colOffset_ = (rowOffset_ + i)%2;
                for (unsigned int j=colOffset_; j<stampedImg.image.cols; j+=2)
                {
                    imgInd = i*stampedImg.image.step + j;
                    *(bufPtr + imgInd) = *(imgPtr + imgInd);
                }
            }

            // Every other frame - write time stamp and buffer data to file
            if (rowOffset_ == 1) 
            {
                try
                {
                    file_.write((char*) &stampedImg.timeStamp, sizeof(double));
                    file_.write((char*) buffer_.data, (buffer_.rows*buffer_.cols)*sizeof(char)); 
                }
                catch (std::ifstream::failure &exc)
                {
                    unsigned int errorId = ERROR_VIDEO_WRITER_ADD_FRAME;
                    std::string errorMsg("video writer add frame failed:\n\n"); 
                    errorMsg += exc.what();
                    throw RuntimeError(errorId, errorMsg); 
                }
            }

            //std::cout << rowOffset_ << std::endl;
            rowOffset_ = (rowOffset_ + 1)%2;
            numWritten_++;
        }
        else 
        {
            //std::cout << "skip" << std::endl;
        }
        frameCount_++;
    }

    void VideoWriter_ifmf::setupOutput(StampedImage stampedImg)
    {
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
            std::string errorMsg("video writer unable to write ifmf header:\n\n"); 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg); 
        }

    }


} // namespace bias
