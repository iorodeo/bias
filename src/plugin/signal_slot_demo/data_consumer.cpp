#include "data_consumer.hpp"
#include <iostream>
#include <QThread>

namespace bias
{
    // Public methods
    // ----------------------------------------------------------------------------------
    DataConsumer::DataConsumer(QObject *parent) : QObject(parent)
    { 
        stopped_ = true;
    }


    void DataConsumer::stop()
    {
        stopped_ = true;
    }


    void DataConsumer::enqueueFrameDataCamera0(FrameData frameData)
    {
        acquireLock();
        frameDataQueueCamera0_.enqueue(frameData);
        releaseLock();
    }


    void DataConsumer::enqueueFrameDataCamera1(FrameData frameData)
    {
        acquireLock();
        frameDataQueueCamera1_.enqueue(frameData);
        releaseLock();
    }

   

    // Protected methods
    // ----------------------------------------------------------------------------------


    void DataConsumer::run()
    {

        std::cout << "hello" << std::endl;

        bool done = false;

        // Set thread priority to idle - only run when no other thread are running
        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::NormalPriority);

        acquireLock();
        stopped_ = false;
        releaseLock();

        bool haveDataCamera0 = false;
        bool haveDataCamera1 = false;
        FrameData frameDataCamera0;
        FrameData frameDataCamera1;
        long long lastProcessedCount = -1;

        while (!done)
        {

            if (haveDataCamera0 && haveDataCamera1)
            {
                int sizeQueue0 = -1;
                int sizeQueue1 = -1;

                acquireLock();

                // Remove frames from queue #0 until caught up with queue #1
                while ((frameDataCamera0.count < frameDataCamera1.count) && !frameDataQueueCamera0_.isEmpty())
                {
                    frameDataCamera0 = frameDataQueueCamera0_.dequeue();
                }

                // Remove frames from queue #1 until caught up with queue #0
                while ((frameDataCamera1.count < frameDataCamera0.count) && !frameDataQueueCamera1_.isEmpty())
                {
                    frameDataCamera1 = frameDataQueueCamera1_.dequeue();
                }

                // Advance to the next frame (both queues) if we have already processed this one 
                if ((frameDataCamera0.count == lastProcessedCount) && !frameDataQueueCamera0_.isEmpty())
                {
                    frameDataCamera0 = frameDataQueueCamera0_.dequeue();
                }
                if ((frameDataCamera1.count == lastProcessedCount) && !frameDataQueueCamera1_.isEmpty())
                {
                    frameDataCamera1 = frameDataQueueCamera1_.dequeue();
                }

                // Get queue sizes - just for general info
                sizeQueue0 = frameDataQueueCamera0_.size();
                sizeQueue1 = frameDataQueueCamera1_.size();

                // Check to see if stop has been called
                done = stopped_;
                releaseLock();

                // If frame counts match and are greater than last processed frame count then process the data
                if (frameDataCamera0.count == frameDataCamera1.count)
                {
                    if ((long long)(frameDataCamera0.count) > lastProcessedCount)
                    {
                        // --------------------------
                        // Process data here 
                        // --------------------------

                        // Update last processed frame count
                        lastProcessedCount = frameDataCamera0.count;

                        // Print some info
                        std::cout << "processed frame " << lastProcessedCount;
                        std::cout << ", queue0 size = " << sizeQueue0 << ", queue1 size = " << sizeQueue1 << std::endl;
                    }
                }
            }
            else
            {
                // Grab initial frame data from queues
                if (!haveDataCamera0)
                {
                    acquireLock();
                    if (!frameDataQueueCamera0_.isEmpty())
                    {
                        frameDataCamera0 = frameDataQueueCamera0_.dequeue();
                        haveDataCamera0 = true;
                    }
                    releaseLock();

                }
                if (!haveDataCamera1)
                {
                    acquireLock();
                    if (!frameDataQueueCamera1_.isEmpty())
                    {
                        frameDataCamera1 = frameDataQueueCamera1_.dequeue();
                        haveDataCamera1 = true;
                    }
                    releaseLock();
                }
            }
        }
    }

} // namespace bias
