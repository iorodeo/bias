#ifndef BGR_TO_LUV_CONVERTER_HPP
#define BGR_TO_LUV_CONVERTER_HPP
#include <opencv2/core/core.hpp>
#include <vector>


class BgrToLuvConverter
{
    // Custom BRG to Luv converter. Note, we are using this
    // instead of opencv's cvtColor because the LUV values
    // computed did not match those of Piotr Dollar's rgbConvert
    // convert function which is is being used for training.
    public:
        static cv::Mat convert(cv::Mat bgrImg);
        static std::vector<float> createLookupTable();
    private:
        static const int LOOKUP_TABLE_SIZE;
        static const std::vector<float> LOOKUP_TABLE;
        static const cv::Matx33f BGR_TO_LUV_MATRIX_8U; 
        static const cv::Matx33f BGR_TO_LUV_MATRIX_FLT;
};

#endif
