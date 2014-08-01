#include "background_histogram_ufmf.hpp"
#include "background_data_ufmf.hpp"
#include "stamped_image.hpp"
#include "affinity.hpp"
#include <QThread>
#include <iostream>

namespace bias
{

    // Static constants
    const unsigned int BackgroundHistogram_ufmf::DEFAULT_NUM_BINS = 256;
    const unsigned int BackgroundHistogram_ufmf::DEFAULT_BIN_SIZE = 1;
    const unsigned int BackgroundHistogram_ufmf::DEFAULT_MEDIAN_UPDATE_COUNT = 100;
    const unsigned int BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_COUNT = 10;
    const unsigned int BackgroundHistogram_ufmf::DEFAULT_MEDIAN_UPDATE_INTERVAL = 50;
    const unsigned int BackgroundHistogram_ufmf::MIN_MEDIAN_UPDATE_INTERVAL = 1;


    // Methods
    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf(QObject *parent) 
        : QObject(parent) 
    {
        initialize(NULL,NULL,NULL,0);
    }


    BackgroundHistogram_ufmf::BackgroundHistogram_ufmf (
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr, 
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
            unsigned int cameraNumber,
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgImageQueuePtr, bgNewDataQueuePtr, bgOldDataQueuePtr,cameraNumber);
    }


    void BackgroundHistogram_ufmf::initialize( 
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
            unsigned int cameraNumber
            ) 
    {
        ready_ = false;
        stopped_ = true;
        bgImageQueuePtr_ = bgImageQueuePtr;
        bgNewDataQueuePtr_ = bgNewDataQueuePtr;
        bgOldDataQueuePtr_ = bgOldDataQueuePtr;
        medianUpdateCount_ = DEFAULT_MEDIAN_UPDATE_COUNT;
        medianUpdateInterval_ = DEFAULT_MEDIAN_UPDATE_INTERVAL;

        // Make sure none of the data queue pointers are null
        bool notNull = true;
        notNull &= (bgImageQueuePtr_   != NULL);
        notNull &= (bgNewDataQueuePtr_ != NULL);
        notNull &= (bgOldDataQueuePtr_ != NULL);

        if (notNull) 
        {
            ready_ = true;
        }
        cameraNumber_ = cameraNumber;
    }


    void BackgroundHistogram_ufmf::stop()
    {
        stopped_ = true;
    }


    void BackgroundHistogram_ufmf::setMedianUpdateCount(unsigned int medianUpdateCount)
    {
        medianUpdateCount_ = medianUpdateCount;
    }


    void BackgroundHistogram_ufmf::setMedianUpdateInterval(unsigned int medianUpdateInterval)
    {
        medianUpdateInterval_ = medianUpdateInterval;
    }


    void BackgroundHistogram_ufmf::run()
    { 
        bool done = false;
        bool isFirst = true;
        unsigned long count = 0;
        double lastUpdateTime;
        double updateDt;

        BackgroundData_ufmf backgroundData;

        StampedImage newStampedImg;

        if (!ready_) 
        { 
            return; 
        }

        // Set thread priority to idle - only run when no other thread are running
        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::NormalPriority);
        ThreadAffinityService::assignThreadAffinity(false,cameraNumber_);

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
                lastUpdateTime = newStampedImg.timeStamp - medianUpdateInterval_;
                isFirst = false;
            }

            // Add image to background data
            backgroundData.addImage(newStampedImg);
            count++;

            // Check to see if median computation is done if so swap the buffers
            updateDt = newStampedImg.timeStamp - lastUpdateTime;
            if ( (count > medianUpdateCount_) && (updateDt > medianUpdateInterval_))
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
                    lastUpdateTime = newStampedImg.timeStamp;
                    backgroundData.clear();
                }
            }

            acquireLock();
            done = stopped_;
            releaseLock();
        }

    }


} // namespace bias
