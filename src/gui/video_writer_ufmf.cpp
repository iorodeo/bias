#include "video_writer_ufmf.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include "lockable.hpp"
#include "background_data_ufmf.hpp"
#include "background_histogram_ufmf.hpp"
#include "background_median_ufmf.hpp"
#include <QThreadPool>
#include <QFileInfo>
#include <QDir>
#include <iostream>

namespace bias
{
    // Static Constants
    // ----------------------------------------------------------------------------------
    const unsigned int VideoWriter_ufmf::DEFAULT_FRAME_SKIP = 1;
    const unsigned int VideoWriter_ufmf::DEFAULT_BACKGROUND_THRESHOLD = 40;
    const unsigned int VideoWriter_ufmf::DEFAULT_UFMF_BOX_LENGTH = 30;
    const unsigned int VideoWriter_ufmf::DEFAULT_NUMBER_OF_COMPRESSORS = 15;
    const unsigned int VideoWriter_ufmf::DEFAULT_MAX_THREAD_COUNT = 
        DEFAULT_NUMBER_OF_COMPRESSORS + 4;
    const QString VideoWriter_ufmf::DEFAULT_COLOR_CODING("MONO8");
    const QString VideoWriter_ufmf::DUMMY_FILENAME("dummy.ufmf");
    const QString VideoWriter_ufmf::UFMF_HEADER_STRING("ufmf");
    const unsigned int VideoWriter_ufmf::UFMF_VERSION_NUMBER = 4;


    // Methods
    // ----------------------------------------------------------------------------------
    VideoWriter_ufmf::VideoWriter_ufmf(QObject *parent) 
        : VideoWriter_ufmf(DUMMY_FILENAME, parent) 
    {} 


    VideoWriter_ufmf::VideoWriter_ufmf(QString fileName, QObject *parent) 
        : VideoWriter(fileName,parent) 
    {
        isFirst_ = true;
        backgroundThreshold_ = DEFAULT_BACKGROUND_THRESHOLD;
        numberOfCompressors_ = DEFAULT_NUMBER_OF_COMPRESSORS;
        setFrameSkip(DEFAULT_FRAME_SKIP);

        // Create thread pool for background modelling
        threadPoolPtr_ = new QThreadPool(this);
        threadPoolPtr_ -> setMaxThreadCount(DEFAULT_MAX_THREAD_COUNT);

        // Create queue for images sent to background modeler
        bgImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();
        bgNewDataQueuePtr_ = std::make_shared<LockableQueue<BackgroundData_ufmf>>();
        bgOldDataQueuePtr_ = std::make_shared<LockableQueue<BackgroundData_ufmf>>();
        medianMatQueuePtr_ = std::make_shared<LockableQueue<cv::Mat>>();

        // Create queue and set for frame compression
        framesToDoQueuePtr_ = std::make_shared<CompressedFrameQueue_ufmf>();
        framesWaitQueuePtr_ = std::make_shared<CompressedFrameQueue_ufmf>();
        framesFinishedSetPtr_ = std::make_shared<CompressedFrameSet_ufmf>();

        indexLocation_ = 0;
        isFixedSize_ = false;
        boxLength_ = DEFAULT_UFMF_BOX_LENGTH;
        colorCoding_ = QString(DEFAULT_COLOR_CODING);

    }


    VideoWriter_ufmf::~VideoWriter_ufmf() 
    {
        stopBackgroundModeling();
        stopCompressors();
        threadPoolPtr_ -> waitForDone();
    } 


