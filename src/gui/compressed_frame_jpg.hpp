#ifndef BIAS_COMPRESSED_FRAME_JPG_HPP
#define BIAS_COMPRESSED_FRAME_JPG_HPP
#include <opencv2/core/core.hpp>
#include <QString>
#include <memory>
#include <vector>
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
            CompressedFrame_jpg(QString fileName, StampedImage stampedImg, unsigned int quality, bool mjpgFlag);

            bool haveFileName() const;
            bool haveStampedImage() const;

            void setStampedImage(StampedImage stampedImg);
            StampedImage getStampedImage() const;
            unsigned long getFrameCount() const;
            double getTimeStamp() const;

            void setFileName(QString fileName);
            QString getFileName() const;

            void setQuality(unsigned int quality);
            unsigned int getQuality() const;

            void setMjpgFlag(bool value);
            bool getMjpgFlag() const;

            bool haveEncoding() const;
            std::vector<uchar> &getEncodedJpgBuffer();

            void write();
            void encode();

            static const QString DEFAULT_FILENAME;
            static const unsigned int DEFAULT_QUALITY;
            static const bool DEFAULT_MJPG_FLAG;

        protected:

            bool haveFileName_;
            bool haveStampedImg_;
            bool haveEncoding_;

            QString fileName_;
            unsigned int quality_;
            bool mjpgFlag_;

            StampedImage stampedImg_;;
            std::vector<uchar> encodedJpgBuffer_;

    };

    class CompressedFrameCmp_jpg 
        : public std::binary_function<CompressedFrame_jpg, CompressedFrame_jpg, bool>
    {
        // Comparison object for Compressed frames
        public:
            bool operator() (
                    const CompressedFrame_jpg &cmpFrame0,
                    const CompressedFrame_jpg &cmpFrame1
                    );
    };

    typedef LockableQueue<CompressedFrame_jpg> CompressedFrameQueue_jpg;
    typedef std::shared_ptr<CompressedFrameQueue_jpg> CompressedFrameQueuePtr_jpg;

    typedef LockableSet<CompressedFrame_jpg, CompressedFrameCmp_jpg> CompressedFrameSet_jpg;
    typedef std::shared_ptr<CompressedFrameSet_jpg> CompressedFrameSetPtr_jpg;

}

#endif
