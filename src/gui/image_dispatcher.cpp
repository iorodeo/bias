#include "image_dispatcher.hpp"
#include "stamped_image.hpp"
#include "affinity.hpp"
#include <iostream>
#include <QThread>

// DEVEL
// ----------------------------------------------------------------------------
#include "camera_window.hpp"
#include <QDir>
#include <QFileInfo>
#include <fstream>
// ----------------------------------------------------------------------------

namespace bias
{

    ImageDispatcher::ImageDispatcher(QObject *parent) : QObject(parent)
    {
        initialize(false,0,NULL,NULL);
    }

    ImageDispatcher::ImageDispatcher( 
            bool logging,
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr, 
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
            QObject *parent
            ) : QObject(parent)
    {
        initialize(logging,cameraNumber,newImageQueuePtr,logImageQueuePtr);
    }

    void ImageDispatcher::initialize(
            bool logging,
            unsigned int cameraNumber,
            std::shared_ptr<LockableQueue<StampedImage>> newImageQueuePtr,
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr 
            ) 
    {
        newImageQueuePtr_ = newImageQueuePtr;
        logImageQueuePtr_ = logImageQueuePtr;
        if ((newImageQueuePtr_ != NULL) && (logImageQueuePtr_ !=NULL)) 
        {
            ready_ = true;
        }
        else 
        {
            ready_ = false;
        }
        stopped_ = true;
        logging_ = logging;
        cameraNumber_ = cameraNumber;
        frameCount_ = 0;
        currentTimeStamp_ = 0.0;
    }

    cv::Mat ImageDispatcher::getImage() const
    {
        return currentImage_;
    }

    double ImageDispatcher::getTimeStamp() const
    {
        return currentTimeStamp_;
    }

    double ImageDispatcher::getFPS() const
    {
        return fpsEstimator_.getValue();
    }

    unsigned long ImageDispatcher::getFrameCount() const
    {
        return frameCount_;
    }

    void ImageDispatcher::stop()
    {
        stopped_ = true;
    }


    void ImageDispatcher::run()
    {
        bool done = false;
        StampedImage newStampImage;

        if (!ready_) 
        { 
            return; 
        }

        // Set thread priority to normal and assign cpu affinity
        QThread *thisThread = QThread::currentThread();
        thisThread -> setPriority(QThread::TimeCriticalPriority);
        ThreadAffinityService::assignThreadAffinity(false,cameraNumber_);

        // Initiaiize values
        acquireLock();
        frameCount_ = 0;
        stopped_ = false;
        fpsEstimator_.reset();
        releaseLock();

        // DEVEL
        // --------------------------------------------------------------------
        CameraWindow* cameraWindowPtr = qobject_cast<CameraWindow *>(parent());
        QDir videoFileDir = cameraWindowPtr -> getVideoFileDir();
        QString stampLogName = QString("stamp_log_cam%1.txt").arg(cameraNumber_);
        QFileInfo stampFileInfo = QFileInfo(videoFileDir, stampLogName);
        std::string stampFileName = stampFileInfo.absoluteFilePath().toStdString();
        std::ofstream stampOutStream;
        stampOutStream.open(stampFileName);

        // --------------------------------------------------------------------

        while (!done) 
        {

            newImageQueuePtr_ -> acquireLock();
            newImageQueuePtr_ -> waitIfEmpty();
            if (newImageQueuePtr_ -> empty())
            {
                newImageQueuePtr_ -> releaseLock();
                break;
            }
            newStampImage = newImageQueuePtr_ -> front();
            newImageQueuePtr_ -> pop();
            newImageQueuePtr_ -> releaseLock();

            if (logging_ )
            {
                logImageQueuePtr_ -> acquireLock();
                logImageQueuePtr_ -> push(newStampImage);
                logImageQueuePtr_ -> signalNotEmpty();
                logImageQueuePtr_ -> releaseLock();
            }

            acquireLock();
            currentImage_ = newStampImage.image;
            currentTimeStamp_ = newStampImage.timeStamp;
            frameCount_ = newStampImage.frameCount;
            fpsEstimator_.update(newStampImage.timeStamp);
            done = stopped_;
            releaseLock();

            // DEVEL
            // ----------------------------------------------------------------
            stampOutStream << QString::number(currentTimeStamp_,'g',15).toStdString(); 
            stampOutStream << std::endl;
            // ----------------------------------------------------------------

        }

        // DEVEL
        // --------------------------------------------------------------------
        stampOutStream.close();
        // --------------------------------------------------------------------
    }

} // namespace bias


