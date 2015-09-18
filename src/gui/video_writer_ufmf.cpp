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
    const unsigned int VideoWriter_ufmf::FRAMES_TODO_MAX_QUEUE_SIZE   = 250;
    const unsigned int VideoWriter_ufmf::FRAMES_FINISHED_MAX_SET_SIZE = 250;
    const unsigned int VideoWriter_ufmf::FRAMES_WAIT_MAX_QUEUE_SIZE   =  50;

    const unsigned int VideoWriter_ufmf::DEFAULT_FRAME_SKIP = 1;

    const unsigned int VideoWriter_ufmf::DEFAULT_BACKGROUND_THRESHOLD = 40;
    const unsigned int VideoWriter_ufmf::MIN_BACKGROUND_THRESHOLD = 1;
    const unsigned int VideoWriter_ufmf::MAX_BACKGROUND_THRESHOLD = 255;

    const unsigned int VideoWriter_ufmf::DEFAULT_BOX_LENGTH = 30;
    const unsigned int VideoWriter_ufmf::MIN_BOX_LENGTH = 4;
    const unsigned int VideoWriter_ufmf::MAX_BOX_LENGTH = 500;

    const unsigned int VideoWriter_ufmf::DEFAULT_NUMBER_OF_COMPRESSORS = 15;
    const unsigned int VideoWriter_ufmf::MIN_NUMBER_OF_COMPRESSORS = 1;
    const unsigned int VideoWriter_ufmf::BASE_NUMBER_OF_THREADS = 4;

    const bool VideoWriter_ufmf::DEFAULT_DILATE_STATE = true;
    const unsigned int VideoWriter_ufmf::DEFAULT_DILATE_WINDOW_SIZE = 1;
    const unsigned int VideoWriter_ufmf::MIN_DILATE_WINDOW_SIZE = 1;
    const unsigned int VideoWriter_ufmf::MAX_DILATE_WINDOW_SIZE = 20;

    const VideoWriterParams_ufmf VideoWriter_ufmf::DEFAULT_PARAMS = 
        VideoWriterParams_ufmf();

    const QString VideoWriter_ufmf::DEFAULT_COLOR_CODING("MONO8");
    const QString VideoWriter_ufmf::DUMMY_FILENAME("dummy.ufmf");
    const QString VideoWriter_ufmf::UFMF_HEADER_STRING("ufmf");
    const unsigned int VideoWriter_ufmf::UFMF_VERSION_NUMBER = 4;

    const unsigned int VideoWriter_ufmf::KEYFRAME_CHUNK_ID   = 0;
    const unsigned int VideoWriter_ufmf::FRAME_CHUNK_ID      = 1;
    const unsigned int VideoWriter_ufmf::INDEX_DICT_CHUNK_ID = 2;

    const char VideoWriter_ufmf::CHAR_FOR_DICT  = 'd';
    const char VideoWriter_ufmf::CHAR_FOR_ARRAY = 'a';

    const char VideoWriter_ufmf::CHAR_FOR_DTYPE_FLOAT  = 'f'; 
    const char VideoWriter_ufmf::CHAR_FOR_DTYPE_UINT8  = 'B';
    const char VideoWriter_ufmf::CHAR_FOR_DTYPE_UINT64 = 'q';
    const char VideoWriter_ufmf::CHAR_FOR_DTYPE_DOUBLE = 'd';


    // Methods
    // ----------------------------------------------------------------------------------
    VideoWriter_ufmf::VideoWriter_ufmf(QObject *parent) 
        : VideoWriter_ufmf(DEFAULT_PARAMS, DUMMY_FILENAME, 0, parent) 
    {} 


    VideoWriter_ufmf::VideoWriter_ufmf( 
            VideoWriterParams_ufmf params, 
            QString fileName, 
            unsigned int cameraNumber,
            QObject *parent
            ) 
        : VideoWriter(fileName,cameraNumber,parent) 
    {
        isFirst_ = true;

        backgroundThreshold_ = params.backgroundThreshold;
        medianUpdateCount_ = params.medianUpdateCount;
        medianUpdateInterval_ = params.medianUpdateInterval;
        boxLength_ = params.boxLength;
        setFrameSkip(params.frameSkip);
        numberOfCompressors_ = params.numberOfCompressors;
        dilateState_ = params.dilateState;
        dilateWindowSize_ = params.dilateWindowSize; 

        // ----------------------------------------------------------------------------
        std::cout << params.toString() << std::endl;
        // -----------------------------------------------------------------------------

        // Create thread pool for background modelling
        threadPoolPtr_ = new QThreadPool(this);
        unsigned int maxThreadCount = numberOfCompressors_ + BASE_NUMBER_OF_THREADS;
        threadPoolPtr_ -> setMaxThreadCount(maxThreadCount);

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
        unsigned int framesToDoQueueSize;
        unsigned int framesFinishedSetSize;
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
            compressedFrame.dilateEnabled(dilateState_);
            compressedFrame.setDilateWindowSize(dilateWindowSize_);

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
            framesToDoQueueSize = framesToDoQueuePtr_ -> size();
            framesToDoQueuePtr_ -> releaseLock();


        } // if (frameCount_%frameSkip_==0) 

        // Remove frames from compressed frames "finished" set and write to disk 
        framesFinishedSetPtr_ -> acquireLock();
        if (!(framesFinishedSetPtr_ -> empty()))
        {
            bool writeDone = false;
            while ( (!writeDone) && (!(framesFinishedSetPtr_ -> empty())) )
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
                else
                {
                    writeDone = true;
                }
            }

        } // if (!(framesFinishedSetPtr_ 
        framesFinishedSetSize = framesFinishedSetPtr_ -> size();
        framesFinishedSetPtr_ -> releaseLock();
        frameCount_++;

        // Cull framesWaitQueue if it starts to grow too large
        unsigned int framesWaitQueueSize = framesWaitQueuePtr_ -> size();
        if ( framesWaitQueueSize >  FRAMES_WAIT_MAX_QUEUE_SIZE )
        {
            unsigned int numToCull = framesWaitQueueSize - FRAMES_WAIT_MAX_QUEUE_SIZE/2;
            for (unsigned int i=0; i<numToCull; i++)
            {
                framesWaitQueuePtr_ -> pop();
            }
        }

        // Check frames todo and frames finish queue/set size and emit error if
        // they grow too large.

        if (framesToDoQueueSize > FRAMES_TODO_MAX_QUEUE_SIZE) 
        { 
            std::cout << "error: framesToDoQueueSize = " << framesToDoQueueSize << std::endl;
            unsigned int errorId = ERROR_FRAMES_TODO_MAX_QUEUE_SIZE;
            QString errorMsg("logger framesToDoQueue has exceeded the maximum allowed size");
            emit imageLoggingError(errorId, errorMsg);
        }
        if (framesFinishedSetSize > FRAMES_FINISHED_MAX_SET_SIZE)
        {
            std::cout << "error: framesFinishedSetSize = " << framesFinishedSetSize << std::endl;
            unsigned int errorId = ERROR_FRAMES_FINISHED_MAX_SET_SIZE;
            QString errorMsg("logger framesFinishedSet has exceeded the maximum allowed size");
            emit imageLoggingError(errorId, errorMsg);
        }
    }


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
            errorMsg += "image depth must be CV_8U";
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

        // Write index
        // --------------------------------------------------------------------

        // Write index chunk identifier and save index location
        uint8_t chunkId = uint8_t(INDEX_DICT_CHUNK_ID);
        file_.write((char*) &chunkId, sizeof(uint8_t));
        indexLocation_ = file_.tellp();

        // Write char for dict and number of keys
        file_.write((char*) &CHAR_FOR_DICT, sizeof(char));
        uint8_t numKeys = 2;
        file_.write((char*) &numKeys, sizeof(uint8_t));

        // Write index -> frame
        // --------------------------------------------------------------------

        // Write length of key and key for frame
        const char frameString[] = "frame"; 
        uint16_t frameStringLength = uint16_t(sizeof(frameString)-1);
        file_.write((char*) &frameStringLength, sizeof(uint16_t));
        file_.write((char*) frameString, frameStringLength*sizeof(char));

        // Write char for dict and number of keys
        file_.write((char*) &CHAR_FOR_DICT, sizeof(char));
        numKeys = 2;
        file_.write((char*) &numKeys, sizeof(uint8_t));

        // Write index -> frame -> location
        // --------------------------------------------------------------------

        // Write length of key and key for location
        const char locString[] = "loc";
        uint16_t locStringLength = uint16_t(sizeof(locString) - 1);
        file_.write((char*) &locStringLength, sizeof(uint16_t));
        file_.write((char*) locString, locStringLength*sizeof(char));

        // Write char for array and data type
        file_.write((char*) &CHAR_FOR_ARRAY, sizeof(char));
        file_.write((char*) &CHAR_FOR_DTYPE_UINT64, sizeof(char));

        // Write number of bytes and frame positions
        uint32_t numBytes = uint32_t(framePosList_.size()*sizeof(uint64_t)); 
        file_.write((char*) &numBytes, sizeof(uint32_t));
        for ( 
                std::list<std::streampos>::iterator it=framePosList_.begin(); 
                it!=framePosList_.end(); 
                it++
            )
        {
            uint64_t pos = *it;
            file_.write((char*) &pos, sizeof(uint64_t));
        }
        // End write index -> frame -> location
        // --------------------------------------------------------------------

        // Write index -> frame -> timestamp
        // --------------------------------------------------------------------
        
        // Write key length and key for timestamp
        const char timeStampString[] = "timestamp";
        uint16_t timeStampStringLength = uint16_t(sizeof(timeStampString)-1);
        file_.write((char*) &timeStampStringLength, sizeof(uint16_t));
        file_.write((char*) timeStampString, timeStampStringLength*sizeof(char));

        // Write char for array and data type
        file_.write((char*) &CHAR_FOR_ARRAY, sizeof(char));
        file_.write((char*) &CHAR_FOR_DTYPE_DOUBLE, sizeof(char));

        // Write number of bytes and time stamps
        numBytes = uint32_t(frameTimeStampList_.size()*sizeof(double));
        file_.write((char*) &numBytes, sizeof(uint32_t));
        for (
                std::list<double>::iterator it=frameTimeStampList_.begin(); 
                it!=frameTimeStampList_.end(); 
                it++
            )
        { 
            double ts = *it;
            file_.write((char*) &ts, sizeof(double));
        }
        // End write index -> frame -> timestamp
        // --------------------------------------------------------------------

        // End write index -> frame
        // --------------------------------------------------------------------

        // Write index -> keyframe
        // --------------------------------------------------------------------

        // Write key length and key for keyframe
        const char keyFrameString[] = "keyframe";
        uint16_t keyFrameStringLength = uint16_t(sizeof(keyFrameString)-1);
        file_.write((char*) &keyFrameStringLength, sizeof(uint16_t)); 
        file_.write((char*) keyFrameString, keyFrameStringLength*sizeof(char));

        // Write char for dict and number of keys
        file_.write((char*) &CHAR_FOR_DICT, sizeof(char)); 
        numKeys = 1;
        file_.write((char*) &numKeys, sizeof(uint8_t));

        // Write index -> keyframe -> mean
        // --------------------------------------------------------------------

        // Write length of key and key for mean
        const char meanString[] = "mean";
        uint16_t meanStringLength = uint16_t(sizeof(meanString)-1);
        file_.write((char*) &meanStringLength, sizeof(uint16_t));
        file_.write((char*) meanString, meanStringLength*sizeof(char));

        // Write char for dict and number of keys
        file_.write((char*) &CHAR_FOR_DICT, sizeof(char));
        numKeys = 2;
        file_.write((char*) &numKeys, sizeof(uint8_t));

        // Write index -> keyframe -> mean -> loc
        // --------------------------------------------------------------------

        // Write key length and key for location
        file_.write((char*) &locStringLength, sizeof(uint16_t));
        file_.write((char*) locString, locStringLength*sizeof(char));

        // Write char for array and data type
        file_.write((char*) &CHAR_FOR_ARRAY, sizeof(char));
        file_.write((char*) &CHAR_FOR_DTYPE_UINT64, sizeof(char));

        // Write number of bytes and keyframe positions
        numBytes = uint32_t(bgKeyFramePosList_.size()*sizeof(uint64_t));
        file_.write((char*) &numBytes, sizeof(uint32_t));
        for (
                std::list<std::streampos>::iterator it = bgKeyFramePosList_.begin();
                it != bgKeyFramePosList_.end();
                it++
            )
        {
            uint64_t pos = *it;
            file_.write((char*) &pos, sizeof(uint64_t));
        }
        // End write index -> keyframe -> mean -> loc
        // --------------------------------------------------------------------
        
        // Write index -> keyframe -> mean -> timestamp
        // --------------------------------------------------------------------

        // write key length and key for time stamp
        file_.write((char*) &timeStampStringLength, sizeof(uint16_t));
        file_.write((char*) timeStampString, timeStampStringLength*sizeof(char));

        // Write char for array and data type
        file_.write((char*) &CHAR_FOR_ARRAY, sizeof(char));
        file_.write((char*) &CHAR_FOR_DTYPE_DOUBLE, sizeof(char));

        // Write number of bytes and keyframe time stamps
        numBytes = uint32_t(bgKeyFrameTimeStampList_.size()*sizeof(double));
        file_.write((char*) &numBytes, sizeof(uint32_t));
        for (
                std::list<double>::iterator it = bgKeyFrameTimeStampList_.begin(); 
                it != bgKeyFrameTimeStampList_.end();
                it++
            )
        {
            double ts = *it;
            file_.write((char*) &ts, sizeof(double));
        }
        // End write index -> keyframe -> mean -> timestamp
        // --------------------------------------------------------------------

        // End write index -> keyframe -> mean
        // --------------------------------------------------------------------
        
        // End write index -> keyframe
        // --------------------------------------------------------------------

        // End index
        // --------------------------------------------------------------------

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
        std::shared_ptr<std::vector<uint16_t>> writeColBufPtr;
        std::shared_ptr<std::vector<uint16_t>> writeRowBufPtr;
        std::shared_ptr<std::vector<uint16_t>> writeWdtBufPtr;
        std::shared_ptr<std::vector<uint16_t>> writeHgtBufPtr;
        std::shared_ptr<std::vector<uint8_t>> imageDataPtr;

        writeColBufPtr = frame.getWriteColBufPtr();
        writeRowBufPtr = frame.getWriteRowBufPtr();
        writeWdtBufPtr = frame.getWriteWdtBufPtr();
        writeHgtBufPtr = frame.getWriteHgtBufPtr();
        imageDataPtr = frame.getImageDataPtr();

        unsigned int dataPos = 0;

        for (unsigned int cc=0; cc<numConnectedComp; cc++)
        {
            uint16_t col = (*writeColBufPtr)[cc];
            uint16_t row = (*writeRowBufPtr)[cc];
            uint16_t wdt = (*writeWdtBufPtr)[cc];
            uint16_t hgt = (*writeHgtBufPtr)[cc];
            unsigned int boxArea = (*writeHgtBufPtr)[cc]*(*writeWdtBufPtr)[cc];

            file_.write((char*) &col, sizeof(uint16_t));
            file_.write((char*) &row, sizeof(uint16_t));
            file_.write((char*) &wdt, sizeof(uint16_t));
            file_.write((char*) &hgt, sizeof(uint16_t));
            file_.write((char*) &(*imageDataPtr)[dataPos], boxArea*sizeof(uint8_t));
            dataPos += boxArea;
        }

        // Calculate frame size
        std::streampos filePosEnd = file_.tellp();
        unsigned long frameSize = (unsigned long)(filePosEnd) - (unsigned long)(filePosBegin);
    }


    void VideoWriter_ufmf::writeKeyFrame()
    {
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

        // Write char specifying data type
        file_.write((char*) &CHAR_FOR_DTYPE_UINT8, sizeof(char));

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
                bgOldDataQueuePtr_,
                cameraNumber_
                );

        bgHistogramPtr_ -> setMedianUpdateCount(medianUpdateCount_);
        bgHistogramPtr_ -> setMedianUpdateInterval(medianUpdateInterval_);

        bgMedianPtr_ = new BackgroundMedian_ufmf(
                bgNewDataQueuePtr_,
                bgOldDataQueuePtr_,
                medianMatQueuePtr_,
                cameraNumber_ 
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
                    framesFinishedSetPtr_,
                    cameraNumber_
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
