#ifndef BIAS_IMAGE_LOGGER_HPP
#define BIAS_IMAGE_LOGGER_HPP

#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include "camera_fwd.hpp"
#include "lockable.hpp"

class QString;


namespace bias
{

    class VideoWriter;

    struct StampedImage;

    class ImageLogger : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            ImageLogger(QObject *parent=0);

            ImageLogger(
                    std::shared_ptr<VideoWriter> videoWriterPtr,
                    std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr, 
                    QObject *parent=0
                    );

            void initialize(
                    std::shared_ptr<VideoWriter> videoWriterPtr,
                    std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr 
                    );

            void stop();

        signals:
            void imageLoggingError(unsigned int errorId, QString errorMsg);

        private:
            bool ready_;
            bool stopped_;
            unsigned long frameCount_;

            std::shared_ptr<VideoWriter> videoWriterPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> logImageQueuePtr_;

            void run();
    };

} // namespace bias

#endif // #ifndef BIAS_IMAGE_LOGGER_HPP
