#include "background_median_ufmf.hpp"
#include "background_data_ufmf.hpp"
#include <iostream>

namespace bias
{ 
    BackgroundMedian_ufmf::BackgroundMedian_ufmf(QObject *parent)
        : QObject(parent)
    { 
        initialize(NULL);
    }


    BackgroundMedian_ufmf::BackgroundMedian_ufmf( 
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr,
            QObject *parent
            ) 
        : QObject(parent)
    {
        initialize(bgDataQueuePtr);
    }


    void BackgroundMedian_ufmf::initialize(
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr
            )
    {
        ready_ = false;
        stopped_ = true;
        bgDataQueuePtr_ = bgDataQueuePtr;
        if (bgDataQueuePtr_ != NULL)
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
            bgDataQueuePtr_ -> acquireLock();
            bgDataQueuePtr_ -> waitIfEmpty();
            if (bgDataQueuePtr_ -> empty())
            {
                bgDataQueuePtr_ -> releaseLock();
                break;
            }
            backgroundData = bgDataQueuePtr_ -> front();
            bgDataQueuePtr_ -> pop();
            bgDataQueuePtr_ -> releaseLock();

            std::cout << "background median - have new data" << std::endl;

            acquireLock();
            done = stopped_;
            releaseLock();

        } // while(!done)
    }


} // namespace bias
