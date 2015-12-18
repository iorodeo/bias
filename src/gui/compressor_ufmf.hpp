#ifndef BIAS_COMPRESSOR_UFMF
#define BIAS_COMPRESSOR_UFMF

#include <QObject>
#include <QRunnable>
#include <memory>
#include "lockable.hpp"
#include "compressed_frame_ufmf.hpp"

namespace bias
{

    class Compressor_ufmf : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:

            Compressor_ufmf(QObject *parent=0);

            Compressor_ufmf(
                    CompressedFrameQueuePtr_ufmf framesToDoQueuePtr,
                    CompressedFrameSetPtr_ufmf framesFinishedSetPtr,
                    std::shared_ptr<Lockable<std::list<unsigned long>>> framesSkippedIndexListPtr,
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );

            void stop();


        signals:
            void imageLoggingError(unsigned int errorId, QString errorMsg);


        private:

            bool ready_;
            bool stopped_;
            bool skipReported_;
            unsigned int cameraNumber_;

            CompressedFrameQueuePtr_ufmf framesToDoQueuePtr_;
            CompressedFrameSetPtr_ufmf framesFinishedSetPtr_;
            std::shared_ptr<Lockable<std::list<unsigned long>>> framesSkippedIndexListPtr_;

            void initialize(
                    CompressedFrameQueuePtr_ufmf framesToDoQueuePtr,
                    CompressedFrameSetPtr_ufmf framesFinishedSetPtr,
                    std::shared_ptr<Lockable<std::list<unsigned long>>> framesSkippedIndexListPtr,
                    unsigned int cameraNumber
                    );

            void run();

    };

} // namespace bias

#endif // #ifndef BIAS_COMPRESSOR_UFMF
