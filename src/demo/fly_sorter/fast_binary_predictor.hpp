#ifndef FAST_BINARY_PREDICTOR_HPP
#define FAST_BINARY_PREDICTOR_HPP
#include "parameters.hpp"
#include <opencv2/core/core.hpp>


template <class T>
class FastBinaryPredictorData 
{
    public:
        T fit;
        T label;
};


class FastBinaryPredictor
{
    public:
        FastBinaryPredictor();
        FastBinaryPredictor(ClassifierParam param);
        void setClassifierParam(ClassifierParam param);
        FastBinaryPredictorData<cv::Mat> predict(cv::Mat mat);
        FastBinaryPredictorData<double> predict(std::vector<double> vec);

    private:
        ClassifierParam param_;
};

#endif // #ifndef FAST_BINARY_PREDICTOR_HPP
