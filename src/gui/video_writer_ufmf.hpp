#ifndef BIAS_VIDEO_WRITER_UFMF_HPP
#define BIAS_VIDEO_WRITER_UFMF_HPP

#include "video_writer.hpp"
#include <memory>
#include <QPointer>
#include <opencv2/core/core.hpp>

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

            // Debug ----------------
            void loopTest();
            void updateMembershipImage();
            cv::Mat getMembershipImage();
            // ----------------------

            static const unsigned int MAX_THREAD_COUNT;
            static const QString DUMMY_FILENAME;
            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int DEFAULT_BACKGROUND_THRESHOLD;

        protected:

            bool isFirst_;
            unsigned int backgroundThreshold_;

            cv::Mat currentImage_;
            cv::Mat bgMedianImage_;
            cv::Mat bgUpperBoundImage_;
            cv::Mat bgLowerBoundImage_;
            cv::Mat bgMembershipImage_;

            QPointer<QThreadPool> threadPoolPtr_;
            QPointer<BackgroundHistogram_ufmf> bgHistogramPtr_;
            QPointer<BackgroundMedian_ufmf> bgMedianPtr_;

            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr_;
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr_;
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr_;
            std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr_;

            void checkImageFormat(StampedImage stampedImg);
            void setupOutput(StampedImage stampedImg);

            void startBackgroundModeling();
            void stopBackgroundModeling();
    };
}

#endif // #ifndef BIAS_VIDEO_WRITER_UFMF_HPP
