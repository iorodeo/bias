#include "background_histogram_ufmf.hpp"
#include "stamped_image.hpp"
#include <iostream>

namespace bias
{

    const unsigned int DEFAULT_NUM_BINS = 256;
    const unsigned int DEFAULT_BIN_SIZE = 1;

    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf(QObject *parent) 
        : QObject(parent) 
    {
        initialize(NULL);
    }

    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf (
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr, 
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgImageQueuePtr);
    }

    void BackgroundHistogram_ufmf::initialize( 
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr 
            ) 
    {
        ready_ = false;
        stopped_ = true;
        bgImageQueuePtr_ = bgImageQueuePtr;
        if (bgImageQueuePtr_ != NULL)
        {
            ready_ = true;
        }
    }

    void BackgroundHistogram_ufmf::stop()
    {
        stopped_ = true;
    }

    void BackgroundHistogram_ufmf::run()
    { 
        bool done = false;
        bool isFirst = true;
        bool haveNewImage = false;
        unsigned long frameCount = 0;

        StampedImage newStampedImg;

        if (!ready_) 
        { 
            return; 
        }

        acquireLock();
        stopped_ = false;
        releaseLock();

        while (!done)
        {
            haveNewImage = false;

            // Grab background image from queue
            bgImageQueuePtr_ -> acquireLock();
            bgImageQueuePtr_ -> waitIfEmpty();
            newStampedImg = bgImageQueuePtr_ -> front();
            bgImageQueuePtr_ -> pop();
            haveNewImage = true;
            bgImageQueuePtr_ -> releaseLock();

            // If we have a new image
            if (haveNewImage)
            {
                std::cout << "have new image" << std::endl;
                if (isFirst)
                {
                    createHistogram(newStampedImg);
                    isFirst = false;
                }

                addImageToHistogram(newStampedImg);
            }

            acquireLock();
            done = stopped_;
            releaseLock();
        }

    }

    void BackgroundHistogram_ufmf::createHistogram(StampedImage &stampedImg)
    {
        // Temporary - currently only handle 8bit grayscale images -----------
        numBins_ = DEFAULT_NUM_BINS;
        binSize_ = DEFAULT_BIN_SIZE;
        // -------------------------------------------------------------------
       
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

    void BackgroundHistogram_ufmf::addImageToHistogram(StampedImage &stampedImg)
    {
        unsigned int bin;
        unsigned int pix;
        unsigned int *binPtr = binPtr_.get();
        unsigned long *cntPtr = cntPtr_.get();

        for (unsigned int row=0; row<numRows_; row++)
        {
            for (unsigned int col=0; col<numCols_; col++)
            {
                pix = (unsigned int) (stampedImg.image.at<uchar>(row,col));
                bin = pix/binSize_; 
                *(binPtr + col + numCols_*row + (numRows_*numCols_)*bin) += 1;
                *(cntPtr + col + numCols_*row) += 1;
            }
        }
    }

} // namespace bias
