#ifndef BIAS_COMPRESSED_FRAME_UFMF
#define BIAS_COMPRESSED_FRAME_UFMF

#include <vector>
#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>
#include "stamped_image.hpp"
#include "lockable.hpp"

namespace bias
{
    class CompressedFrame_ufmf 
    {
        public:

            CompressedFrame_ufmf();

            CompressedFrame_ufmf(
                    unsigned int boxLength
                    );

            CompressedFrame_ufmf(
                    unsigned int boxLength, 
                    double fgMaxFracCompress
                    );

            void setData(
                    StampedImage stampedImg, 
                    cv::Mat bgLowerBound, 
                    cv::Mat bgUpperBound,
                    unsigned long bgUpdateCount
                    );

            void compress();

            bool haveData() const;
            bool isReady() const;
            double getTimeStamp() const;
            unsigned long getFrameCount() const;
            unsigned int getNumConnectedComp() const;

            void dilateEnabled(bool value);
            void setDilateWindowSize(unsigned int value);

            std::shared_ptr<std::vector<uint16_t>> getWriteRowBufPtr();
            std::shared_ptr<std::vector<uint16_t>> getWriteColBufPtr();
            std::shared_ptr<std::vector<uint16_t>> getWriteHgtBufPtr();
            std::shared_ptr<std::vector<uint16_t>> getWriteWdtBufPtr();
            std::shared_ptr<std::vector<uint8_t>> getImageDataPtr();

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

            cv::Mat bgLowerBound_;        // Background lower bound image values
            cv::Mat bgUpperBound_;        // Background upper bound image values
            cv::Mat membershipImage_;     // Background/foreground membership
            StampedImage stampedImg_;     // Original image w/ framenumber and timestamp

            unsigned int numPix_;
            unsigned int numForeground_;  // Number of forground pixels
            unsigned int numPixWritten_;  // Number of pixels written
            unsigned long bgUpdateCount_; // Update count of background model used 

            std::shared_ptr<std::vector<uint16_t>> writeRowBufPtr_;  // Y mins
            std::shared_ptr<std::vector<uint16_t>> writeColBufPtr_;  // X mins
            std::shared_ptr<std::vector<uint16_t>> writeHgtBufPtr_;  // Heights
            std::shared_ptr<std::vector<uint16_t>> writeWdtBufPtr_;  // Widths
            std::shared_ptr<std::vector<uint16_t>> numWriteBufPtr_;  // Number of times pixel written 
            std::shared_ptr<std::vector<uint8_t>>  imageDatBufPtr_;  // Image data 

            unsigned int boxArea_;           // BoxLength*boxLength
            unsigned int boxLength_;         // Length of boxes or foreground pixels to store
            unsigned int numConnectedComp_;  // Number of connected components
            double fgMaxFracCompress_;       // Maximum fraction of pixels that can be in foreground in order for us to compress

            bool dilateEnabled_;
            unsigned int dilateWindowSize_;


            void allocateBuffers();          
            void resetBuffers(); 
            void createUncompressedFrame();
            void createCompressedFrame();
                                      
    };


    class CompressedFrameCmp_ufmf 
        : public std::binary_function<CompressedFrame_ufmf, CompressedFrame_ufmf, bool>
    {
        // Comparison object for Compressed frames
        public:
            bool operator() (
                    const CompressedFrame_ufmf &cmpFrame0,
                    const CompressedFrame_ufmf &cmpFrame1
                    );
    };

    // Typedef for sets and queues of compressed frame objects
    typedef LockableQueue<CompressedFrame_ufmf> CompressedFrameQueue_ufmf;
    typedef std::shared_ptr<CompressedFrameQueue_ufmf> CompressedFrameQueuePtr_ufmf;

    typedef LockableSet<CompressedFrame_ufmf, CompressedFrameCmp_ufmf> CompressedFrameSet_ufmf;
    typedef std::shared_ptr<CompressedFrameSet_ufmf> CompressedFrameSetPtr_ufmf;

} // namespace bias

#endif // #ifndef BIAS_COMPRESSED_FRAME_UFMF
