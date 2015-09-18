#include "compressed_frame_ufmf.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iostream>
#include <QThread>

namespace bias
{
    // Constants 
    // ------------------------------------------------------------------------------
    const uchar CompressedFrame_ufmf::BACKGROUND_MEMBER_VALUE = 255; 
    const uchar CompressedFrame_ufmf::FOREGROUND_MEMBER_VALUE = 0;
    const unsigned int CompressedFrame_ufmf::DEFAULT_BOX_LENGTH = 30; 
    const double CompressedFrame_ufmf::DEFAULT_FG_MAX_FRAC_COMPRESS = 0.2;


    // Methods
    // -------------------------------------------------------------------------------
    CompressedFrame_ufmf::CompressedFrame_ufmf() 
        : CompressedFrame_ufmf(DEFAULT_BOX_LENGTH,DEFAULT_FG_MAX_FRAC_COMPRESS) 
    { }


    CompressedFrame_ufmf::CompressedFrame_ufmf(unsigned int boxLength)
        : CompressedFrame_ufmf(boxLength, DEFAULT_FG_MAX_FRAC_COMPRESS)
    { }


    CompressedFrame_ufmf::CompressedFrame_ufmf(
            unsigned int boxLength, 
            double fgMaxFracCompress
            ) 
    {
        haveData_ = false;
        isCompressed_ = false;
        ready_ = false;
        numPix_ = 0;
        numForeground_ = 0;
        numPixWritten_ = 0;
        numConnectedComp_ = 0;
        boxLength_ = boxLength;
        boxArea_ = boxLength*boxLength;
        fgMaxFracCompress_ = fgMaxFracCompress;
    }


    bool CompressedFrame_ufmf::haveData() const
    {
        return haveData_;
    }

   
    bool CompressedFrame_ufmf::isReady() const
    {
        return ready_;
    }


    double CompressedFrame_ufmf::getTimeStamp() const
    {
        if (haveData_)
        {
            return stampedImg_.timeStamp;
        }
        else
        {
            return 0.0;
        }
    }


    unsigned long CompressedFrame_ufmf::getFrameCount() const
    {
        if (haveData_)
        {
            return stampedImg_.frameCount;
        }
        else 
        {
            return 0;
        }
    }


    unsigned int CompressedFrame_ufmf::getNumConnectedComp() const
    {
        return numConnectedComp_;
    }


    void CompressedFrame_ufmf::dilateEnabled(bool value)
    {
        dilateEnabled_ = true;
    }


    void CompressedFrame_ufmf::setDilateWindowSize(unsigned int value)
    {
        if (value > 0)
        {
            dilateWindowSize_ = value;
        }
        else
        {
            dilateWindowSize_ = 1;
        }
    }


    std::shared_ptr<std::vector<uint16_t>> CompressedFrame_ufmf::getWriteRowBufPtr()
    {
        return writeRowBufPtr_;
    }


    std::shared_ptr<std::vector<uint16_t>> CompressedFrame_ufmf::getWriteColBufPtr()
    {
        return writeColBufPtr_;
    }


    std::shared_ptr<std::vector<uint16_t>> CompressedFrame_ufmf::getWriteHgtBufPtr()
    {
        return writeHgtBufPtr_;
    }


    std::shared_ptr<std::vector<uint16_t>> CompressedFrame_ufmf::getWriteWdtBufPtr()
    {
        return writeWdtBufPtr_;
    }


    std::shared_ptr<std::vector<uint8_t>> CompressedFrame_ufmf::getImageDataPtr()
    {
        return imageDatBufPtr_;
    }


    void CompressedFrame_ufmf::setData(
            StampedImage stampedImg, 
            cv::Mat bgLowerBound,
            cv::Mat bgUpperBound,
            unsigned long bgUpdateCount
            )
    {

        // Save original stamped image
        stampedImg_ = stampedImg;
        bgLowerBound_ = bgLowerBound;
        bgUpperBound_ = bgUpperBound;
        bgUpdateCount_ = bgUpdateCount;
        haveData_ = true;
    }


