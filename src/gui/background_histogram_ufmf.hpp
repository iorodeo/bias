#ifndef BIAS_BACKGROUND_HISTOGRAM_UFMF_HPP
#define BIAS_BACKGROUND_HISTOGRAM_UFMF_HPP
#include <memory>
#include <QObject>
#include <QRunnable>
#include "lockable.hpp"

namespace bias
{
    struct StampedImage;
    class BackgroundData_ufmf;

    class BackgroundHistogram_ufmf 
        : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            BackgroundHistogram_ufmf(QObject *parent=0);
            BackgroundHistogram_ufmf(
                    std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr, 
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );
            void initialize(
                    std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr,
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr,
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr,
                    unsigned int cameraNumber
                    );
            void stop();
            void setMedianUpdateCount(unsigned int medianUpdateCount);
            void setMedianUpdateInterval(unsigned int medianUpdateInterval);

            static const unsigned int DEFAULT_NUM_BINS; 
            static const unsigned int DEFAULT_BIN_SIZE; 
            static const unsigned int DEFAULT_MEDIAN_UPDATE_COUNT;
            static const unsigned int MIN_MEDIAN_UPDATE_COUNT;
            static const unsigned int DEFAULT_MEDIAN_UPDATE_INTERVAL;
            static const unsigned int MIN_MEDIAN_UPDATE_INTERVAL;

        private:

            bool ready_;
            bool stopped_;
            unsigned int cameraNumber_;
            unsigned int medianUpdateCount_;
            unsigned int medianUpdateInterval_;

            // Queue of incoming images for background model
            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr_;       

            // Queue of outgoing and incoming background data for median calculations
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgNewDataQueuePtr_;
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgOldDataQueuePtr_;

            void run();
    };

}

#endif // #ifndef BAIS_BACKGOUND_HISTOGRAM_UFMF_HPP
