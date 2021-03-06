#ifndef BIAS_VIDEO_WRITER_JPG_HPP
#define BIAS_VIDEO_WRITER_JPG_HPP
#include "video_writer.hpp"
#include "video_writer_params.hpp"
#include "compressed_frame_jpg.hpp"
#include "compressor_jpg.hpp"
#include <QPointer>
#include <QDir>
#include <QString>
#include <QPointer>
#include <vector>
#include <list>
#include <string>
#include <fstream>

class QThreadPool;

namespace bias
{

    class VideoWriter_jpg : public VideoWriter
    {
        Q_OBJECT 

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
            static const QString MJPG_FILE_EXT; 
            static const QString MJPG_INDEX_EXT; 
            static const QString MJPG_FILE_NAME;
            static const QString MJPG_INDEX_NAME;
            static const std::string MJPG_BOUNDARY_MARKER;
            static const unsigned int FRAMES_TODO_MAX_QUEUE_SIZE;
            static const unsigned int FRAMES_FINISHED_MAX_SET_SIZE;
            static const unsigned int DEFAULT_FRAME_SKIP;
            static const unsigned int DEFAULT_QUALITY;
            static const unsigned int MIN_QUALITY;
            static const unsigned int MAX_QUALITY;
            static const unsigned int DEFAULT_NUMBER_OF_COMPRESSORS;
            static const bool DEFAULT_MJPG_FLAG;
            static const bool DEFAULT_MJPG_MAX_FRAME_PER_FILE_FLAG;
            static const unsigned long DEFAULT_MJPG_MAX_FRAME_PER_FILE;
            static const unsigned long MJPG_MINVAL_MAX_FRAME_PER_FILE;
            static const VideoWriterParams_jpg DEFAULT_PARAMS;


        protected:

            bool isFirst_;
            bool skipReported_;
            unsigned int quality_;
            bool mjpgFlag_;
            bool mjpgMaxFramePerFileFlag_;
            unsigned long mjpgMaxFramePerFile_;
            QDir baseDir_;
            QString baseName_;
            QDir logDir_;
            unsigned int numberOfCompressors_;
            unsigned long nextFrameToWrite_;

            std::ofstream movieFile_;
            std::ofstream indexFile_;

            unsigned int movieFileCount_;
            unsigned long movieFileFrameCount_;

            std::vector<QPointer<Compressor_jpg>> compressorPtrVec_;

            CompressedFrameQueuePtr_jpg framesToDoQueuePtr_;
            CompressedFrameSetPtr_jpg framesFinishedSetPtr_;
            std::shared_ptr<Lockable<std::list<unsigned long>>> framesSkippedIndexListPtr_;

            QPointer<QThreadPool> threadPoolPtr_;

            void setupOutput();
            QString getUniqueDirName();
            QString getLogDirName(unsigned int verNum);
            QDir getLogDir(unsigned int verNum);
            QString getMovieFileName();
            QString getIndexFileName();

            void startCompressors();
            void stopCompressors();
            unsigned int clearFinishedFrames();
            void writeCompressedMjpgFrame(CompressedFrame_jpg frame);


        private slots:
            void onCompressorError(unsigned int errorId, QString errorMsg);

    };
   
}

#endif // #ifndef BIAS_VIDEO_WRITER_JPG_HPP