    void CompressedFrame_ufmf::compress()
    {
        // ---------------------------------------------------------------------
        // NOTE, probably should raise an exception here
        // ---------------------------------------------------------------------
        if (!haveData_) { return; }

        // Get number of rows, cols and pixels from image
        unsigned int numRow = (unsigned int) (stampedImg_.image.rows);
        unsigned int numCol = (unsigned int) (stampedImg_.image.cols);
        unsigned int numPix = numRow*numCol;

        // Allocate memory for compressed frames if required and set buffer values
        if (numPix_ != numPix) 
        {
            numPix_ = numPix;
            allocateBuffers();
        }
        resetBuffers();

        unsigned int fgMaxNumCompress = (unsigned int)(double(numPix)*fgMaxFracCompress_);

        // Get background/foreground membership, 255=background, 0=foreground
        cv::inRange(stampedImg_.image, bgLowerBound_, bgUpperBound_, membershipImage_);
        if (dilateEnabled_)
        {
            cv::Size structElemSize = cv::Size(2*dilateWindowSize_+1,2*dilateWindowSize_+1);
            cv::Mat structElem = cv::getStructuringElement(cv::MORPH_RECT,structElemSize,cv::Point(-1,-1));
            cv::erode(membershipImage_, membershipImage_, structElem, cv::Point(-1,-1),1);
            //cv::erode(membershipImage_, membershipImage_, cv::Mat(), cv::Point(-1,-1),1);
        }

        numForeground_ = numPix - cv::countNonZero(membershipImage_);
        numConnectedComp_ = 0;
        numPixWritten_ = 0;

        // Create frame - uncompressed/compressed based on number of foreground pixels
        if (numForeground_ > fgMaxNumCompress)
        {
            createUncompressedFrame();
        }
        else 
        {
            createCompressedFrame();
        }
        ready_ = true;

    } // CompressedFrame_ufmf::compress


    void CompressedFrame_ufmf::createUncompressedFrame()
    { 
        unsigned int numRow = (unsigned int) (stampedImg_.image.rows);
        unsigned int numCol = (unsigned int) (stampedImg_.image.cols);

        (*writeRowBufPtr_)[0] = 0;
        (*writeColBufPtr_)[0] = 0;
        (*writeHgtBufPtr_)[0] = numRow;
        (*writeWdtBufPtr_)[0] = numCol;

        unsigned int pixCnt = 0;
        for (unsigned int row=0; row<numRow; row++)
        {
            for (unsigned int col=0; col<numCol; col++)
            {
                (*imageDatBufPtr_)[pixCnt] = (uint8_t) stampedImg_.image.at<uchar>(row,col);
                (*numWriteBufPtr_)[pixCnt] = 1;
                pixCnt++;
            }
        }
        numPixWritten_ = numRow*numCol; 
        numConnectedComp_ = 1;
        isCompressed_ = false;

    } // CompressedFrame_ufmf::createUncompressedFrame


