#include "compressor_jpg.hpp"
#include "affinity.hpp"
#include <iostream>
#include <QThread>

namespace bias
{
    Compressor_jpg::Compressor_jpg(QObject *parent) : QObject(parent)
    { 
        initialize(NULL,NULL,0);
        ready_ = false;
    }

    Compressor_jpg::Compressor_jpg(
            CompressedFrameQueuePtr_jpg framesToDoQueuePtr, 
            CompressedFrameSetPtr_jpg framesFinishedSetPtr, 
            unsigned int cameraNumber, 
            QObject *parent
            )  : QObject(parent)
    {
        initialize(framesToDoQueuePtr,framesFinishedSetPtr,cameraNumber);
    }

    
    void Compressor_jpg::initialize(
            CompressedFrameQueuePtr_jpg framesToDoQueuePtr, 
            CompressedFrameSetPtr_jpg framesFinishedSetPtr, 
            unsigned int cameraNumber
            )
    {
        ready_ = false;
        stopped_ = true;
        framesToDoQueuePtr_ = framesToDoQueuePtr;
        framesFinishedSetPtr_ = framesFinishedSetPtr;
        if (framesToDoQueuePtr_ != NULL) 
        {
            ready_ = true;
        }
        cameraNumber_ = cameraNumber;
    }

   
    void Compressor_jpg::stop()
    {
        stopped_ = true;
    }


    void Compressor_jpg::run()
    {
        bool done = false;

        CompressedFrame_jpg compressedFrame;

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
                // Compress the frame and write to file
                bool mjpgFlag = compressedFrame.getMjpgFlag();
                if (mjpgFlag)
                {
                    compressedFrame.encode();
                    framesFinishedSetPtr_ -> acquireLock();
                    framesFinishedSetPtr_ -> insert(compressedFrame);
                    framesFinishedSetSize = framesFinishedSetPtr_ -> size();
                    framesFinishedSetPtr_ -> releaseLock();

                }
                else
                {
                    compressedFrame.write();
                }

            }
        }
    }
} // namespace bias
