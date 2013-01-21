#ifndef BIAS_COMPRESSED_FRAME_UFMF
#define BIAS_COMPRESSED_FRAME_UFMF

#include <memory>
#include <vector>
#include <opencv2/core/core.hpp>
#include "stamped_image.hpp"

namespace bias
{

    class CompressedFrame_ufmf
    {
        public:

            CompressedFrame_ufmf();
            CompressedFrame_ufmf(unsigned int boxLength, double fgMaxFracCompress);

            void setData(
                    StampedImage stampedImg, 
                    cv::Mat bgLowerBound, 
                    cv::Mat bgUpperBound
                    );

            static const uchar BACKGROUND_MEMBER_VALUE;
            static const uchar FOREGROUND_MEMBER_VALUE;
            static const unsigned int DEFAULT_BOX_LENGTH; 
            static const double DEFAULT_FG_MAX_FRAC_COMPRESS;

            // TEMPORARY REMOVE THIS ???
            // -------------------------------------------------------------
            cv::Mat getMembershipImage();
            // --------------------------------------------------------------

        private:

            bool haveData_;              
            bool isCompressed_;           // True if frame is compressed 

            StampedImage stampedImg_;     // Original image w/ framenumber and timestamp
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
            void allocateBuffers();          
            void resetBuffers();
                                      
    };

}

#endif // #ifndef BIAS_COMPRESSED_FRAME_UFMF
