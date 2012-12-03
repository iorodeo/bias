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
        unsigned int errorId = 0;
        bool haveNewImage = false;
        bool isFirst = true;
        bool error = false;
        bool done = false;

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
            std::cout << "log   queue size = " << logImageQueuePtr_ -> size() << std::endl;
            logImageQueuePtr_ -> releaseLock();

            if (haveNewImage)
            {
                frameCount_++;

                try 
                {
                    videoWriterPtr_ -> addFrame(newStampedImage);
                }
                catch (RuntimeError &runtimeError)
                {
                    acquireLock();
                    stopped_ = true;
                    releaseLock();

                    unsigned int errorId = ERROR_VIDEO_WRITER_ADD_FRAME;
                    QString errorMsg("videoWriter addFrame failed");
                    emit imageLoggingError(errorId, errorMsg);
                }
            }

            acquireLock();
            done = stopped_;
            releaseLock();
        }

        videoWriterPtr_ -> stop();

    } // while (!done)

} // namespace bias


// cv videoWriter example
// ----------------------------------------------------
//#include <opencv2/highgui/highgui.hpp>
//cv::VideoWriter videoWriter;
//cv::Size frameSize = newStampedImage.image.size();
//videoWriter.open( logFileName_.toStdString(), CV_FOURCC('D','I','V', 'X'), 30.0, frameSize, false );
//videoWriter << newStampedImage.image;
