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

        private:

            bool haveData_;              
            bool isCompressed_;          // true if frame is compressed 

            StampedImage stampedImg_;    // original image w/ framenumber and timestamp
            cv::Mat bgMembership_;       // background membership

            unsigned int numForeground_; // number of forground pixels
            unsigned int numPixWritten_; // number of pixels written

            std::shared_ptr<uint16_t> writeRowBuffer_;     // ymins
            std::shared_ptr<uint16_t> writeColBuffer_;     // xmins
            std::shared_ptr<uint16_t> writeHeightBuffer_;  // heights
            std::shared_ptr<uint16_t> writeWidthBuffer_;   // widths
            std::shared_ptr<uint16_t> numPixWriteBuffer_;  // # times pixel written 
            std::shared_ptr<uint8_t> imgDataBuffer_;       // image data 

            unsigned long numConectedComp_; // number of connected components
            unsigned int boxLength_;        // length of boxes or foreground pixels to store
            unsigned int boxArea_;          // boxLength*boxLength

            double fgMaxFracCompress_;      // maximum fraction of pixels that can be in foreground
                                            // in order for us to compress
                                            //
            void allocateBuffers(); 
                                      

    };

}

#endif // #ifndef BIAS_COMPRESSED_FRAME_UFMF
