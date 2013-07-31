#ifndef FAST_BINARY_PREDICTOR_HPP
#define FAST_BINARY_PREDICTOR_HPP
#include "parameters.hpp"
#include <opencv2/core/core.hpp>

class FastBinaryPredictorData
{
    public:
        cv::Mat fit;
        cv::Mat label;
};


class FastBinaryPredictor
{
    public:
        FastBinaryPredictor();
        FastBinaryPredictor(ClassifierParam param);
        void setClassifierParam(ClassifierParam param);
        FastBinaryPredictorData predict(cv::Mat mat);

    private:
        ClassifierParam param_;
};

#endif // #ifndef FAST_BINARY_PREDICTOR_HPP
