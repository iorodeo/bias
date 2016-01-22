#include "video_writer_jpg.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>
#include <QFileInfo>
#include <QThreadPool>
#include <stdexcept>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <QtDebug>

namespace bias
{
    const QString VideoWriter_jpg::IMAGE_FILE_BASE = QString("image_");
    const QString VideoWriter_jpg::IMAGE_FILE_EXT = QString(".jpg");
    const QString VideoWriter_jpg::MJPG_FILE_EXT = QString(".mjpg");
    const QString VideoWriter_jpg::MJPG_INDEX_EXT = QString(".txt");
    const QString VideoWriter_jpg::MJPG_FILE_NAME = QString("movie");
    const QString VideoWriter_jpg::MJPG_INDEX_NAME = QString("index");
    const std::string VideoWriter_jpg::MJPG_BOUNDARY_MARKER = std::string("--boundary\r\n");
    const QString DUMMY_FILENAME("dummy.jpg");
    const unsigned int VideoWriter_jpg::FRAMES_TODO_MAX_QUEUE_SIZE = 250;
    const unsigned int VideoWriter_jpg::FRAMES_FINISHED_MAX_SET_SIZE = 250;
    const unsigned int VideoWriter_jpg::DEFAULT_FRAME_SKIP = 1;
    const unsigned int VideoWriter_jpg::DEFAULT_QUALITY = 90;
    const unsigned int VideoWriter_jpg::MIN_QUALITY = 0;
    const unsigned int VideoWriter_jpg::MAX_QUALITY = 100;
    const unsigned int VideoWriter_jpg::DEFAULT_NUMBER_OF_COMPRESSORS = 10;
    const bool VideoWriter_jpg::DEFAULT_MJPG_FLAG = true;
    const bool VideoWriter_jpg::DEFAULT_MJPG_MAX_FRAME_PER_FILE_FLAG = false;
    const unsigned long VideoWriter_jpg::DEFAULT_MJPG_MAX_FRAME_PER_FILE = 5000000;
    const unsigned long VideoWriter_jpg::MJPG_MINVAL_MAX_FRAME_PER_FILE = 10;
    const VideoWriterParams_jpg VideoWriter_jpg::DEFAULT_PARAMS = VideoWriterParams_jpg();

    // VideoWriter_jpg methods
    VideoWriter_jpg::VideoWriter_jpg(QObject *parent) : VideoWriter_jpg(DEFAULT_PARAMS,DUMMY_FILENAME,0,parent) 
    {}

    VideoWriter_jpg::VideoWriter_jpg(
            VideoWriterParams_jpg params,
            QString fileName, 
            unsigned int cameraNumber,
            QObject *parent
            ) : VideoWriter(fileName,cameraNumber,parent) 
    {

        //std::cout << params.toString() << std::endl;

        isFirst_ = true;
        skipReported_ = false;;
        nextFrameToWrite_ = 0;

        movieFileCount_ = 0;
        movieFileFrameCount_ = 0;

        setFrameSkip(params.frameSkip);
        quality_ = params.quality;
        mjpgFlag_ = params.mjpgFlag;
        mjpgMaxFramePerFileFlag_ = params.mjpgMaxFramePerFileFlag;
        mjpgMaxFramePerFile_ = params.mjpgMaxFramePerFile;
        numberOfCompressors_ = params.numberOfCompressors; 

        threadPoolPtr_ = new QThreadPool(this);
        threadPoolPtr_ -> setMaxThreadCount(numberOfCompressors_);
        framesToDoQueuePtr_ = std::make_shared<CompressedFrameQueue_jpg>();
        framesFinishedSetPtr_ = std::make_shared<CompressedFrameSet_jpg>();
        framesSkippedIndexListPtr_ = std::make_shared<Lockable<std::list<unsigned long>>>();
    }


    VideoWriter_jpg::~VideoWriter_jpg() 
    {
        stopCompressors();
        if (mjpgFlag_)
        {
            movieFile_.close();
            indexFile_.close();
        }
    }


    void VideoWriter_jpg::setFileName(QString fileName)
    {
        fileName_ = fileName;
        QFileInfo fileInfo(fileName_);
        baseName_ = fileInfo.baseName();
        baseDir_ = fileInfo.dir();
    }

