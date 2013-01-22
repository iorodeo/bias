#ifndef BIAS_COMPRESSED_FRAME_UFMF
#define BIAS_COMPRESSED_FRAME_UFMF

#include <memory>
#include <QObject>
#include <QRunnable>
#include <vector>
#include <opencv2/core/core.hpp>
#include "lockable.hpp"
#include "stamped_image.hpp"

namespace bias
{

    class CompressedFrame_ufmf : public QObject, public QRunnable, public Lockable<Empty>
    {
        Q_OBJECT

        public:

            CompressedFrame_ufmf(QObject *parent=0);

            CompressedFrame_ufmf(
                    unsigned int boxLength, 
                    double fgMaxFracCompress,
                    QObject *parent=0
                    );

            void setData(
                    StampedImage stampedImg, 
                    cv::Mat bgLowerBound, 
                    cv::Mat bgUpperBound
                    );

            void compress();


            static const uchar BACKGROUND_MEMBER_VALUE;
            static const uchar FOREGROUND_MEMBER_VALUE;
            static const unsigned int DEFAULT_BOX_LENGTH; 
            static const double DEFAULT_FG_MAX_FRAC_COMPRESS;

            // TEMPORARY REMOVE THIS ???
            // -------------------------------------------------------------
            //cv::Mat getMembershipImage();
            // --------------------------------------------------------------

        private:

            bool haveData_;               // True if image data has been set 
            bool isCompressed_;           // True if frame is compressed 
            bool ready_;                  // True if compressed frame has been calculated

            StampedImage stampedImg_;     // Original image w/ framenumber and timestamp
            cv::Mat bgLowerBound_;        // Background lower bound image values
            cv::Mat bgUpperBound_;        // Background upper bound image values
            cv::Mat membershipImage_;     // Background/foreground membership

            unsigned int numPix_;
            unsigned int numForeground_;  // Number of forground pixels
            unsigned int numPixWritten_;  // Number of pixels written

            std::vector<uint16_t> writeRowBuf_;  // Y mins
            std::vector<uint16_t> writeColBuf_;  // X mins
            std::vector<uint16_t> writeHgtBuf_;  // Heights
            std::vector<uint16_t> writeWdtBuf_;  // Widths
            std::vector<uint16_t> numWriteBuf_;  // Number of times pixel written 
            std::vector<uint8_t>  imageDatBuf_;  // Image data 

            unsigned int boxArea_;           // BoxLength*boxLength
            unsigned int boxLength_;         // Length of boxes or foreground pixels to store
            unsigned long numConnectedComp_; // Number of connected components
            double fgMaxFracCompress_;       // Maximum fraction of pixels that can be in foreground
                                             // in order for us to compress
                                             
            void initialize( unsigned int boxLength, double fgMaxFraccompress);
            void allocateBuffers();          
            void resetBuffers(); 
            void createUncompressedFrame();
            void createCompressedFrame();
            void run();
                                      
    };

} // namespace bias

#endif // #ifndef BIAS_COMPRESSED_FRAME_UFMF
