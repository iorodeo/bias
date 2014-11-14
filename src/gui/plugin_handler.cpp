#include <plugin_handler.hpp>
#include <QThread>
#include <iostream>
#include "affinity.hpp"
#include "stamped_image.hpp"

namespace bias
{ 
    const unsigned int PluginHandler::MAX_IMAGE_QUEUE_SIZE = 500;

    PluginHandler::PluginHandler(QObject *parent) : QObject(parent)
    {
        initialize(0,NULL);
    }


    PluginHandler::PluginHandler(
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr,
            QObject *parent
            ) : QObject(parent)
    {
        initialize(cameraNumber, pluginImageQueuePtr);
    }


    void PluginHandler::initialize(
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr
            )
    {
        stopped_ = true;
        cameraNumber_ = cameraNumber;
        pluginImageQueuePtr_ = pluginImageQueuePtr;
        if (pluginImageQueuePtr_ != NULL)
        {
            ready_ = true;
        }
        else
        {
            ready_ = false;
        }

        // Plugin setup actions
        std::cout << "plugin: setup" << std::endl;
    }


    void PluginHandler::stop()
    {
        stopped_ = true;
    }


    void PluginHandler::run()
    {
        bool done = false;
        unsigned int imageQueueSize = 0;
        StampedImage newStampedImage;

        if (!ready_) 
        { 
            return; 
        }

        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::NormalPriority);
        ThreadAffinityService::assignThreadAffinity(false,cameraNumber_);

        acquireLock();
        stopped_ = false;
        releaseLock();

        while (!done)
        {
            // Grab frame from image queue
            pluginImageQueuePtr_ -> acquireLock();
            pluginImageQueuePtr_ -> waitIfEmpty();
            if (pluginImageQueuePtr_ -> empty())
            {
                pluginImageQueuePtr_ -> releaseLock();
                break;
            }
            newStampedImage = pluginImageQueuePtr_ -> front();
            pluginImageQueuePtr_ -> pop();
            imageQueueSize =  pluginImageQueuePtr_ -> size();
            pluginImageQueuePtr_ -> releaseLock();

            // Plugin process frame
            std::cout << "plugin: process frame, frame count = ";
            std::cout << newStampedImage.frameCount;
            std::cout << ", imageQueueSize = " << imageQueueSize << std::endl;
            
            acquireLock();
            done = stopped_;
            releaseLock();

        } // while (!done)

        // Plugin clean up actions
        std::cout << "plugin: clean up" << std::endl;

    } // PlugingHandler::run()

} // namespace bias;
