#ifndef BIAS_BACKGROUND_MEDIAN_UFMF_HPP
#define BIAS_BACKGROUND_MEDIAN_UFMF_HPP
#include <memory>
#include <QObject>
#include <QRunnable>
#include "lockable.hpp"

namespace cv {class Mat;}

namespace bias
{
    class BackgroundData_ufmf;


    class BackgroundMedian_ufmf
        : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            BackgroundMedian_ufmf(QObject *parent=0);
            BackgroundMedian_ufmf( 
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
                    std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr,
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );
            void initialize(
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
                    std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr,
                    unsigned int cameraNumber
                    );
            void stop();

        private:
            bool ready_;
            bool stopped_;
            unsigned int cameraNumber_;

            // Queues of incoming and outgoing background data for median calculation
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr_;
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr_;
            std::shared_ptr<LockableQueue<cv::Mat>> medianMatQueuePtr_;
            void run();

    };


} // namespace bias

#endif // #ifndef BAIS_BACKGROUND_MEDAIN_HPP
