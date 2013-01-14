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
            bool isCompressed_;

            StampImage stampedImg_;

            cv::Mat bgMembership_;

            unsigned int numForeground_;
            unsigned int numPixWritten_;

            std::shared_ptr<uint16_t> writeRowBuffer_;
            std::shared_ptr<uint16_t> writeColBuffer_;
            std::shared_ptr<uint16_t> writeHeightBuffer_;
            std::shared_ptr<uint16_t> writeWidthBuffer_;
            std::shared_ptr<uint16_t> numWrites_;
            std::shared_ptr<uint8_t> writeDataBuffer_;

            unsigned long numConectedComp_;
            unsigned int boxLength_;
            unsigned int boxArea_;

            double fgMaxFracCompress_;

    };

}

#endif // #ifndef BIAS_COMPRESSED_FRAME_UFMF
