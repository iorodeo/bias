#ifndef BIAS_BACKGROUND_HISTOGRAM_UFMF_HPP
#define BIAS_BACKGROUND_HISTOGRAM_UFMF_HPP
#include <memory>
#include <QObject>
#include <QRunnable>
#include "lockable.hpp"

namespace bias
{
    struct StampedImage;

    class BackgroundHistogram_ufmf 
        : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            BackgroundHistogram_ufmf(QObject *parent=0);

            BackgroundHistogram_ufmf(
                    std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr, 
                    QObject *parent=0
                    );

            void initialize(
                    std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr 
                    );

            void stop();

        private:
            bool ready_;
            bool stopped_;

            std::shared_ptr<LockableQueue<StampedImage>> bgImageQueuePtr_;

            std::shared_ptr<unsigned int>  binPtr_;
            std::shared_ptr<unsigned long> cntPtr_;

            unsigned int numRows_;
            unsigned int numCols_;
            unsigned int numBins_;
            unsigned int binSize_;

            void run();
            void createHistogram(StampedImage &stampedImg);
            void addImageToHistogram(StampedImage &stampedImg);
    };

}

#endif // #ifndef BAIS_BACKGOUND_HISTOGRAM_UFMF_HPP
