#include "background_data_ufmf.hpp"
#include "stamped_image.hpp"
#include <cstring>

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
        clear();
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
                cntInd = col + numCols_*row;

                *(binPtr + binInd) += 1;
                *(cntPtr + cntInd) += 1;
            } 
        } 
    }


    std::shared_ptr<float> BackgroundData_ufmf::getMedians()
    {
        unsigned int bin;
        unsigned int binValue;
        unsigned int *binPtr = binPtr_.get();

        unsigned long cntTotal;
        unsigned long cntHalf;
        unsigned long cntCurrent;
        unsigned long *cntPtr = cntPtr_.get();

        float medianScale = float(binSize_);
        float medianShift = (medianScale - 1.0)/2.0;
        float median;

        std::shared_ptr<float> medianPtrShared = std::shared_ptr<float>(
                new float[numRows_*numCols_],
                std::default_delete<float[]>()
                );

        float *medianPtr = medianPtrShared.get();  

        for (unsigned int row=0; row<numRows_; row++)
        {
            for (unsigned int col=0; col<numCols_; col++)
            {
                // Get total and half total # of counts for current pixel
                cntTotal = *(cntPtr + col + numCols_*row);
                cntHalf = cntTotal/2;

                // Find first bin such that at least half of all counts are in a 
                // bin with a value smaller than of equal to itself. 
                for (bin=0,cntCurrent=0; bin<numBins_, cntCurrent<=cntHalf; bin++)
                {
                    binValue = *(binPtr + col + numCols_*row + (numRows_*numCols_)*bin);  
                    cntCurrent += binValue;
                }

                // Compute the median bin value
                if ((cntTotal%2!=0) || ((cntHalf-(cntCurrent-binValue)) > 1))
                {
                    median = float(bin);
                }
                else
                {
                    median = (float(bin)-0.5);
                }

                // Adjust to get the median pixal value
                median = medianScale*median + medianShift;
                *(medianPtr + col + numCols_*row) = median;

            } // for j

        } // for i

        return medianPtrShared;
    }

    void BackgroundData_ufmf::clear()
    {
        memset(binPtr_.get(), 0, numRows_*numCols_*numBins_*sizeof(unsigned int));
        memset(cntPtr_.get(), 0, numRows_*numCols_*sizeof(unsigned long));
    }

} // namespace bias