    void VideoWriter_ufmf::addFrame(StampedImage stampedImg) 
    {
        bool haveNewMedianImage = false;

        //currentImage_ = stampedImg.image;
        currentImage_ = stampedImg;

        // On first call - setup output file, background modeling, etc
        if (isFirst_)
        {
            checkImageFormat(stampedImg);
            setupOutputFile(stampedImg);
            writeHeader();

            bgMedianImage_ = stampedImg.image;
            bgMembershipImage_.create(stampedImg.image.rows, stampedImg.image.cols,CV_8UC1);
            cv::add(bgMedianImage_,  backgroundThreshold_, bgUpperBoundImage_);
            cv::subtract(bgMedianImage_, backgroundThreshold_, bgLowerBoundImage_); 

            startBackgroundModeling();
            startCompressors();

            isFirst_ = false;
        }


        // Process frame on every frame count divisible by the frame skip parameter
        if (frameCount_%frameSkip_==0)
        {

            // Add frame to background image queue if it is empty 
            bgImageQueuePtr_ -> acquireLock();
            if (bgImageQueuePtr_ -> empty())
            {
                bgImageQueuePtr_ -> push(stampedImg);
                bgImageQueuePtr_ -> signalNotEmpty();
            }
            bgImageQueuePtr_ -> releaseLock();

            // Get median image if available
            medianMatQueuePtr_ -> acquireLock();
            if ( !(medianMatQueuePtr_ -> empty()))
            {
                bgMedianImage_ = medianMatQueuePtr_ -> front();
                medianMatQueuePtr_ -> pop();
                haveNewMedianImage = true;
                //std::cout << " *** got median image " << std::endl;
            }
            medianMatQueuePtr_ -> releaseLock();

            // When new median image available re-calculate thresholds
            if (haveNewMedianImage)
            {
                cv::add(bgMedianImage_,  backgroundThreshold_, bgUpperBoundImage_);
                cv::subtract(bgMedianImage_, backgroundThreshold_, bgLowerBoundImage_); 
            }

            // Create compressed frame and set its data using the current frame 
            CompressedFrame_ufmf compressedFrame(boxLength_);

            if (!(framesWaitQueuePtr_ -> empty()))
            {
                // Take pre-allocate frame if available
                compressedFrame = framesWaitQueuePtr_ -> front();
                framesWaitQueuePtr_ -> pop();
            }
            //std::cout << "ufmf frames wait size: " << framesWaitQueuePtr_ -> size() << std::endl;
            compressedFrame.setData(currentImage_, bgLowerBoundImage_, bgUpperBoundImage_);

            // Insert new (uncalculated) compressed frame int "to do" queue.
            //std::cout << "ufmf todo queue size: " << framesToDoQueuePtr_ -> size() << std::endl;
            framesToDoQueuePtr_ -> acquireLock();
            framesToDoQueuePtr_ -> push(compressedFrame);
            framesToDoQueuePtr_ -> wakeOne();
            framesToDoQueuePtr_ -> releaseLock();


        } // if (frameCount_%frameSkip_==0) 

        // Remove frames form "finished" set
        framesFinishedSetPtr_ -> acquireLock();
        if (!(framesFinishedSetPtr_ -> empty()))
        {
            CompressedFrameSet_ufmf::iterator it = framesFinishedSetPtr_ -> begin();
            CompressedFrame_ufmf compressedFrame = *it;
            framesWaitQueuePtr_ -> push(compressedFrame);
            framesFinishedSetPtr_ -> erase(it);
        }
        framesFinishedSetPtr_ -> releaseLock();
        //std::cout << "ufmf set size: " << framesFinishedSetPtr_ -> size() << std::endl;

        frameCount_++;
    }


    //void VideoWriter_ufmf::updateMembershipImage()
    //{
    //    compressedFrame_.setData(currentImage_, bgLowerBoundImage_, bgUpperBoundImage_);
    //    bgMembershipImage_ = compressedFrame_.getMembershipImage();
    //    //cv::inRange(currentImage_.image, bgLowerBoundImage_, bgUpperBoundImage_, bgMembershipImage_);
    //}

    //cv::Mat VideoWriter_ufmf::getMembershipImage()
    //{
    //    updateMembershipImage();
    //    return bgMembershipImage_;
    //}


    void VideoWriter_ufmf::checkImageFormat(StampedImage stampedImg)
    {
        if (stampedImg.image.channels() != 1)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer ufmf setup failed:\n\n"); 
            errorMsg += "images must be single channel";
            throw RuntimeError(errorId,errorMsg);
        }