    void CompressedFrame_ufmf::createCompressedFrame()
    { 
        // Get pointer to current thread for yeilding computation
        QThread *thisThread = QThread::currentThread();

        // Get number of rows, cols and pixels from image
        unsigned int numRow = (unsigned int) (stampedImg_.image.rows);
        unsigned int numCol = (unsigned int) (stampedImg_.image.cols);
        unsigned int numPix = numRow*numCol;

        isCompressed_ = true;
        numPixWritten_ = 0;
        numConnectedComp_ = 0;
        for (unsigned int i=0; i<numPix; i++) { (*numWriteBufPtr_)[i] = 0; }

        unsigned int imageDatInd = 0;
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
                (*writeRowBufPtr_)[numConnectedComp_] = row;
                (*writeColBufPtr_)[numConnectedComp_] = col;
                (*writeHgtBufPtr_)[numConnectedComp_] = std::min(boxLength_, numRow-row);
                (*writeWdtBufPtr_)[numConnectedComp_] = std::min(boxLength_, numCol-col);

                // Loop through pixels to store
                unsigned int rowPlusHeight = row + (*writeHgtBufPtr_)[numConnectedComp_];

                for (unsigned int rowEnd=row; rowEnd < rowPlusHeight; rowEnd++)
                {
                    
                    bool stopEarly = false;
                    unsigned int colEnd = col;
                    unsigned int numWriteInd = rowEnd*numCol + col;
                    unsigned int colPlusWidth = col + (*writeWdtBufPtr_)[numConnectedComp_];

                    // Check if we've already written something in this column
                    for (colEnd=col; colEnd < colPlusWidth; colEnd++)
                    {
                        if ((*numWriteBufPtr_)[numWriteInd] > 0)
                        {
                            stopEarly = true;
                            break;
                        }
                        numWriteInd += 1;
                    }  // for (unsigned int colEnd 

                    if (stopEarly) 
                    {
                        if (rowEnd == row)
                        { 
                            // If this is the first row - shorten the width and write as usual
                            (*writeWdtBufPtr_)[numConnectedComp_] = colEnd - col;
                        }
                        else
                        {
                            // Otherwise, shorten the height, and don't write any of this row
                            (*writeHgtBufPtr_)[numConnectedComp_] = rowEnd - row;
                            break;
                        }

                    } // if (stopEarly) 

                    colPlusWidth = col + (*writeWdtBufPtr_)[numConnectedComp_];
                    numWriteInd = rowEnd*numCol + col;

                    for (colEnd=col; colEnd < colPlusWidth; colEnd++)
                    {
                        (*numWriteBufPtr_)[numWriteInd] += 1;
                        numWriteInd += 1;

                        (*imageDatBufPtr_)[imageDatInd] = (uint8_t) stampedImg_.image.at<uchar>(rowEnd,colEnd); 
                        imageDatInd += 1;

                        membershipImage_.at<uchar>(rowEnd,colEnd) = BACKGROUND_MEMBER_VALUE;

                    } // for (unsigned int colEnd 


                } // for (unsigned int rowEnd 

                numConnectedComp_++;

                // Yeild to another thread - helps keep frame rate steady
                thisThread -> yieldCurrentThread();

            } // for (unsigned int col


        } // for (unsigned int row

        numPixWritten_ = imageDatInd;

    } // CompressedFrame_ufmf::createCompressedFrame


    //cv::Mat CompressedFrame_ufmf::getMembershipImage()
    //{
    //    return membershipImage_;
    //}


    void CompressedFrame_ufmf::allocateBuffers()
    {
        writeRowBufPtr_ = std::make_shared<std::vector<uint16_t>>();
        writeColBufPtr_ = std::make_shared<std::vector<uint16_t>>();
        writeHgtBufPtr_ = std::make_shared<std::vector<uint16_t>>();
        writeWdtBufPtr_ = std::make_shared<std::vector<uint16_t>>();
        numWriteBufPtr_ = std::make_shared<std::vector<uint16_t>>();
        imageDatBufPtr_ = std::make_shared<std::vector<uint8_t>>();

        writeRowBufPtr_ -> resize(numPix_);
        writeColBufPtr_ -> resize(numPix_);
        writeHgtBufPtr_ -> resize(numPix_);
        writeWdtBufPtr_ -> resize(numPix_);
        numWriteBufPtr_ -> resize(numPix_);
        imageDatBufPtr_ -> resize(numPix_);
    }


    void CompressedFrame_ufmf::resetBuffers()
    {
        std::fill_n(writeRowBufPtr_ -> begin(), numPix_, 0); 
        std::fill_n(writeColBufPtr_ -> begin(), numPix_, 0);
        std::fill_n(writeHgtBufPtr_ -> begin(), numPix_, 0);
        std::fill_n(writeWdtBufPtr_ -> begin(), numPix_, 0);
        std::fill_n(numWriteBufPtr_ -> begin(), numPix_, 0);
        std::fill_n(imageDatBufPtr_ -> begin(), numPix_, 0);
    }


    // Compressed frame comparison operator
    // ----------------------------------------------------------------------------------------
    bool CompressedFrameCmp_ufmf::operator() (
            const CompressedFrame_ufmf &cmpFrame0,
            const CompressedFrame_ufmf &cmpFrame1
            )
    {
        bool haveData0 = cmpFrame0.haveData();
        bool haveData1 = cmpFrame1.haveData();

        if ((haveData0 == false) && (haveData1 == false))
        {
            return false;
        }
        else if ((haveData0 == false) && (haveData1 == true))
        {
            return false;
        }
        else if ((haveData0 == true) && (haveData1 == false))
        {
            return true;
        }
        else
        {
            unsigned long frameCount0 = cmpFrame0.getFrameCount();
            unsigned long frameCount1 = cmpFrame1.getFrameCount();
            return (frameCount0 < frameCount1);
        }
    }

} // namespace bias
