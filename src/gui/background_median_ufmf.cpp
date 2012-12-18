#include "background_median_ufmf.hpp"
#include "background_data_ufmf.hpp"
#include <iostream>

namespace bias
{ 
    BackgroundMedian_ufmf::BackgroundMedian_ufmf(QObject *parent)
        : QObject(parent)
    { 
        initialize(NULL,NULL);
    }


    BackgroundMedian_ufmf::BackgroundMedian_ufmf( 
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgNewDataQueuePtr,bgOldDataQueuePtr);
    }


    void BackgroundMedian_ufmf::initialize(
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr
            )
    {
        ready_ = false;
        stopped_ = true;
        bgNewDataQueuePtr_ = bgNewDataQueuePtr;
        bgOldDataQueuePtr_ = bgOldDataQueuePtr;
        if ((bgNewDataQueuePtr_ != NULL) && (bgOldDataQueuePtr_))
        {
            ready_ = true;
        }
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
        std::shared_ptr<float> medianData;

        if (!ready_) 
        { 
            return; 
        }

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

            std::cout << "*** new median data" << std::endl;

            // Compute median
            medianData = backgroundData.getMedians();


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