        if (stampedImg.image.depth() != CV_8U)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer ufmf setup failed:\n\n"); 
            errorMsg += "image depth must be CV_8U or CV_16U";
            throw RuntimeError(errorId,errorMsg);
        }
    }


    void VideoWriter_ufmf::setupOutputFile(StampedImage stampedImg) 
    {

        // Set error control state, set exceptions mask
        file_.clear();
        file_.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        // Get unique name for file and open for reading
        QString incrFileName = getUniqueFileName();

        try
        {
            file_.open(incrFileName.toStdString(), std::ios::binary | std::ios::out);
        }
        catch (std::ifstream::failure &exc)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg); 
        }

        if (!file_.is_open())
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to open file:\n\n"); 
            errorMsg += "no exception thrown";
            throw RuntimeError(errorId, errorMsg); 
        }
        setSize(stampedImg.image.size());

    }


    void VideoWriter_ufmf::writeHeader()
    {
        try 
        {
            
            QByteArray headerStrArray = UFMF_HEADER_STRING.toLatin1();
            unsigned int headerStrLen = UFMF_HEADER_STRING.size();
            file_.write((char*) headerStrArray.data(), headerStrLen*sizeof(char));

            uint32_t ufmf_version_uint32 = uint32_t(UFMF_VERSION_NUMBER);
            file_.write((char*) &ufmf_version_uint32, sizeof(uint32_t));

            indexLocationPtr_ = file_.tellp();
            uint64_t indexLocation_uint64 = uint64_t(indexLocation_);
            file_.write((char*) &indexLocation_uint64, sizeof(uint64_t));

            if (isFixedSize_)
            {
                uint16_t boxLength_uint16 = uint16_t(boxLength_);
                file_.write((char*) &boxLength_uint16, sizeof(uint16_t)); 
                file_.write((char*) &boxLength_uint16, sizeof(uint16_t));
            }
            else
            {
                uint16_t width_uint16 = uint16_t(size_.width);
                file_.write((char*) &width_uint16, sizeof(uint16_t));

                uint16_t height_uint16 = uint16_t(size_.height);
                file_.write((char*) &height_uint16, sizeof(uint16_t));
            }

            uint8_t isFixedSize_uint8 = uint8_t(isFixedSize_);
            file_.write((char*) &isFixedSize_uint8, sizeof(uint8_t));

            uint8_t colorCodingLength = uint8_t(colorCoding_.size());
            file_.write((char*) &colorCodingLength, sizeof(uint8_t));

            QByteArray colorCodingArray = colorCoding_.toLatin1();
            file_.write((char*) colorCodingArray.data(), colorCodingLength*sizeof(char));
        }
        catch (std::ifstream::failure &exc)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
            std::string errorMsg("video writer unable to write ufmf header:\n\n"); 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg); 
        }
    }


    void VideoWriter_ufmf::startBackgroundModeling()
    {
        bgImageQueuePtr_ -> clear();
        bgNewDataQueuePtr_ -> clear();
        bgOldDataQueuePtr_ -> clear();
        medianMatQueuePtr_ -> clear();

        bgHistogramPtr_ = new BackgroundHistogram_ufmf(
                bgImageQueuePtr_,
                bgNewDataQueuePtr_,
                bgOldDataQueuePtr_
                );

        bgMedianPtr_ = new BackgroundMedian_ufmf(
                bgNewDataQueuePtr_,
                bgOldDataQueuePtr_,
                medianMatQueuePtr_
                );

        threadPoolPtr_ -> start(bgHistogramPtr_);
        threadPoolPtr_ -> start(bgMedianPtr_);
    }


    void VideoWriter_ufmf::stopBackgroundModeling()
    {
        // Signal for background modeling threads to stop
        if (!bgMedianPtr_.isNull())
        {
            bgMedianPtr_ -> acquireLock();
            bgMedianPtr_ -> stop();
            bgMedianPtr_ -> releaseLock();

            bgNewDataQueuePtr_ -> acquireLock();
            bgNewDataQueuePtr_ -> signalNotEmpty();
            bgNewDataQueuePtr_ -> releaseLock();
        }

        if (!bgHistogramPtr_.isNull()) 
        {
            bgHistogramPtr_ -> acquireLock();
            bgHistogramPtr_ -> stop();
            bgHistogramPtr_ -> releaseLock();

            bgImageQueuePtr_ -> acquireLock();
            bgImageQueuePtr_ -> signalNotEmpty();
            bgImageQueuePtr_ -> releaseLock();
        }

    }

    void VideoWriter_ufmf::startCompressors()
    {
        framesToDoQueuePtr_ -> clear();
        framesFinishedSetPtr_ -> clear();

        // Create compressor threads and start on thread pool
        compressorPtrVec_.resize(numberOfCompressors_);
        for (unsigned int i=0; i<compressorPtrVec_.size(); i++)
        {
            compressorPtrVec_[i] = new Compressor_ufmf(
                    framesToDoQueuePtr_,
                    framesFinishedSetPtr_
                    );
            threadPoolPtr_ -> start(compressorPtrVec_[i]);
        }
    }

    void VideoWriter_ufmf::stopCompressors()
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


} // namespace bias
