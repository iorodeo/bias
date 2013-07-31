#include "fast_binary_predictor.hpp"


FastBinaryPredictor::FastBinaryPredictor() {} 


FastBinaryPredictor::FastBinaryPredictor(ClassifierParam param)
{
    setClassifierParam(param);
}


void FastBinaryPredictor::setClassifierParam(ClassifierParam param)
{
    param_ = param;
}


FastBinaryPredictorData FastBinaryPredictor::predict(cv::Mat mat)
{
    FastBinaryPredictorData data;
    return data;
}

