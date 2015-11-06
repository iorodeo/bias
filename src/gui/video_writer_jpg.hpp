#ifndef BIAS_VIDEO_WRITER_JPG_HPP
#define BIAS_VIDEO_WRITER_JPG_HPP
#include "video_writer.hpp"
#include "video_writer_params.hpp"
#include "compressed_frame_jpg.hpp"
#include "compressor_jpg.hpp"
#include <QPointer>
#include <QDir>
#include <QString>
#include <vector>
#include <list>

class QThreadPool;

namespace bias
{
    class VideoWriter_jpg : public VideoWriter
    {
        public:

            VideoWriter_jpg(QObject *parent=0);
            explicit VideoWriter_jpg(
                    VideoWriterParams_jpg params,
                    QString fileName, 
                    unsigned int cameraNumber,
                    QObject *parent=0
                    );

            virtual ~VideoWriter_jpg();
            virtual void setFileName(QString fileName);
            virtual unsigned int getNextVersionNumber();
            virtual void addFrame(StampedImage stampedImg);
            virtual void finish();

            static const QString IMAGE_FILE_BASE;
            static const QString IMAGE_FILE_EXT;
            static const unsigned int FRAMES_TODO_MAX_QUEUE_SIZE;
            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int DEFAULT_QUALITY;
            static const unsigned int MIN_QUALITY;
            static const unsigned int MAX_QUALITY;
            static const unsigned int DEFAULT_NUMBER_OF_COMPRESSORS;
            static const bool DEFAULT_MJPG_FLAG;
            static const VideoWriterParams_jpg DEFAULT_PARAMS;

        protected:

            bool isFirst_;
            unsigned int quality_;
            bool mjpgFlag_;
            QDir baseDir_;
            QString baseName_;
            QDir logDir_;
            unsigned int numberOfCompressors_;

            std::vector<QPointer<Compressor_jpg>> compressorPtrVec_;

            CompressedFrameQueuePtr_jpg framesToDoQueuePtr_;
            CompressedFrameSetPtr_jpg framesFinishedSetPtr_;

            QPointer<QThreadPool> threadPoolPtr_;

            void setupOutput();
            QString getUniqueDirName();
            QString getLogDirName(unsigned int verNum);
            QDir getLogDir(unsigned int verNum);

            void startCompressors();
            void stopCompressors();


    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_JPG_HPP
