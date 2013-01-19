#ifndef BIAS_COMPRESSED_FRAME_UFMF
#define BIAS_COMPRESSED_FRAME_UFMF

#include <memory>
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

            static const unsigned int DEFAULT_BOX_LENGTH; 
            static const double DEFAULT_FG_MAX_FRAC_COMPRESS;

            // TEMPORARY REMOVE THIS ???
            // ----------------------------------------------------
            cv::Mat bgMembershipImage_;    // Background membership
            cv::Mat getMembershipImage();
            // ----------------------------------------------------

        private:

            bool haveData_;              
            bool isCompressed_;            // True if frame is compressed 


            StampedImage stampedImg_;      // Original image w/ framenumber and timestamp

            unsigned int numPix_;
            unsigned int numForeground_;   // Number of forground pixels
            unsigned int numPixWritten_;   // Number of pixels written

            std::shared_ptr<bool> isForeground_;             
            std::shared_ptr<uint16_t> writeRowBuffer_;     // Y mins
            std::shared_ptr<uint16_t> writeColBuffer_;     // X mins
            std::shared_ptr<uint16_t> writeHeightBuffer_;  // Heights
            std::shared_ptr<uint16_t> writeWidthBuffer_;   // Widths
            std::shared_ptr<uint16_t> numPixWriteBuffer_;  // Number of times pixel written 
            std::shared_ptr<uint8_t>  imageDataBuffer_;    // Image data 

            unsigned long numConectedComp_; // Number of connected components
            unsigned int boxLength_;        // Length of boxes or foreground pixels to store
            unsigned int boxArea_;          // BoxLength*boxLength

            double fgMaxFracCompress_;      // Maximum fraction of pixels that can be in foreground
                                            // in order for us to compress

            void allocateBuffers();          
            void resetBuffers();
                                      
    };

}

#endif // #ifndef BIAS_COMPRESSED_FRAME_UFMF
