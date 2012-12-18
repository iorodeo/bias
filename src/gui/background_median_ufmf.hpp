#ifndef BIAS_BACKGROUND_MEDIAN_UFMF_HPP
#define BIAS_BACKGROUND_MEDIAN_UFMF_HPP
#include <memory>
#include <QObject>
#include <QRunnable>
#include "lockable.hpp"

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
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr,
                    QObject *parent=0
                    );
            void initialize(
                    std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr
                    );
            void stop();

        private:
            bool ready_;
            bool stopped_;
            // Queue of incoming background data for median calculation
            std::shared_ptr<LockableQueue<BackgroundData_ufmf>> bgDataQueuePtr_;

            void run();

    };


} // namespace bias

#endif // #ifndef BAIS_BACKGROUND_MEDAIN_HPP
