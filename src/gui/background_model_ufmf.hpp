#ifndef BIAS_BACKGROUND_MODEL_UFMF_HPP
#define BIAS_BACKGROUND_MODEL_UFMF_HPP

#include "stamped_image.hpp"
#include <valarray>
#include <opencv2/core/core.hpp>

namespace bias
{
    class BackgroundModel_ufmf
    {

        public:
            BackgroundModel_ufmf();
            void addFrame(StampedImage stampedImg);

            static const unsigned int DEFAULT_BIN_SIZE;

        private:
            bool isFirst_;
            cv::Size imageSize_;
            int imageDepth_;
            unsigned int binSize_;
            unsigned int numberOfBins_;
            float halfBin;

            std::valarray<std::valarray<unsigned int>> bins_;

            void initialize(StampedImage stampedImg);
    };

   
} // namespace bias


#endif // #ifndef BIAS_BACKGROUND_MODEL_UFMF_HPP