    void VideoWriter_jpg::addFrame(StampedImage stampedImg)
    {
        bool skipFrame = false;

        if (isFirst_)
        {
            setupOutput();
            startCompressors();
            isFirst_= false;
        }

        QString imageFileName = IMAGE_FILE_BASE;  
        imageFileName += QString::number(frameCount_);
        imageFileName += IMAGE_FILE_EXT;
        QFileInfo imageFileInfo(logDir_,imageFileName);
        QString fullPathName = imageFileInfo.absoluteFilePath();

        framesToDoQueuePtr_  -> acquireLock();
        framesToDoQueuePtr_  -> releaseLock();

        if (frameCount_%frameSkip_==0) 
        {
            framesToDoQueuePtr_ -> acquireLock();
            unsigned int framesToDoQueueSize = framesToDoQueuePtr_ -> size();
            if (framesToDoQueueSize < FRAMES_TODO_MAX_QUEUE_SIZE)
            {
                CompressedFrame_jpg compressedFrame(fullPathName, stampedImg, quality_, mjpgFlag_);
                framesToDoQueuePtr_ -> push(compressedFrame);
                framesToDoQueuePtr_ -> wakeOne();
            }
            else
            { 
                skipFrame = true;
            }
            framesToDoQueuePtr_ -> releaseLock();

            if (skipFrame)
            {
                framesSkippedIndexListPtr_ -> acquireLock();
                framesSkippedIndexListPtr_ -> push_back(stampedImg.frameCount);
                framesSkippedIndexListPtr_ -> releaseLock();
            }
        }

        framesSkippedIndexListPtr_ -> acquireLock();
        unsigned int framesSkippedIndexListSize = framesSkippedIndexListPtr_ -> size();
        framesSkippedIndexListPtr_ -> releaseLock();

        if ((skipFrame) && (!skipReported_))
        { 
            std::cout << "warning: logging overflow - skipped frame -" << std::endl;
            unsigned int errorId = ERROR_FRAMES_TODO_MAX_QUEUE_SIZE;
            QString errorMsg("logger framesToDoQueue has exceeded the maximum allowed size");
            emit imageLoggingError(errorId, errorMsg);
            skipReported_ = true;
            // Note this will not trigger stop of image acquisition ... just display a warning.
        }

        clearFinishedFrames();
        frameCount_++;
    }


    void VideoWriter_jpg::finish()
    {
        bool finished = false;
        while (!finished)
        {
            framesToDoQueuePtr_ -> acquireLock();
            if ( (framesToDoQueuePtr_ -> size()) == 0)
            {
                finished = true;
            }
            framesToDoQueuePtr_ -> releaseLock();
        }
        while (clearFinishedFrames() > 0);
    }


    unsigned int VideoWriter_jpg::getNextVersionNumber()
    {
        unsigned int nextVerNum = 0;
        if (addVersionNumber_)
        {
            bool done = false;
            QDir testLogDir;
            while (!done)
            {
                nextVerNum++;
                testLogDir = getLogDir(nextVerNum);
                if (!testLogDir.exists())
                {
                    done = true;
                }
            }
        }
        return nextVerNum;
    }

    void VideoWriter_jpg::setupOutput()
    {
        if (!baseDir_.exists())
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("base log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            errorMsg += std::string("does not exist");
            throw RuntimeError(errorId, errorMsg);
        }

        unsigned int verNum = getNextVersionNumber();
        QString logDirName = getLogDirName(verNum);
        logDir_ = getLogDir(verNum);

        if (baseDir_.exists(logDirName) && !addVersionNumber_)
        {
            QString dirCopyName = logDirName + QString("_copy");
            if (baseDir_.exists(dirCopyName))
            {
                bool done = false;
                unsigned int cnt = 2;
                while (!done)
                {
                    dirCopyName = logDirName + QString("_copy_%1").arg(cnt);
                    if (!baseDir_.exists(dirCopyName))
                    {
                        done = true;
                    }
                    cnt++;
                }
            }
            baseDir_.rename(logDirName,  dirCopyName);
        }

        if (!baseDir_.mkdir(logDirName))
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("unable to create log directory, "); 
            errorMsg += baseDir_.absolutePath().toStdString();
            throw RuntimeError(errorId, errorMsg);
        }

