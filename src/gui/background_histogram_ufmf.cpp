#include "background_histogram_ufmf.hpp"
#include "background_data_ufmf.hpp"
#include "stamped_image.hpp"
#include <iostream>

namespace bias
{

    // Temporary
    // ------------------------------------------------
    const unsigned int DEFAULT_NUM_BINS = 256;
    const unsigned int DEFAULT_BIN_SIZE = 1;
    const unsigned int DEFAULT_MIN_UPDATE_COUNT = 100;
    // -------------------------------------------------


    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf(QObject *parent) 
        : QObject(parent) 
    {
        initialize(NULL,NULL,NULL);
    }


    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf (
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr, 
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgImageQueuePtr, bgNewDataQueuePtr, bgOldDataQueuePtr);
    }


    void BackgroundHistogram_ufmf::initialize( 
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr
            ) 
    {
        ready_ = false;
        stopped_ = true;
        bgImageQueuePtr_ = bgImageQueuePtr;
        bgNewDataQueuePtr_ = bgNewDataQueuePtr;
        bgOldDataQueuePtr_ = bgOldDataQueuePtr;

        // Make sure none of the data queue pointers are null
        bool notNull = true;
        notNull &= (bgImageQueuePtr_   != NULL);
        notNull &= (bgNewDataQueuePtr_ != NULL);
        notNull &= (bgOldDataQueuePtr_ != NULL);

        if (notNull) 
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
        unsigned long count = 0;
        BackgroundData_ufmf backgroundData;

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
            // Grab background image from queue
            bgImageQueuePtr_ -> acquireLock();
            bgImageQueuePtr_ -> waitIfEmpty();
            if ((bgImageQueuePtr_ -> empty()))
            {
                bgImageQueuePtr_ -> releaseLock();
                break;
            }
            newStampedImg = bgImageQueuePtr_ -> front();
            bgImageQueuePtr_ -> pop();
            bgImageQueuePtr_ -> releaseLock();

            //std::cout << "* new bg image, count = " << count << std::endl;
            if (isFirst)
            {
                // Create two new background data objects - put one in old data queue.
                for (int i=0; i<2; i++) 
                {
                    backgroundData = BackgroundData_ufmf(
                            newStampedImg,
                            DEFAULT_NUM_BINS,
                            DEFAULT_BIN_SIZE
                            );
                    if (i==0)
                    {
                        bgOldDataQueuePtr_ -> acquireLock();
                        bgOldDataQueuePtr_ -> push(backgroundData);
                        bgOldDataQueuePtr_ -> releaseLock();
                    }
                }

                isFirst = false;
            }

            // Add image to background data
            backgroundData.addImage(newStampedImg);
            count++;

            // Check to see if median computation is done if so swap the buffers
            if (count > DEFAULT_MIN_UPDATE_COUNT)
            {
                bool swapDataFlag = false;
                BackgroundData_ufmf backgroundDataTmp;

                bgOldDataQueuePtr_ -> acquireLock();
                if (!(bgOldDataQueuePtr_ -> empty()))
                {
                    backgroundDataTmp = bgOldDataQueuePtr_ -> front();
                    bgOldDataQueuePtr_ -> pop();
                    swapDataFlag = true;
                }
                bgOldDataQueuePtr_ -> releaseLock();

                if (swapDataFlag)
                {
                    bgNewDataQueuePtr_ -> acquireLock();
                    bgNewDataQueuePtr_ -> push(backgroundData);
                    bgNewDataQueuePtr_ -> signalNotEmpty();
                    bgNewDataQueuePtr_ -> releaseLock();
                    backgroundData = backgroundDataTmp;

                    // Clear out old data
                    count = 0;
                    backgroundData.clear();
                }
            }

            acquireLock();
            done = stopped_;
            releaseLock();
        }

    }


} // namespace bias
