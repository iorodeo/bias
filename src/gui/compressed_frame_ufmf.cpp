#include "compressed_frame_ufmf.hpp"
#include <algorithm>

namespace bias
{
    // Constants 
    // ------------------------------------------------------------------------------
    const uchar CompressedFrame_ufmf::BACKGROUND_MEMBER_VALUE = 255; 
    const uchar CompressedFrame_ufmf::FOREGROUND_MEMBER_VALUE = 0;
    const unsigned int CompressedFrame_ufmf::DEFAULT_BOX_LENGTH = 30; 
    const double CompressedFrame_ufmf::DEFAULT_FG_MAX_FRAC_COMPRESS = 1.0;

    // Methods
    // -------------------------------------------------------------------------------
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
        numConnectedComp_ = 0;
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
        unsigned int numRow = (unsigned int) (stampedImg.image.rows);
        unsigned int numCol = (unsigned int) (stampedImg.image.cols);
        unsigned int numPix = numRow*numCol;

        // Save original stamped image
        stampedImg_ = stampedImg;

        // Allocate memory for compressed frames if require
        if (numPix_ != numPix) 
        {
            numPix_ = numPix;
            allocateBuffers();
        }

        // Set initial values
        unsigned int fgMaxNumCompress = (unsigned int)(double(numPix)*fgMaxFracCompress_);
        numForeground_ = 0;
        numPixWritten_ = 0;
        numConnectedComp_ = 0;
        resetBuffers();

        // Get background/foreground membership, 255=background, 0=foreground
        cv::inRange(stampedImg.image, bgLowerBound, bgUpperBound, membershipImage_);
        unsigned int numForeground = numPix - cv::countNonZero(membershipImage_);

        // Create frame - uncompressed/compressed based on number of foreground pixels
        if (numForeground > fgMaxNumCompress)
        {
            // Create uncompressed image - too many pixels in foreground
            // --------------------------------------------------------------------------
            writeRowBuf_[0] = 0;
            writeColBuf_[0] = 0;
            writeHgtBuf_[0] = numRow;
            writeWdtBuf_[0] = numCol;

            unsigned int pixCnt = 0;
            for (unsigned int row=0; row<numRow; row++)
            {
                for (unsigned int col=0; col<numCol; col++)
                {
                    imageDatBuf_[pixCnt] = (uint8_t) stampedImg.image.at<uchar>(row,col);
                    numWriteBuf_[pixCnt] = 1;
                    pixCnt++;
                }
            }
            numPixWritten_ = numPix;
            numConnectedComp_ = 1;
            isCompressed_ = false;
        }
        else 
        {
            // Compressed image
            // --------------------------------------------------------------------------
            bool stopEarly = false;

            for (unsigned int row=0; row<numRow; row++)
            {
                for (unsigned int col=0; col<numCol; col++)
                {
                    // Start new box if pixel if foreground or continue to next pixel
                    if (membershipImage_.at<uchar>(row,col) == BACKGROUND_MEMBER_VALUE) 
                    { 
                        continue;
                    }

                    // store everything in box with corner at (row,col)
                    writeRowBuf_[numConnectedComp_] = row;
                    writeColBuf_[numConnectedComp_] = col;
                    writeHgtBuf_[numConnectedComp_] = std::min(boxLength_, numRow-row);
                    writeWdtBuf_[numConnectedComp_] = std::min(boxLength_, numCol-col);

                    // Loop through pixels to store


                    // end loop
                    numConnectedComp_++;
                }
            }

            isCompressed = true;
        }

        haveData_ = true;
    }

    cv::Mat CompressedFrame_ufmf::getMembershipImage()
    {
        return membershipImage_;
    }

    void CompressedFrame_ufmf::allocateBuffers()
    {
        writeRowBuf_.resize(numPix_);
        writeColBuf_.resize(numPix_);
        writeHgtBuf_.resize(numPix_);
        writeWdtBuf_.resize(numPix_);
        numWriteBuf_.resize(numPix_);
        imageDatBuf_.resize(numPix_);
        
    }

    void CompressedFrame_ufmf::resetBuffers()
    {
        std::fill_n(writeRowBuf_.begin(), numPix_, 0); 
        std::fill_n(writeColBuf_.begin(), numPix_, 0);
        std::fill_n(writeHgtBuf_.begin(), numPix_, 0);
        std::fill_n(writeWdtBuf_.begin(), numPix_, 0);
        std::fill_n(numWriteBuf_.begin(), numPix_, 0);
        std::fill_n(imageDatBuf_.begin(), numPix_, 0);
    }

}
