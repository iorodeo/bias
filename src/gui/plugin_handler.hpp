#ifndef BIAS_PLUGIN_HANDLER_HPP 
#define BIAS_PLUGIN_HANDLER_HPP
#include <memory>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <QPointer>
#include "lockable.hpp"
#include <opencv2/core/core.hpp>
#include "bias_plugin.hpp"


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

            PluginHandler(
                    unsigned int cameraNumber,
                    std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr, 
                    BiasPlugin *pluginPtr,
                    QObject *parent=0
                    );

            void initialize(
                    unsigned int cameraNumber,
                    std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr,
                    BiasPlugin *pluginPtr
                    );

            void stop();

            void setCameraNumber(unsigned int cameraNumber);
            void setImageQueue(std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr);
            void setPlugin(BiasPlugin *pluginPtr);
            cv::Mat getImage() const;

        signals:
            void pluginError(unsigned int errorId, QString errorMsg);

        private:
            bool ready_;
            bool stopped_;
            unsigned int cameraNumber_;
            QPointer<BiasPlugin> pluginPtr_;
            std::shared_ptr<LockableQueue<StampedImage>> pluginImageQueuePtr_;

            void run();
            void setReadyState();


    }; // class PluginHandler

} // namespace bias

#endif
