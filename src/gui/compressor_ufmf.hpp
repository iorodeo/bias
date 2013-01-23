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
                    CompressedFrameQueuePtr_ufmf cmpFrameWaitQueuePtr,
                    CompressedFrameSetPtr_ufmf cmpFrameSetPtr,
                    QObject *parent=0
                    );

            void stop();

        private:

            bool ready_;
            bool stopped_;

            CompressedFrameQueuePtr_ufmf cmpFrameWaitQueuePtr_;
            CompressedFrameSetPtr_ufmf cmpFrameDoneSetPtr_;

            void initialize(
                    CompressedFrameQueuePtr_ufmf cmpFrameWaitQueuePtr,
                    CompressedFrameSetPtr_ufmf cmpFrameDoneSetPtr
                    );

            void run();

    };

} // namespace bias

#endif // #ifndef BIAS_COMPRESSOR_UFMF
