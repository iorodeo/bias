#include "background_median_ufmf.hpp"
#include "background_data_ufmf.hpp"
#include "affinity.hpp"
#include <iostream>
#include <QThread>
#include <opencv2/core/core.hpp>

namespace bias
{ 
    BackgroundMedian_ufmf::BackgroundMedian_ufmf(QObject *parent)
        : QObject(parent)
    { 
        initialize(NULL,NULL,NULL,0);
    }


    BackgroundMedian_ufmf::BackgroundMedian_ufmf( 
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
            std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr,
            unsigned int cameraNumber,
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgNewDataQueuePtr,bgOldDataQueuePtr,medianMatQueuePtr,cameraNumber);
    }


    void BackgroundMedian_ufmf::initialize(
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
            std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr,
            unsigned int cameraNumber
            )
    {
        ready_ = false;
        stopped_ = true;
        bgNewDataQueuePtr_ = bgNewDataQueuePtr;
        bgOldDataQueuePtr_ = bgOldDataQueuePtr;
        medianMatQueuePtr_ = medianMatQueuePtr;

        bool notNull = true;
        notNull &= (bgNewDataQueuePtr_ != NULL);
        notNull &= (bgOldDataQueuePtr_ != NULL);
        notNull &= (medianMatQueuePtr_ != NULL);
        if (notNull)
        {
            ready_ = true;
        }
        cameraNumber_ = cameraNumber;
    }


    void BackgroundMedian_ufmf::stop()
    {
        stopped_ = true;
    }


    void BackgroundMedian_ufmf::run()
    {
        bool done = false;
        bool isFirst = true;
        BackgroundData_ufmf backgroundData;
        cv::Mat medianImage;

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

        while(!done)
        {
            // Grab background data from queue
            bgNewDataQueuePtr_ -> acquireLock();
            bgNewDataQueuePtr_ -> waitIfEmpty();
            if (bgNewDataQueuePtr_ -> empty())
            {
                bgNewDataQueuePtr_ -> releaseLock();
                break;
            }
            backgroundData = bgNewDataQueuePtr_ -> front();
            bgNewDataQueuePtr_ -> pop();
            bgNewDataQueuePtr_ -> releaseLock();

            //std::cout << "*** new median data" << std::endl;

            // Compute median
            medianImage = backgroundData.getMedianImage();

            medianMatQueuePtr_ -> acquireLock();
            medianMatQueuePtr_ -> push(medianImage);
            medianMatQueuePtr_ -> releaseLock();

            // Put data back into outgoing queue 
            bgOldDataQueuePtr_ -> acquireLock();
            bgOldDataQueuePtr_ -> push(backgroundData);
            bgOldDataQueuePtr_ -> releaseLock();


            acquireLock();
            done = stopped_;
            releaseLock();

        } // while(!done)
    }


} // namespace bias
