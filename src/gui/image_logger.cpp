#include "image_logger.hpp"
#include "exception.hpp"
#include "stamped_image.hpp"
#include <queue>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace bias
{
    ImageLogger::ImageLogger(QObject *parent) : QObject(parent) 
    {
        logFileName_ = QString("no_log_file_set");
        ready_ = false;
        stopped_ = true;
        frameCount_ = 0;
    }

    ImageLogger::ImageLogger (
            QString logFileName,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(logFileName, logImageQueuePtr);
    }

    void ImageLogger::initialize( 
            QString logFileName,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr 
            ) 
    {
        logFileName_ = logFileName;
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
        StampedImage newStampImage;
        unsigned int errorId = 0;
        bool haveNewImage = false;
        bool isFirst = true;
        bool error = false;
        bool done = false;
        cv::VideoWriter videoWriter;

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
                newStampImage = logImageQueuePtr_ -> front();
                logImageQueuePtr_ -> pop();
                haveNewImage = true;
                std::cout << "log   queue size = " << logImageQueuePtr_ -> size() << std::endl;
            }
            logImageQueuePtr_ -> releaseLock();

            if (haveNewImage)
            {
                frameCount_++;
                if (isFirst)
                {
                    // On first frame get image size and open file.
                    cv::Size frameSize = newStampImage.image.size();
                    std::cout << "logFileName: " << logFileName_.toStdString() << std::endl;
                    videoWriter.open(
                            logFileName_.toStdString(),
                            CV_FOURCC('D','I','V', 'X'),
                            30.0,
                            frameSize,false
                            ); 
                    isFirst = false;
                }
                if (videoWriter.isOpened())
                {
                    if (frameCount_%5 == 0) 
                    {
                        std::cout << "writing image " << frameCount_ << std::endl << std::flush;
                        videoWriter << newStampImage.image;
                    }
                }
                else
                {
                    std::cout << "video file not open" << std::endl << std::flush;
                }
            }

            acquireLock();
            done = stopped_;
            releaseLock();
        }
    }

} // namespace bias

