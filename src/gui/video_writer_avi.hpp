#ifndef BIAS_VIDEO_WRITER_AVI_HPP
#define BIAS_VIDEO_WRITER_AVI_HPP

#include "video_writer.hpp"
#include "video_writer_params.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QPointer>
#include <QMutex>

namespace bias
{
    class VideoWriter_avi : public VideoWriter
    {
        public:

            VideoWriter_avi(QObject *parent=0);
            VideoWriter_avi(
                    VideoWriterParams_avi params,
                    QString fileName, 
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );
            virtual ~VideoWriter_avi();
            virtual void addFrame(StampedImage stampedImg);

            // Static variables 
            static const int DEFAULT_FOURCC;
            static const double DEFAULT_FPS;
            static const double MIN_ALLOWED_DT_ESTIMATE;
            static const unsigned int DEFAULT_FRAME_SKIP;
            static const VideoWriterParams_avi DEFAULT_PARAMS;

            // Static methods
            static QStringList getListOfAllowedCodecs();
            static QMap<unsigned int, QString> getFourccToStringMap();
            static QMap<QString, unsigned int> getStringToFourccMap();
            static bool isAllowedCodec(QString codecString);
            static QString fourccToString(unsigned int fourcc);
            static unsigned int stringToFourcc(QString fouccString);

        protected:

            int fourcc_;
            double fps_;
            bool isFirst_;
            cv::VideoWriter videoWriter_;
            void setupOutput(StampedImage stampedImage);
            static QMutex *videoWriterMutexPtr_;
            
    };

}

#endif // #ifndef BIAS_VIDEO_WRITER_AVI_HPP

