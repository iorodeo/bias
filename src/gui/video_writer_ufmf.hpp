#ifndef BIAS_VIDEO_WRITER_UFMF_HPP
#define BIAS_VIDEO_WRITER_UFMF_HPP

#include "video_writer.hpp"
#include "compressor_ufmf.hpp"
#include "compressed_frame_ufmf.hpp"
#include <memory>
#include <vector>
#include <list>
#include <QPointer>
#include <opencv2/core/core.hpp>
#include <fstream>

class QThreadPool;

namespace bias
{
    class BackgroundData_ufmf;
    class BackgroundHistogram_ufmf;
    class BackgroundMedian_ufmf;
    template <class T> class Lockable;
    template <class T> class LockableQueue;

    class VideoWriter_ufmf : public VideoWriter
    {
        Q_OBJECT

        public:

            VideoWriter_ufmf(QObject *parent=0);
            VideoWriter_ufmf(QString fileName, QObject *parent=0);
            virtual ~VideoWriter_ufmf();
            virtual void addFrame(StampedImage stampedImg);

            //// Debug ----------------
            //cv::Mat currentImage_;
            //void updateMembershipImage();
            //cv::Mat getMembershipImage();
            //// ----------------------

            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int DEFAULT_BACKGROUND_THRESHOLD;
            static const unsigned int DEFAULT_UFMF_BOX_LENGTH;
            static const unsigned int DEFAULT_NUMBER_OF_COMPRESSORS;
            static const unsigned int DEFAULT_MAX_THREAD_COUNT;
            static const QString DEFAULT_COLOR_CODING;
            static const QString DUMMY_FILENAME;
            static const QString UFMF_HEADER_STRING;
            static const unsigned int UFMF_VERSION_NUMBER;

            static const unsigned int KEYFRAME_CHUNK_ID;
            static const unsigned int FRAME_CHUNK_ID;
            static const unsigned int INDEX_DICT_CHUNK_ID;

        protected:

            bool isFirst_;
            unsigned int backgroundThreshold_;
            unsigned int numberOfCompressors_;
            unsigned int boxLength_;
            bool isFixedSize_;
            QString colorCoding_;

            StampedImage currentImage_;

            std::fstream file_;
            std::streampos indexLocationPtr_;
            unsigned long indexLocation_;
            unsigned long nextFrameToWrite_;
            unsigned long numKeyFramesWritten_;

            unsigned long bgUpdateCount_;
            unsigned long bgModelFrameCount_;
            double bgModelTimeStamp_;

            std::list<std::streampos> bgKeyFramePosList_; 
            std::list<double> bgKeyFrameTimeStampList_;

            cv::Mat bgMedianImage_;
            cv::Mat bgUpperBoundImage_;
            cv::Mat bgLowerBoundImage_;
            cv::Mat bgMembershipImage_;

            QPointer<QThreadPool> threadPoolPtr_;
            QPointer<BackgroundHistogram_ufmf> bgHistogramPtr_;
            QPointer<BackgroundMedian_ufmf> bgMedianPtr_;

            std::vector<QPointer<Compressor_ufmf>> compressorPtrVec_;

            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr_;
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr_;
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr_;
            std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr_;

            CompressedFrameQueuePtr_ufmf framesToDoQueuePtr_;
            CompressedFrameQueuePtr_ufmf framesWaitQueuePtr_;
            CompressedFrameSetPtr_ufmf framesFinishedSetPtr_;

            void checkImageFormat(StampedImage stampedImg);
            void setupOutputFile(StampedImage stampedImg);
            void writeHeader();
            void writeKeyFrame();
            void writeCompressedFrame(CompressedFrame_ufmf frame);
            void finishWriting();

            void startBackgroundModeling();
            void stopBackgroundModeling();
            void startCompressors();
            void stopCompressors();
    };
}

#endif // #ifndef BIAS_VIDEO_WRITER_UFMF_HPP
