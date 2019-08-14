#ifndef FRAME_DATA_HPP
#define FRAME_DATA_HPP

#include <cv.h>
#include <QQueue>
#include <QSharedPointer>


namespace bias
{
    class FrameData
    {

        public:

            FrameData() {};

            cv::Mat image;
            unsigned long count;
    };

    typedef QQueue<FrameData> FrameDataQueue;
    typedef QSharedPointer<FrameDataQueue> FrameDataQueuePtr;

}


#endif
