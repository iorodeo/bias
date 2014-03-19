#include "image_logger.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include "stamped_image.hpp"
#include "video_writer.hpp"
#include "affinity.hpp"
#include <QThread>
#include <queue>
#include <iostream>
#include <opencv2/core/core.hpp>

namespace bias
{
    const unsigned int MAX_LOG_QUEUE_SIZE = 1000;

    ImageLogger::ImageLogger(QObject *parent) : QObject(parent) 
    {
        initialize(0, NULL,NULL);
    }

    ImageLogger::ImageLogger (
            unsigned int cameraNumber,
            std::shared_ptr<VideoWriter> videoWriterPtr,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(cameraNumber, videoWriterPtr, logImageQueuePtr);
    }

    void ImageLogger::initialize( 
            unsigned int cameraNumber,
            std::shared_ptr<VideoWriter> videoWriterPtr,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr 
            ) 
    {
        frameCount_ = 0;
        stopped_ = true;
        cameraNumber_ = cameraNumber;
        videoWriterPtr_ = videoWriterPtr;
        logImageQueuePtr_ = logImageQueuePtr;
        if ((logImageQueuePtr_ != NULL) && (videoWriterPtr_ != NULL))
        {
            ready_ = true;
        }
        else
        {
            ready_ = false;
        }
    }

    void ImageLogger::stop()
    {
        stopped_ = true;
    }

    unsigned int ImageLogger::getLogQueueSize()
    {
        return logQueueSize_;
    }

    void ImageLogger::run()
    {
        bool done = false;
        bool errorFlag = false;
        StampedImage newStampedImage;
        unsigned int logQueueSize;

        if (!ready_) { return; }

        // Set thread priority to normal
        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::NormalPriority);
        assignThreadAffinity(false,1);

        acquireLock();
        stopped_ = false;
        frameCount_ = 0;
        releaseLock();

        while (!done)
        {
            logImageQueuePtr_ -> acquireLock();
            logImageQueuePtr_ -> waitIfEmpty();
            if (logImageQueuePtr_ -> empty())
            {
                logImageQueuePtr_ -> releaseLock();
                break;
            }
            newStampedImage = logImageQueuePtr_ -> front();
            logImageQueuePtr_ -> pop();
            logQueueSize =  logImageQueuePtr_ -> size();
            logImageQueuePtr_ -> releaseLock();

            frameCount_++;
            //std::cout << "logger frame count = " << frameCount_ << std::endl;

            if (!errorFlag) 
            {
                // Check if log queue has grown too large - if so signal an error
                if (logQueueSize > MAX_LOG_QUEUE_SIZE)
                {
                    unsigned int errorId = ERROR_IMAGE_LOGGER_MAX_QUEUE_SIZE;
                    QString errorMsg("logger image queue has exceeded the maximum allowed size");
                    emit imageLoggingError(errorId, errorMsg);
                    errorFlag = true;
                }
                //std::cout << "cam: " << cameraNumber_ << ", queue size: " << logQueueSize;
                //std::cout << "/" << MAX_LOG_QUEUE_SIZE << std::endl;

                // Add frame to video writer
                try 
                {
                    videoWriterPtr_ -> addFrame(newStampedImage);
                }
                catch (RuntimeError &runtimeError)
                {
                    unsigned int errorId = runtimeError.id();;
                    QString errorMsg = QString::fromStdString(runtimeError.what());
                    emit imageLoggingError(errorId, errorMsg);
                }
            }

            acquireLock();
            done = stopped_;
            logQueueSize_ = logQueueSize;
            releaseLock();

        } // while (!done)

        try
        {
            videoWriterPtr_ -> finish();
        }
        catch (RuntimeError &runtimeError)
        {
            unsigned int errorId = runtimeError.id();;
            QString errorMsg = QString::fromStdString(runtimeError.what());
            emit imageLoggingError(errorId, errorMsg);
        }
    
    }  // void ImageLogger::run()



} // namespace bias

