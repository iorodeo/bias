#include "compressor_ufmf.hpp"

namespace bias
{
    Compressor_ufmf::Compressor_ufmf(QObject *parent)
        : QObject(parent)
    { 
        initialize(NULL,NULL);
        ready_ = false;
    }

    Compressor_ufmf::Compressor_ufmf( 
            CompressedFrameQueuePtr_ufmf cmpFrameWaitQueuePtr, 
            CompressedFrameSetPtr_ufmf cmpFrameDoneSetPtr, 
            QObject *parent
            )  
        : QObject(parent)
    {
        initialize(cmpFrameWaitQueuePtr,cmpFrameDoneSetPtr);
    }

    
    void Compressor_ufmf::initialize( 
            CompressedFrameQueuePtr_ufmf cmpFrameWaitQueuePtr, 
            CompressedFrameSetPtr_ufmf cmpFrameDoneSetPtr 
            )
    {
        ready_ = false;
        stopped_ = true;
        cmpFrameWaitQueuePtr_ = cmpFrameWaitQueuePtr;
        cmpFrameDoneSetPtr_ = cmpFrameDoneSetPtr;
        if ((cmpFrameWaitQueuePtr_ != NULL) && (cmpFrameDoneSetPtr_ != NULL))
        {
            ready_ = true;
        }
    }

   
    void Compressor_ufmf::stop()
    {
        stopped_ = true;
    }


    void Compressor_ufmf::run()
    {
        bool done = false;
        CompressedFrame_ufmf cmpFrame;

        if (!ready_)
        {
            return;
        }

        acquireLock();
        stopped_ = false;
        releaseLock();

        while (!done)
        {
            // Get next frame from in waiting queue
            cmpFrameWaitQueuePtr_ -> acquireLock();
            cmpFrameWaitQueuePtr_ -> waitIfEmpty();
            if (cmpFrameWaitQueuePtr_ -> empty())
            {
                cmpFrameWaitQueuePtr_ -> releaseLock();
                break;
            }
            cmpFrame = cmpFrameWaitQueuePtr_ -> front();
            cmpFrameWaitQueuePtr_ -> pop();
            cmpFrameWaitQueuePtr_ -> releaseLock();

        }

    }

} // namespace bias
