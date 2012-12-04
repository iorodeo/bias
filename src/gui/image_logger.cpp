#include "image_logger.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include "stamped_image.hpp"
#include "video_writer.hpp"
#include <queue>
#include <iostream>
#include <opencv2/core/core.hpp>

namespace bias
{
    const unsigned int MAX_LOG_QUEUE_SIZE = 1000;

    ImageLogger::ImageLogger(QObject *parent) : QObject(parent) 
    {
        ready_ = false;
        stopped_ = true;
        frameCount_ = 0;
    }

    ImageLogger::ImageLogger (
            std::shared_ptr<VideoWriter> videoWriterPtr,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(videoWriterPtr, logImageQueuePtr);
    }

    void ImageLogger::initialize( 
            std::shared_ptr<VideoWriter> videoWriterPtr,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr 
            ) 
    {
        videoWriterPtr_ = videoWriterPtr;
        logImageQueuePtr_ = logImageQueuePtr;
        ready_ = true;
        stopped_ = true;
        frameCount_ = 0;
    }

    void ImageLogger::stop()
    {
        stopped_ = true;
    }


    void ImageLogger::run()
    {
        StampedImage newStampedImage;
        bool haveNewImage = false;
        bool done = false;
        unsigned int logQueueSize;

        if (!ready_) { return; }

        acquireLock();
        stopped_ = false;
        frameCount_ = 0;
        releaseLock();

        while (!done)
        {
            haveNewImage = false;

            logImageQueuePtr_ -> acquireLock();
            if (!logImageQueuePtr_ -> empty())
            {
                newStampedImage = logImageQueuePtr_ -> front();
                logImageQueuePtr_ -> pop();
                haveNewImage = true;
            }
            logQueueSize =  logImageQueuePtr_ -> size();
            logImageQueuePtr_ -> releaseLock();

            //std::cout << "queue size: " << logQueueSize << std::endl;

            if (logQueueSize > MAX_LOG_QUEUE_SIZE)
            {
                unsigned int errorId = ERROR_IMAGE_LOGGER_MAX_QUEUE_SIZE;
                QString errorMsg("logger image queue has exceeded the maximum allowed size");
                emit imageLoggingError(errorId, errorMsg);
            }

            if (haveNewImage)
            {
                frameCount_++;

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
            releaseLock();
        }

    } // while (!done)

} // namespace bias

