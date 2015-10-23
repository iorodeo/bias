#ifndef BIAS_COMPRESSED_FRAME_JPG_HPP
#define BIAS_COMPRESSED_FRAME_JPG_HPP
#include <opencv2/core/core.hpp>
#include <QString>
#include <memory>
#include "stamped_image.hpp"
#include "lockable.hpp"



namespace bias
{

    class CompressedFrame_jpg 
    {

        public:
            
            CompressedFrame_jpg();
            CompressedFrame_jpg(QString fileName);
            CompressedFrame_jpg(QString fileName, StampedImage stampedImg);
            CompressedFrame_jpg(QString fileName, StampedImage stampedImg, unsigned int quality);

            bool haveFileName();
            bool haveStampedImage();
            void setStampedImage(StampedImage stampedImg);
            StampedImage getStampedImage();
            void setFileName(QString fileName);
            QString getFileName();
            void setQuality(unsigned int quality);
            unsigned int getQuality();
            void write();

            static const QString DEFAULT_FILENAME;
            static const unsigned int DEFAULT_QUALITY;

        protected:

            bool haveFileName_;
            bool haveStampedImg_;

            QString fileName_;
            unsigned int quality_;

            StampedImage stampedImg_;;

    };

    typedef LockableQueue<CompressedFrame_jpg> CompressedFrameQueue_jpg;
    typedef std::shared_ptr<CompressedFrameQueue_jpg> CompressedFrameQueuePtr_jpg;
}

#endif
