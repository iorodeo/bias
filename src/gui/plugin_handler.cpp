#include <plugin_handler.hpp>
#include <QThread>
#include <iostream>
#include <sstream>
#include "affinity.hpp"
#include "stamped_image.hpp"

namespace bias
{ 
    const unsigned int PluginHandler::MAX_IMAGE_QUEUE_SIZE = 500;

    PluginHandler::PluginHandler(QObject *parent) : QObject(parent)
    {
        QPointer<BiasPlugin> emptyPluginPtr;
        initialize(0,NULL,emptyPluginPtr);
    }


    PluginHandler::PluginHandler(
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr,
            QObject *parent
            ) : QObject(parent)
    {
        QPointer<BiasPlugin> emptyPluginPtr;
        initialize(cameraNumber, pluginImageQueuePtr, emptyPluginPtr);
    }


    PluginHandler::PluginHandler(
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr,
            BiasPlugin *pluginPtr,
            QObject *parent
            ) : QObject(parent)
    {
        QPointer<BiasPlugin> emptyPluginPtr;
        initialize(cameraNumber, pluginImageQueuePtr, emptyPluginPtr);
    }

    void PluginHandler::setCameraNumber(unsigned int cameraNumber)
    {
       cameraNumber_ = cameraNumber;
    } 

    void PluginHandler::setImageQueue(std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr)
    {
        pluginImageQueuePtr_ = pluginImageQueuePtr;
        setReadyState();
    }


    void PluginHandler::setPlugin(BiasPlugin *pluginPtr)
    {
        pluginPtr_ = pluginPtr;
        setReadyState();
    }


    void PluginHandler::initialize(
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr,
            BiasPlugin *pluginPtr
            )
    {
        ready_ = false;
        stopped_ = true;
        setCameraNumber(cameraNumber);
        setImageQueue(pluginImageQueuePtr);
        setPlugin(pluginPtr);

        // Plugin setup actions
        std::cout << "plugin: setup" << std::endl;
    }


    void PluginHandler::stop()
    {
        stopped_ = true;
    }


    cv::Mat PluginHandler::getImage() const
    {
        cv::Mat currentImage;
        if (!pluginPtr_.isNull())
        {
            currentImage = pluginPtr_ -> getCurrentImage();
        }
        return currentImage;
    }

    void PluginHandler::setReadyState()
    {
        if ((pluginImageQueuePtr_ != NULL) && (!pluginPtr_.isNull()))
        {
            ready_ = true;
        }
        else
        {
            ready_ = false;
        }
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

            // Process Frame with plugin
            if (!pluginPtr_.isNull())
            {
                pluginPtr_ -> processFrame(newStampedImage);
            }
            
            acquireLock();
            done = stopped_;
            releaseLock();

        } // while (!done)

        // Plugin clean up actions
        std::cout << "plugin: clean up" << std::endl;

    } // PlugingHandler::run()

} // namespace bias;
