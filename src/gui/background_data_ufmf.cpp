#include "background_data_ufmf.hpp"
#include "stamped_image.hpp"

namespace bias
{ 
    BackgroundData_ufmf::BackgroundData_ufmf()  
    {
        binSize_ = 1;
        numBins_ = 0;
        numRows_ = 0;
        numCols_ = 0;
        binPtr_ = NULL;
        cntPtr_ = NULL;
    }


    BackgroundData_ufmf::BackgroundData_ufmf(
            StampedImage stampedImg, 
            unsigned int numBins, 
            unsigned int binSize
            )
    {
        numBins_ = numBins;
        binSize_ = binSize;
        numRows_ = stampedImg.image.rows;
        numCols_ = stampedImg.image.cols;

        binPtr_ = std::shared_ptr<unsigned int>(
                new unsigned int[numRows_*numCols_*numBins_], 
                std::default_delete<unsigned int[]>()
                );

        cntPtr_ = std::shared_ptr<unsigned long>(
                new unsigned long[numRows_*numCols_],
                std::default_delete<unsigned long[]>()
                );

    }


    void BackgroundData_ufmf::addImage(StampedImage stampedImg)
    {
        unsigned int bin;
        unsigned int pix;
        unsigned int binInd;
        unsigned int cntInd;
        unsigned int  *binPtr = binPtr_.get();
        unsigned long *cntPtr = cntPtr_.get();

        for (unsigned int row=0; row < numRows_; row++)
        {
            for (unsigned int col=0; col< numCols_; col++)
            {
                pix = (unsigned int) (stampedImg.image.at<uchar>(row,col));
                bin = pix/binSize_; 

                binInd = col + numCols_*row + (numRows_*numCols_)*bin;
                *(binPtr + binInd) += 1;

                cntInd = col + numCols_*row;
                *(cntPtr + cntInd) += 1;
            }
        }

    }

} // namespace bias
