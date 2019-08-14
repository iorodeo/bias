#ifndef DATA_CONSUMER_HPP
#define DATA_CONSUMER_HPP

#include <QObject>
#include <QRunnable>
#include "lockable.hpp"
#include "frame_data.hpp"

namespace bias
{

    class DataConsumer : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:

            DataConsumer(QObject *parent=0);
            void stop();

            void enqueueFrameDataCamera0(FrameData frameData);
            void enqueueFrameDataCamera1(FrameData frameData);

        private:

            bool stopped_;

            FrameDataQueue frameDataQueueCamera0_;
            FrameDataQueue frameDataQueueCamera1_;

            void run();
    };


}
#endif
