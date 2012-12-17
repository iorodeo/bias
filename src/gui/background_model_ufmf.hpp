#ifndef BIAS_BACKGROUND_MODEL_UFMF_HPP
#define BIAS_BACKGROUND_MODEL_UFMF_HPP

#include "stamped_image.hpp"
#include <opencv2/core/core.hpp>
#include <stdint.h>
#include <vector>
#include <memory>


namespace bias
{

    class BackgroundHistogram
    {
        public:
            BackgroundHistogram();
            BackgroundHistogram(cv::Size imageSize, unsigned int numberOfBins);
            BackgroundHistogram(
                    unsigned int numRows,
                    unsigned int numCols,
                    unsigned int numBins
                    );
            ~BackgroundHistogram();
            unsigned int getValue(
                    unsigned int row, 
                    unsigned int col, 
                    unsigned int bin
                    );
            void incrementData(
                    unsigned int row, 
                    unsigned int col, 
                    unsigned int bin
                    );
            //float getMedianValue(unsigned int row, unsigned int col);
            void getMedianValues();
            unsigned long getTotal(unsigned int row, unsigned int col);
            void setValuesToZero();


        private: 
            std::shared_ptr<unsigned int> dataPtr_;
            std::shared_ptr<unsigned long> totalPtr_;
            unsigned int numRows_;
            unsigned int numCols_;
            unsigned int numBins_;
            unsigned long valueCount_;

    };


    class BackgroundModel_ufmf
    {

        public:
            BackgroundModel_ufmf();
            ~BackgroundModel_ufmf();
            void addFrame(StampedImage stampedImg);

            static const unsigned int DEFAULT_BIN_COUNT_MAX;
            static const unsigned int DEFAULT_BIN_SIZE_CV_8U;
            static const unsigned int DEFAULT_BIN_SIZE_CV_16U;

        private:
            bool isFirst_;
            cv::Size imageSize_;
            int imageType_;
            int imageDepth_;
            unsigned int binSize_;
            unsigned int numberOfBins_;
            unsigned int frameCnt_;
            float halfBin;

            BackgroundHistogram histogram_;


            void initialize(StampedImage stampedImg);
    };


} // namespace bias


#endif // #ifndef BIAS_BACKGROUND_MODEL_UFMF_HPP
