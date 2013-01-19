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
        stampedImg_ = stampedImg;
        cv::inRange(stampedImg.image, bgLowerBound, bgUpperBound, bgMembership_);

    }

    void CompressedFrame_ufmf::allocateBuffers(cv::Size imageSize)
    {
       
        unsigned int numPix = imageSize.width*imageSize.height;

        // Allocate memory for compressed frame buffers
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
        
        // Initial buffer values
        std::fill_n(writeRowBuffer_.get(), numPix, 0); 
        std::fill_n(writeColBuffer_.get(), numPix, 0);
        std::fill_n(writeHeightBuffer_.get(), numPix, 0);
        std::fill_n(writeWidthBuffer_.get(), numPix, 0);
        std::fill_n(numPixWriteBuffer_.get(), numPix, 0);
        std::fill_n(imageDataBuffer_.get(), numPix, 0);


    }

}
