#include "compressed_frame_ufmf.hpp"
#include <algorithm>

namespace bias
{
    const unsigned int CompressedFrame_ufmf::DEFAULT_BOX_LENGTH = 30; 
    const double CompressedFrame_ufmf::DEFAULT_FG_MAX_FRAC_COMPRESS = 1.0;

    CompressedFrame_ufmf::CompressedFrame_ufmf() 
        : CompressedFrame_ufmf(DEFAULT_BOX_LENGTH, DEFAULT_FG_MAX_FRAC_COMPRESS)
    { }

    CompressedFrame_ufmf::CompressedFrame_ufmf(
            unsigned int boxLength, 
            double fgMaxFracCompress
            )
    {
        haveData_ = false;
        isCompressed_ = false;
        numPix_ = 0;
        numForeground_ = 0;
        numPixWritten_ = 0;
        boxLength_ = boxLength;
        boxArea_ = boxLength*boxLength;
        fgMaxFracCompress_ = fgMaxFracCompress;
    }

    void CompressedFrame_ufmf::setData(
            StampedImage stampedImg, 
            cv::Mat bgLowerBound,
            cv::Mat bgUpperBound
            )
    {
        // Get number of rows, cols and pixels from image
        unsigned int numRow = stampedImg.image.rows;
        unsigned int numCol = stampedImg.image.cols;
        unsigned int numPix = numRow*numCol;
        unsigned int fgMaxNumCompress = (unsigned int)(double(numPix)*fgMaxFracCompress_);

        // Save original stamped image
        stampedImg_ = stampedImg;

        // Allocate memory for compressed frames and set initial values
        if (numPix_ != numPix) 
        {
            allocateBuffers();
            numPix_ = numPix;
        }
        resetBuffers();

        // Get background/foreground membership, 255=background, 0=foreground
        cv::inRange(stampedImg.image, bgLowerBound, bgUpperBound, bgMembershipImage_);
        unsigned int numForeground = numPix - cv::countNonZero(bgMembershipImage_);
        
    }

    cv::Mat CompressedFrame_ufmf::getMembershipImage()
    {
        return bgMembershipImage_;
    }

    void CompressedFrame_ufmf::allocateBuffers()
    {
        // Get number of rows, cols and pixels from image
        unsigned int numRow = stampedImg_.image.rows;
        unsigned int numCol = stampedImg_.image.cols;
        unsigned int numPix = numRow*numCol;

        // --------------------------------------------------------------------------
        // Allocate memory for compressed frame buffers - don't need this because its 
        // allocated as needed in inRange ???
        //bgMembershipImage_.create(numRow,numCol,CV_8UC1);
        // --------------------------------------------------------------------------

        isForeground_ = std::shared_ptr<bool>(
                new bool[numPix],
                std::default_delete<bool[]>()
                );

        writeRowBuffer_ = std::shared_ptr<uint16_t>(
                new uint16_t[numPix], 
                std::default_delete<uint16_t[]>()
                );

        writeColBuffer_ = std::shared_ptr<uint16_t>( 
                new uint16_t[numPix], 
                std::default_delete<uint16_t[]>()
                ); 

        writeHeightBuffer_ = std::shared_ptr<uint16_t>( 
                new uint16_t[numPix], 
                std::default_delete<uint16_t[]>()
                ); 

        writeWidthBuffer_ = std::shared_ptr<uint16_t>( 
                new uint16_t[numPix], 
                std::default_delete<uint16_t[]>()
                ); 

        numPixWriteBuffer_ = std::shared_ptr<uint16_t>( 
                new uint16_t[numPix], 
                std::default_delete<uint16_t[]>()
                ); 

        imageDataBuffer_ = std::shared_ptr<uint8_t>( 
                new uint8_t[numPix], 
                std::default_delete<uint8_t[]>()
                ); 
        
    }

    void CompressedFrame_ufmf::resetBuffers()
    {
        unsigned int numPix = stampedImg_.image.rows*stampedImg_.image.cols;
        std::fill_n(isForeground_.get(),numPix, false);
        std::fill_n(writeRowBuffer_.get(), numPix, 0); 
        std::fill_n(writeColBuffer_.get(), numPix, 0);
        std::fill_n(writeHeightBuffer_.get(), numPix, 0);
        std::fill_n(writeWidthBuffer_.get(), numPix, 0);
        std::fill_n(numPixWriteBuffer_.get(), numPix, 0);
        std::fill_n(imageDataBuffer_.get(), numPix, 0);
    }

}
