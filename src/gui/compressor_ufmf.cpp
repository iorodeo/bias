#include "compressor_ufmf.hpp"
#include "basic_types.hpp"
#include "video_writer_ufmf.hpp"
#include "affinity.hpp"
#include <iostream>
#include <QThread>

namespace bias
{
    Compressor_ufmf::Compressor_ufmf(QObject *parent)
        : QObject(parent)
    { 
        initialize(nullptr,nullptr,nullptr,0);
        ready_ = false;
    }

    Compressor_ufmf::Compressor_ufmf( 
            CompressedFrameQueuePtr_ufmf framesToDoQueuePtr, 
            CompressedFrameSetPtr_ufmf framesFinishedSetPtr, 
            std::shared_ptr<Lockable<std::list<unsigned long>>> framesSkippedIndexListPtr,
            unsigned int cameraNumber,
            QObject *parent
            )  
        : QObject(parent)
    {
        initialize(framesToDoQueuePtr,framesFinishedSetPtr,framesSkippedIndexListPtr,cameraNumber);
    }

    
    void Compressor_ufmf::initialize( 
            CompressedFrameQueuePtr_ufmf framesToDoQueuePtr, 
            CompressedFrameSetPtr_ufmf framesFinishedSetPtr,
            std::shared_ptr<Lockable<std::list<unsigned long>>> framesSkippedIndexListPtr,
            unsigned int cameraNumber
            )
    {
        ready_ = false;
        stopped_ = true;
        skipReported_ = false;
        framesToDoQueuePtr_ = framesToDoQueuePtr;
        framesFinishedSetPtr_ = framesFinishedSetPtr;
        framesSkippedIndexListPtr_ = framesSkippedIndexListPtr;
        if ((framesToDoQueuePtr_ != NULL) && (framesFinishedSetPtr_ != NULL))
        {
            ready_ = true;
        }
        cameraNumber_ = cameraNumber;
    }

   
    void Compressor_ufmf::stop()
    {
        stopped_ = true;
    }


    void Compressor_ufmf::run()
    {
        bool done = false;

        CompressedFrame_ufmf compressedFrame;

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

        unsigned int framesFinishedSetSize = framesFinishedSetPtr_ -> size();

        while (!done)
        {
            bool haveNewFrame = false;

            // Get next frame from in waiting queue
            framesToDoQueuePtr_ -> acquireLock();
            framesToDoQueuePtr_ -> waitIfEmpty();
            if (framesToDoQueuePtr_ -> empty())
            {
                haveNewFrame = false;
            }
            else
            {
                haveNewFrame = true;
                compressedFrame = framesToDoQueuePtr_ -> front();
                framesToDoQueuePtr_ -> pop();
            }
            framesToDoQueuePtr_ -> releaseLock();

            // Check to see if stop has been called
            acquireLock();
            done = stopped_;
            releaseLock();

            if ((haveNewFrame) && (!done))
            {
                // Compress the frame
                compressedFrame.compress();

                if (framesFinishedSetSize < VideoWriter_ufmf::FRAMES_FINISHED_MAX_SET_SIZE)
                {
                    // Put completed compressed frame into finished set
                    framesFinishedSetPtr_ -> acquireLock();
                    framesFinishedSetPtr_ -> insert(compressedFrame);
                    framesFinishedSetSize = framesFinishedSetPtr_ -> size();
                    framesFinishedSetPtr_ -> releaseLock();
                }
                else
                {
                    // Finished frames set is full - skip
                    framesSkippedIndexListPtr_ -> acquireLock();
                    framesSkippedIndexListPtr_ -> push_back(compressedFrame.getFrameCount());
                    framesSkippedIndexListPtr_ -> releaseLock();
                    if (!skipReported_)
                    {
                        unsigned int errorId = ERROR_FRAMES_TODO_MAX_QUEUE_SIZE;
                        QString errorMsg("jpg compressor frames finished set has exceeded the maximum allowed size");
                        emit imageLoggingError(errorId, errorMsg);
                        skipReported_ = true;
                    }
                }

            } // if (haveNewFrame) 

        } // while (!done)  

    } // void Compressor_ufmf::run()

} // namespace bias