        if (mjpgFlag_)
        {
            QString movieFileName = getMovieFileName(); 
            QString indexFileName = getIndexFileName();
            movieFile_.open(movieFileName.toStdString(), std::ios::out | std::ios::binary);
            indexFile_.open(indexFileName.toStdString(), std::ios::out);
        }
    }


    QString VideoWriter_jpg::getMovieFileName()
    {
        QString movieFileName;
        if (mjpgMaxFramePerFileFlag_)
        { 
            QString incrFileName = QString("%1_%2%3").arg(MJPG_FILE_NAME).arg(movieFileCount_).arg(MJPG_FILE_EXT);
            movieFileName = logDir_.absoluteFilePath(incrFileName);
        }
        else
        {
            movieFileName = logDir_.absoluteFilePath(MJPG_FILE_NAME + MJPG_FILE_EXT);
        }
        return movieFileName;
    }


    QString VideoWriter_jpg::getIndexFileName()
    {
        QString indexFileName;
        if (mjpgMaxFramePerFileFlag_)
        {
            QString incrFileName = QString("%1_%2%3").arg(MJPG_INDEX_NAME).arg(movieFileCount_).arg(MJPG_INDEX_EXT);
            indexFileName = logDir_.absoluteFilePath(incrFileName);
        }
        else
        {
            indexFileName = logDir_.absoluteFilePath(MJPG_INDEX_NAME + MJPG_INDEX_EXT);
        }
        return indexFileName;
    }


    QString VideoWriter_jpg::getUniqueDirName()
    {
        unsigned int nextVerNum = getNextVersionNumber();
        QString logDirName = getLogDirName(nextVerNum);
        return logDirName;
    }


    QString VideoWriter_jpg::getLogDirName(unsigned int verNum)
    {
        QString logDirName;
        if (verNum > 0)
        { 
            QString verStr = QString("_v%1").arg(verNum,3,10,QChar('0'));
            logDirName =  baseName_ + verStr;
        }
        else
        {
            logDirName = baseName_;
        }
        return logDirName;
    }


    QDir VideoWriter_jpg::getLogDir(unsigned int verNum)
    {
        QString logDirName = getLogDirName(verNum);
        QDir logDir = QDir(baseDir_.absolutePath() + "/" + logDirName);
        return logDir;
    }


    void VideoWriter_jpg::startCompressors()
    {
        framesToDoQueuePtr_ -> clear();
        compressorPtrVec_.resize(numberOfCompressors_);
        for (unsigned int i=0; i<compressorPtrVec_.size(); i++)
        {
            compressorPtrVec_[i] = new Compressor_jpg(
                    framesToDoQueuePtr_, 
                    framesFinishedSetPtr_, 
                    framesSkippedIndexListPtr_, 
                    cameraNumber_
                    );
            threadPoolPtr_ -> start(compressorPtrVec_[i]);
            connect(
                    compressorPtrVec_[i],
                    SIGNAL(imageLoggingError(unsigned int, QString)),
                    this,
                    SLOT(onCompressorError(unsigned int, QString))
                   );
        }
    }


    void VideoWriter_jpg::stopCompressors()
    {
        // Send all compressor threads a stop signal 
        for (unsigned int i=0; i<compressorPtrVec_.size(); i++)
        {
            if (!(compressorPtrVec_[i].isNull()))
            { 
                compressorPtrVec_[i] -> acquireLock();
                compressorPtrVec_[i] -> stop();
                compressorPtrVec_[i] -> releaseLock();
            }
        }

        // Wait until all compressor threads are null
        for (unsigned int i=0; i<compressorPtrVec_.size(); i++)
        {
            while (!(compressorPtrVec_[i].isNull()))
            {
                framesToDoQueuePtr_ -> acquireLock();
                framesToDoQueuePtr_ -> wakeOne();
                framesToDoQueuePtr_ -> releaseLock();
            }
        }
    }

    unsigned int VideoWriter_jpg::clearFinishedFrames()
    {
        framesFinishedSetPtr_ -> acquireLock();
        bool framesFinishedSetEmpty = framesFinishedSetPtr_ -> empty();
        framesFinishedSetPtr_ -> releaseLock();

        if (!framesFinishedSetEmpty)
        {
            bool writeDone = false;
            while ( (!writeDone) && (!framesFinishedSetEmpty) ) 
            {
                // Handle skipped frames. 
                // --------------------------------------------------------------------------------
                framesSkippedIndexListPtr_ -> acquireLock();
                if (framesSkippedIndexListPtr_ -> size() > 0)

                {
                    std::list<unsigned long>::iterator skippedIndexIt = framesSkippedIndexListPtr_ -> begin();
                    bool done = false;

                    while (!done)
                    {
                        if (*skippedIndexIt == nextFrameToWrite_)
                        {
                            nextFrameToWrite_ += frameSkip_;
                        }
                        else if (*skippedIndexIt < nextFrameToWrite_)
                        {
                            if (framesSkippedIndexListPtr_ -> size() > 1)
                            {
                                skippedIndexIt++;
                            }
                            else
                            {
                                done = true;
                            }
                            framesSkippedIndexListPtr_ -> pop_front();
                        }
                        else
                        {
                            done = true;
                        }
                    }
                }
                framesSkippedIndexListPtr_ -> releaseLock();

                // Write next frame to file
                // --------------------------------------------------------------------------------
                framesFinishedSetPtr_ -> acquireLock();
                CompressedFrameSet_jpg::iterator frameIt = framesFinishedSetPtr_ -> begin();
                CompressedFrame_jpg compressedFrame = *frameIt;

                if (compressedFrame.getFrameCount() == nextFrameToWrite_)
                {
                    framesFinishedSetPtr_ -> erase(frameIt);
                    nextFrameToWrite_ += frameSkip_;
                    writeCompressedMjpgFrame(compressedFrame);

                    movieFileFrameCount_ += 1;
                    if ((mjpgMaxFramePerFileFlag_) && (movieFileFrameCount_ >= mjpgMaxFramePerFile_)) { 
                        movieFile_.close();
                        indexFile_.close();
                        movieFileCount_ += 1;
                        movieFileFrameCount_ = 0;
                        QString movieFileName = getMovieFileName(); 
                        QString indexFileName = getIndexFileName();
                        movieFile_.open(movieFileName.toStdString(), std::ios::out | std::ios::binary);
                        indexFile_.open(indexFileName.toStdString(), std::ios::out);
                    }
                }
                else
                {
                    writeDone = true;
                }
                framesFinishedSetEmpty = framesFinishedSetPtr_ -> empty();
                framesFinishedSetPtr_ -> releaseLock();

            } // while ( (!writeDone) && (!framesFinishedSetEmpty) ) 

        } // if (!(framesFinishedSetPtr_ 

        framesFinishedSetPtr_ -> acquireLock();
        unsigned int framesFinishedSetSize = framesFinishedSetPtr_ -> size();
        framesFinishedSetPtr_ -> releaseLock();
        return framesFinishedSetSize;
    }

    void VideoWriter_jpg::writeCompressedMjpgFrame(CompressedFrame_jpg frame)
    {
        if (frame.haveEncoding())
        {
            movieFile_.write(MJPG_BOUNDARY_MARKER.c_str(),MJPG_BOUNDARY_MARKER.size());
            std::vector<uchar> jpgBuffer = frame.getEncodedJpgBuffer();
            std::ofstream::pos_type frameBeginPos = movieFile_.tellp();
            movieFile_.write((const char *) &jpgBuffer[0],jpgBuffer.size());
            std::ofstream::pos_type frameEndPos = movieFile_.tellp();

            std::stringstream ss;
            ss << frame.getFrameCount()  << " "; 
            ss << frame.getTimeStamp()   << " ";
            ss << frameBeginPos          << " ";
            ss << frameEndPos            << std::endl;;
            std::string indexData = ss.str();
            indexFile_.write(indexData.c_str(), indexData.size());
        }
    }

    // Private slots
    // ----------------------------------------------------------------------------------
    void VideoWriter_jpg::onCompressorError(unsigned int errorId, QString errorMsg)
    {
        emit imageLoggingError(errorId, errorMsg);
    }

} // namespace bias
