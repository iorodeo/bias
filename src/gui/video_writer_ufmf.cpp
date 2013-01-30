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

    const unsigned int VideoWriter_ufmf::KEYFRAME_CHUNK_ID = 0;
    const unsigned int VideoWriter_ufmf::FRAME_CHUNK_ID = 1;
    const unsigned int VideoWriter_ufmf::INDEX_DICT_CHUNK_ID = 2;

    const char VideoWriter_ufmf::CHAR_FOR_DICT = 'd';
    const char VideoWriter_ufmf::CHAR_FOR_ARRAY = 'a';


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

        // Create "to do" queue and "finished" set for frame compressors
        framesToDoQueuePtr_ = std::make_shared<CompressedFrameQueue_ufmf>();
        framesWaitQueuePtr_ = std::make_shared<CompressedFrameQueue_ufmf>();
        framesFinishedSetPtr_ = std::make_shared<CompressedFrameSet_ufmf>();

        isFixedSize_ = false;
        boxLength_ = DEFAULT_UFMF_BOX_LENGTH;
        colorCoding_ = QString(DEFAULT_COLOR_CODING);

        indexLocation_ = 0;
        nextFrameToWrite_ = 0;
        numKeyFramesWritten_ = 0;
        bgUpdateCount_ = 0;
        bgModelFrameCount_ = 0;
        bgModelTimeStamp_ = 0.0;

    }


    VideoWriter_ufmf::~VideoWriter_ufmf() 
    {
        stopBackgroundModeling();
        stopCompressors();
        threadPoolPtr_ -> waitForDone();
        finishWriting();
    } 


    void VideoWriter_ufmf::addFrame(StampedImage stampedImg) 
    {
        bool haveNewMedianImage = false;

        //currentImage_ = stampedImg.image;
        currentImage_ = stampedImg;

        // On first call - setup output file, background modeling, start compressors, ...
        if (isFirst_)
        {
            checkImageFormat(stampedImg);

            // Set output file and write header
            setupOutputFile(stampedImg);
            writeHeader();

            // Set initial bg median image - just use current image.
            bgMedianImage_ = stampedImg.image;
            bgMembershipImage_.create(stampedImg.image.rows, stampedImg.image.cols,CV_8UC1);
            cv::add(bgMedianImage_,  backgroundThreshold_, bgUpperBoundImage_);
            cv::subtract(bgMedianImage_, backgroundThreshold_, bgLowerBoundImage_); 

            // Start background model and frame compressors
            startBackgroundModeling();
            startCompressors();
            writeKeyFrame();

            isFirst_ = false;
        }


        // Process frames or skip 
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
            if (!(medianMatQueuePtr_ -> empty()))
            {
                bgMedianImage_ = medianMatQueuePtr_ -> front();
                medianMatQueuePtr_ -> pop();
                haveNewMedianImage = true;
            }
            medianMatQueuePtr_ -> releaseLock();

            // When new median image available re-calculate thresholds
            if (haveNewMedianImage)
            {
                cv::add(bgMedianImage_,  backgroundThreshold_, bgUpperBoundImage_);
                cv::subtract(bgMedianImage_, backgroundThreshold_, bgLowerBoundImage_); 

                bgUpdateCount_++;
                bgModelTimeStamp_ = currentImage_.timeStamp;
                bgModelFrameCount_ = currentImage_.frameCount;
                writeKeyFrame();
            }

            // Create compressed frame and set its data using the current frame 
            CompressedFrame_ufmf compressedFrame(boxLength_);

            if (!(framesWaitQueuePtr_ -> empty()))
            {
                // Take pre-allocated compressed frame if available
                compressedFrame = framesWaitQueuePtr_ -> front();
                framesWaitQueuePtr_ -> pop();
            }

            compressedFrame.setData(
                    currentImage_, 
                    bgLowerBoundImage_, 
                    bgUpperBoundImage_,
                    bgUpdateCount_
                    );

            // Insert new (uncalculated) compressed frame into "to do" queue.
            framesToDoQueuePtr_ -> acquireLock();
            framesToDoQueuePtr_ -> push(compressedFrame);
            framesToDoQueuePtr_ -> wakeOne();
            framesToDoQueuePtr_ -> releaseLock();


        } // if (frameCount_%frameSkip_==0) 

        // Remove frames from compressed frames "finished" set and write to disk 
        framesFinishedSetPtr_ -> acquireLock();
        if (!(framesFinishedSetPtr_ -> empty()))
        {
            CompressedFrameSet_ufmf::iterator it = framesFinishedSetPtr_ -> begin();
            CompressedFrame_ufmf compressedFrame = *it;

            if (compressedFrame.getFrameCount() == nextFrameToWrite_)
            {
                framesWaitQueuePtr_ -> push(compressedFrame);
                framesFinishedSetPtr_ -> erase(it);
                nextFrameToWrite_ += frameSkip_;
                writeCompressedFrame(compressedFrame);
            }

        }
        framesFinishedSetPtr_ -> releaseLock();

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

            uint32_t ufmf_version = uint32_t(UFMF_VERSION_NUMBER);
            file_.write((char*) &ufmf_version, sizeof(uint32_t));

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
                uint16_t width = uint16_t(size_.width);
                file_.write((char*) &width, sizeof(uint16_t));

                uint16_t height = uint16_t(size_.height);
                file_.write((char*) &height, sizeof(uint16_t));
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

    void VideoWriter_ufmf::finishWriting()
    {
        // Move to end of filed
        file_.seekp(0, std::ios_base::end);

        // Write index chunk identifier
        uint8_t chunkId = uint8_t(INDEX_DICT_CHUNK_ID);
        file_.write((char*) &chunkId, sizeof(uint8_t));

        // Save index location
        indexLocation_ = (unsigned long)(file_.tellp());

        // Write char for dict
        file_.write((char*) &CHAR_FOR_DICT, sizeof(char));

        // Write number of keys
        uint8_t numKeys = 2;
        file_.write((char*) &numKeys, sizeof(uint8_t));

        // Write index -> frame

        // Write length of key and key
        const char frameString[] = "frame"; 
        uint16_t frameStringLength = sizeof(frameString)-1;
        file_.write((char*) &frameStringLength, sizeof(uint16_t));
        file_.write((char*) frameString, frameStringLength*sizeof(char));

        // Write char for dict
        file_.write((char*) &CHAR_FOR_DICT, sizeof(char));

        // Write number of keys
        file_.write((char*) &numKeys, sizeof(uint8_t));

        // Write index -> frame -> location

        // Write length of key and key
        const char locString[] = "loc";
        uint16_t locStringLength = sizeof(locString) - 1;
        file_.write((char*) &locStringLength, sizeof(uint16_t));
        file_.write((char*) locString, locStringLength*sizeof(char));

        // Write char for array
        file_.write((char*) &CHAR_FOR_ARRAY, sizeof(char));

        // Write data type
        char charForDataType = 'q';
        file_.write((char*) &charForDataType, sizeof(char));

        // -----------------------------------------------
        // TO DO - finish writing index 
        // -----------------------------------------------

        // End write index -> frame -> location

        // Write index -> frame -> timestamp
        // End write index -> frame -> timestamp


        // End write index -> frame

        // Write the index location
        file_.seekp(indexLocationPtr_, std::ios_base::beg);
        uint64_t indexLocation_uint64 = uint64_t(indexLocation_);
        file_.write((char*) &indexLocation_uint64, sizeof(uint64_t));

        // Close the file
        file_.close();
    }


    void VideoWriter_ufmf::writeCompressedFrame(CompressedFrame_ufmf frame)
    {

        if (!frame.isReady()) { return; }

        // Get position and time stamp for index
        double timeStamp = frame.getTimeStamp();
        std::streampos filePosBegin = file_.tellp();

        framePosList_.push_back(filePosBegin);
        frameTimeStampList_.push_back(timeStamp);

        // Write keyframe chunk identifier
        uint8_t chunkId = uint8_t(FRAME_CHUNK_ID);
        file_.write((char*) &chunkId, sizeof(uint8_t));

        // Write time stamp
        file_.write((char*) &timeStamp, sizeof(double));

        // Write number of connected components
        uint32_t numConnectedComp = uint32_t(frame.getNumConnectedComp());
        file_.write((char*) &numConnectedComp, sizeof(uint32_t));

        // Write each box
        std::shared_ptr<std::vector<uint16_t>> writeRowBufPtr;
        std::shared_ptr<std::vector<uint16_t>> writeColBufPtr;
        std::shared_ptr<std::vector<uint16_t>> writeHgtBufPtr;
        std::shared_ptr<std::vector<uint16_t>> writeWdtBufPtr;
        std::shared_ptr<std::vector<uint8_t>> imageDataPtr;

        writeRowBufPtr = frame.getWriteRowBufPtr();
        writeColBufPtr = frame.getWriteColBufPtr();
        writeHgtBufPtr = frame.getWriteHgtBufPtr();
        writeWdtBufPtr = frame.getWriteWdtBufPtr();
        imageDataPtr = frame.getImageDataPtr();

        unsigned int dataPos = 0;

        for (unsigned int cc=0; cc<numConnectedComp; cc++)
        {
            uint16_t row = (*writeRowBufPtr)[cc];
            uint16_t col = (*writeColBufPtr)[cc];
            uint16_t wdt = (*writeWdtBufPtr)[cc];
            uint16_t hgt = (*writeHgtBufPtr)[cc];
            unsigned int boxArea = (*writeHgtBufPtr)[cc]*(*writeWdtBufPtr)[cc];

            file_.write((char*) &row, sizeof(uint16_t));
            file_.write((char*) &col, sizeof(uint16_t));
            file_.write((char*) &wdt, sizeof(uint16_t));
            file_.write((char*) &hgt, sizeof(uint16_t));
            file_.write((char*) &(*imageDataPtr)[dataPos], boxArea*sizeof(uint8_t));
            dataPos += boxArea;
        }

        // Calculate frame size
        std::streampos filePosEnd = file_.tellp();
        unsigned long frameSize = (unsigned long)(filePosEnd) - (unsigned long)(filePosBegin);
        //std::cout << "writing compressed frame: size = " << frameSize << std::endl;
    }


    void VideoWriter_ufmf::writeKeyFrame()
    {
        //std::cout << "writing key frame" << std::endl;

        // Get position and time stamp for index
        bgKeyFramePosList_.push_back(file_.tellp());
        bgKeyFrameTimeStampList_.push_back(bgModelTimeStamp_);

        // Write keyframe chunk identifier
        uint8_t chunkId = uint8_t(KEYFRAME_CHUNK_ID);
        file_.write((char*) &chunkId, sizeof(uint8_t));

        // Write keyframe type
        const char keyFrameType[] = "mean";
        uint8_t keyFrameTypeLength = sizeof(keyFrameType)-1;
        file_.write((char*) &keyFrameTypeLength, sizeof(uint8_t));
        file_.write((char*) keyFrameType, keyFrameTypeLength*sizeof(char));

        // Discrepancy ... what about number of points/boxes

        // Write data type
        const char dataType = 'B'; // 'B' for uint8, 'f' for float
        file_.write((char*) &dataType, sizeof(char));

        // Write width and height
        uint16_t width = uint16_t(bgMedianImage_.cols);
        file_.write((char*) &width, sizeof(uint16_t));

        uint16_t height = uint16_t(bgMedianImage_.rows);
        file_.write((char*) &height, sizeof(uint16_t));

        // Write timestamp
        file_.write((char*) &bgModelTimeStamp_, sizeof(double));

        // Write the frame data
        unsigned int numPixel = bgMedianImage_.rows*bgMedianImage_.cols;
        file_.write((char*) bgMedianImage_.data, numPixel*sizeof(char));

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
