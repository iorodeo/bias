#include "background_histogram_ufmf.hpp"
#include "background_data_ufmf.hpp"
#include "stamped_image.hpp"
#include <iostream>

namespace bias
{

    // Temporary
    // ---------------------------------------
    const unsigned int DEFAULT_NUM_BINS = 256;
    const unsigned int DEFAULT_BIN_SIZE = 1;
    // ---------------------------------------


    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf(QObject *parent) 
        : QObject(parent) 
    {
        initialize(NULL,NULL);
    }


    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf (
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr, 
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr,
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgImageQueuePtr, bgDataQueuePtr);
    }


    void BackgroundHistogram_ufmf::initialize( 
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr
            ) 
    {
        ready_ = false;
        stopped_ = true;
        bgImageQueuePtr_ = bgImageQueuePtr;
        bgDataQueuePtr_ = bgDataQueuePtr;
        if ((bgImageQueuePtr_ != NULL) && (bgDataQueuePtr_ != NULL))
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
        unsigned long frameCount = 0;
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

            std::cout << "have new image" << std::endl;
            if (isFirst)
            {
                // Create new Background data object
                backgroundData = BackgroundData_ufmf(
                        newStampedImg,
                        DEFAULT_NUM_BINS,
                        DEFAULT_BIN_SIZE
                        );
                isFirst = false;
            }

            // Add image to background data
            backgroundData.addImage(newStampedImg);

            acquireLock();
            done = stopped_;
            releaseLock();
        }

    }


} // namespace bias
