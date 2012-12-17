#include "background_model_ufmf.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <cmath>
#include <iostream>

namespace bias
{ 
    // BackgroundHistogram
    // -----------------------------------------------------------------------------
    BackgroundHistogram::BackgroundHistogram()
    {
        numRows_ = 0;
        numCols_ = 0;
        numBins_ = 0;
        valueCount_ = 0;
    }
    

    BackgroundHistogram::BackgroundHistogram(
            unsigned int numRows,
            unsigned int numCols,
            unsigned int numBins
            )
    {
        numRows_ = numRows;
        numCols_ = numCols;
        numBins_ = numBins;
        dataPtr_ = std::shared_ptr<unsigned int>(
                new unsigned int[numRows_*numCols_*numBins_], 
                std::default_delete<unsigned int[]>()
                );
        totalPtr_ = std::shared_ptr<unsigned long>(
                new unsigned long[numRows_*numCols_],
                std::default_delete<unsigned long[]>()
                );
        setValuesToZero();
    }


    BackgroundHistogram::~BackgroundHistogram() { }


    unsigned int BackgroundHistogram::getValue(
            unsigned int row, 
            unsigned int col, 
            unsigned int bin
            )
    {
        return *(dataPtr_.get() + col + numCols_*row + (numRows_*numCols_)*bin);
    }


    void BackgroundHistogram::incrementData(
            unsigned int row, 
            unsigned int col, 
            unsigned int bin
            )
    {
        *(dataPtr_.get() + col + numCols_*row + (numRows_*numCols_)*bin) += 1;
        *(totalPtr_.get() + col + numCols_*row) += 1;
    }

    unsigned long BackgroundHistogram::getTotal(unsigned int row, unsigned int col)
    {
        return *(totalPtr_.get() + col + numCols_*row);
    }


    //float BackgroundHistogram::getMedianValue(unsigned int row, unsigned int col)
    //{
    //    unsigned long total = getTotal(row,col);
    //    unsigned long halfTotal = total/2;
    //    unsigned int bin;
    //    unsigned int binVal;
    //    unsigned long cntCur;
    //    for (bin=0,cntCur=0; bin<numBins_, cntCur<=halfTotal; bin++)
    //    {
    //        binVal = getValue(row,col,bin);
    //        cntCur += binVal;
    //    }
    //    if ((total%2!=0) || (binVal > 1))
    //    {
    //        return float(bin);

    //    }
    //    else
    //    {
    //        return float(bin)-0.5;
    //    }
    //}
    //

    void BackgroundHistogram::getMedianValues()
    {
        unsigned int bin;
        unsigned int binVal;
        unsigned long total;
        unsigned long halfTotal;
        unsigned long *totalPtr = totalPtr_.get();
        unsigned int *dataPtr = dataPtr_.get();
        float temp;

        for (unsigned int i=0; i<numRows_; i++)
        {
            for (unsigned int j=0; j<numCols_; j++)
            {
                unsigned long total = *(totalPtr + j + numCols_*i);
                unsigned long halfTotal = total/2;
                unsigned int bin;
                unsigned int binVal;
                unsigned long cntCur;
                for (bin=0,cntCur=0; bin<numBins_, cntCur<=halfTotal; bin++)
                {
                    binVal = *(dataPtr + j+ numCols_*i + (numRows_*numCols_)*bin);  
                    cntCur += binVal;
                }
                if ((total%2!=0) || (binVal > 1))
                {
                    temp = float(bin);

                }
                else
                {
                    temp = float(bin)-0.5;
                }

            }
        }

    }

    void BackgroundHistogram::setValuesToZero()
    {
        memset(dataPtr_.get(),0,numRows_*numCols_*numBins_*sizeof(unsigned int));
        memset(totalPtr_.get(),0,numRows_*numCols_*sizeof(unsigned long));
    }



    // BackgroundModel_ufmf
    // ----------------------------------------------------------------------

    const unsigned int BackgroundModel_ufmf::DEFAULT_BIN_SIZE_CV_8U = 1;

    BackgroundModel_ufmf::BackgroundModel_ufmf()
    {
        frameCnt_ = 0;
        isFirst_ = true;
    }


    BackgroundModel_ufmf::~BackgroundModel_ufmf() {}


    void BackgroundModel_ufmf::addFrame(StampedImage stampedImg)
    {
        if (isFirst_ == true)
        {
            initialize(stampedImg);
            isFirst_ = false;
        }

        if (frameCnt_%50==0) 
        {
            std::cout << "add background frame" << std::endl;

            unsigned int pixVal;
            unsigned int binNum;

            for (unsigned int i=0; i< stampedImg.image.rows; i++)
            {
                for (unsigned int j=0; j< stampedImg.image.cols; j++)
                {
                    pixVal = (unsigned int) (stampedImg.image.at<uchar>(i,j));
                    binNum = pixVal/binSize_; 
                    histogram_.incrementData(i,j,binNum);
                }
            }
        }

        //if ((frameCnt_ > 1) && (frameCnt_%3000==0))
        //{
        //    std::cout << "computing median" << std::endl;
        //    histogram_.getMedianValues();
        //}
        frameCnt_++;
    }


    void BackgroundModel_ufmf::initialize(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        if (stampedImg.image.channels() != 1)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("ufmf background model setup failed:\n\n"); 
            errorMsg += "images must be single channel";
            throw RuntimeError(errorId,errorMsg);
        }

        imageType_ = stampedImg.image.type();
        imageDepth_ = stampedImg.image.depth();
        imageSize_ = stampedImg.image.size();

        // Currently only allow CV_8U images
        if (imageDepth_ != CV_8U) 
        { 
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("ufmf background model setup failed:\n\n"); 
            errorMsg += "image depth must be CV_8U or CV_16U";
            throw RuntimeError(errorId,errorMsg);
        }
        binSize_ = DEFAULT_BIN_SIZE_CV_8U;
        numberOfBins_ = (unsigned int)(ceil(256.0/float(binSize_)));

        // Create background histogram
        unsigned int rows = stampedImg.image.rows;
        unsigned int cols = stampedImg.image.cols;
        histogram_ = BackgroundHistogram(rows,cols,numberOfBins_);

    }

} // namespace bias
