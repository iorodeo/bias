#ifndef BIAS_PLUGIN_HANDLER_HPP 
#define BIAS_PLUGIN_HANDLER_HPP
#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <opencv2/core/core.hpp>
#include "lockable.hpp"

namespace bias
{
    struct StampedImage;

    class PluginHandler : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:
            static const unsigned int MAX_IMAGE_QUEUE_SIZE;

            PluginHandler(QObject *parent=0);

            PluginHandler(
                    unsigned int cameraNumber,
                    std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr, 
                    QObject *parent=0
                    );

            void initialize(
                    unsigned int cameraNumber,
                    std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr 
                    );

            void stop();

            cv::Mat getImage() const;

        signals:
            void pluginError(unsigned int errorId, QString errorMsg);

        private:
            bool ready_;
            bool stopped_;
            unsigned int cameraNumber_;
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr_;
            cv::Mat currentImage_;

            void run();

    }; // class PluginHandler

} // namespace bias

#endif
