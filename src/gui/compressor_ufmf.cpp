#include "compressor_ufmf.hpp"
#include <iostream>

namespace bias
{
    Compressor_ufmf::Compressor_ufmf(QObject *parent)
        : QObject(parent)
    { 
        initialize(NULL,NULL);
        ready_ = false;
    }

    Compressor_ufmf::Compressor_ufmf( 
            CompressedFrameQueuePtr_ufmf framesToDoQueuePtr, 
            CompressedFrameSetPtr_ufmf framesFinishedSetPtr, 
            QObject *parent
            )  
        : QObject(parent)
    {
        initialize(framesToDoQueuePtr,framesFinishedSetPtr);
    }

    
    void Compressor_ufmf::initialize( 
            CompressedFrameQueuePtr_ufmf framesToDoQueuePtr, 
            CompressedFrameSetPtr_ufmf framesFinishedSetPtr 
            )
    {
        std::cout << " starting compressor" << std:: endl;

        ready_ = false;
        stopped_ = true;
        framesToDoQueuePtr_ = framesToDoQueuePtr;
        framesFinishedSetPtr_ = framesFinishedSetPtr;
        if ((framesToDoQueuePtr_ != NULL) && (framesFinishedSetPtr_ != NULL))
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
            framesToDoQueuePtr_ -> acquireLock();
            framesToDoQueuePtr_ -> waitIfEmpty();
            if (framesToDoQueuePtr_ -> empty())
            {
                framesToDoQueuePtr_ -> releaseLock();
                break;
            }
            cmpFrame = framesToDoQueuePtr_ -> front();
            framesToDoQueuePtr_ -> pop();
            framesToDoQueuePtr_ -> releaseLock();

            // Compress the frame
            cmpFrame.compress();

            // Put completed compressed frame into "done" set
            framesFinishedSetPtr_ -> acquireLock();
            framesFinishedSetPtr_ -> insert(cmpFrame);
            framesFinishedSetPtr_ -> releaseLock();

            // Check to see if stop has been called
            acquireLock();
            done = stopped_;
            releaseLock();

        }

        std::cout << "  compressor done" << std::endl;

    }

} // namespace bias
