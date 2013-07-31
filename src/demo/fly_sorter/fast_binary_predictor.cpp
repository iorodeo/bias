#include "fast_binary_predictor.hpp"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>


// FastBinaryPredictor
// --------------------------------------------------------------------------

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

    // Get fit values - what does fit standfor ... fitness? ask Kristin
    data.fit = cv::Mat(mat.size(), CV_32FC1, cv::Scalar(param_.offset));
    for (int i=0; i<mat.rows;i++)
    {
        for (int j=0; j<mat.cols; j++)
        {
            for (int k=0; k<param_.stumpVector.size(); k++)
            {
                StumpData stumpData = param_.stumpVector[k];
                cv::Vec3b elem = mat.at<cv::Vec3b>(i,j);
                uchar chanValue = elem[stumpData.channel];
                if (chanValue < stumpData.threshold)
                {
                    data.fit.at<float>(i,j) = data.fit.at<float>(i,j) - stumpData.value;
                }
            } 
        } 
    } 

    // Get labels - is there any reason not to use binary labels?
    data.label = cv::Mat(mat.size(), CV_8UC1, cv::Scalar(0)); 
    cv::threshold(data.fit,data.label,0.0,255,CV_THRESH_BINARY);
    return data;
}

