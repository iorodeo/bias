#ifndef BIAS_COMPRESSOR_JPG_HPP
#define BIAS_COMPRESSOR_JPG_HPP

#include <QObject>
#include <QRunnable>
#include <memory>
#include "lockable.hpp"
#include "compressed_frame_jpg.hpp"

namespace bias
{
    class Compressor_jpg : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:

            Compressor_jpg(QObject *parent=0);

            Compressor_jpg(CompressedFrameQueuePtr_jpg framesToDoQueuePtr, unsigned int cameraNumber, QObject *parent=0);

            void stop();

        private:

            bool ready_;
            bool stopped_;
            unsigned int cameraNumber_;
            CompressedFrameQueuePtr_jpg framesToDoQueuePtr_;

            void initialize(CompressedFrameQueuePtr_jpg framesToDoQueuePtr, unsigned int cameraNumber);
            void run();
    };

}
#endif
