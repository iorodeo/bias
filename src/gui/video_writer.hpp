#ifndef BIAS_VIDEO_WRITER_HPP
#define BIAS_VIDEO_WRITER_HPP
#include "stamped_image.hpp"
#include <QString>
#include <QObject>
#include <opencv2/core/core.hpp>

namespace bias
{
    class VideoWriter : public QObject
    {
        Q_OBJECT 

        public:

            VideoWriter(QObject *parent=0); 
            explicit VideoWriter(QString fileName, unsigned int cameraNumber, QObject *parent=0);
            virtual ~VideoWriter();
            virtual void setFileName(QString fileName);
            virtual void setSize(cv::Size size);
            virtual void setFrameSkip(unsigned int frameSkip);
            virtual void addFrame(StampedImage stampedImg);
            virtual QString getFileName() const;
            virtual cv::Size getSize() const;
            virtual unsigned int getFrameSkip() const;
            virtual void finish();

        signals:
            void imageLoggingError(unsigned int errorId, QString errorMsg);

        protected:

            cv::Size size_;
            QString fileName_;
            unsigned long frameCount_;
            unsigned int frameSkip_;
            unsigned int cameraNumber_;
            QString getUniqueFileName();
    };

} // namespace bias

#endif // #ifndef BIAS_VIDEO_WRITER_HPP
