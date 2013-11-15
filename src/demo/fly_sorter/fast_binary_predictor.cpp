#include "fast_binary_predictor.hpp"
#include <iostream>
#include <cassert>
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


FastBinaryPredictorData<cv::Mat> FastBinaryPredictor::predict(cv::Mat mat)
{
    assert((mat.type() == CV_32FC3) || (mat.type() == CV_64FC3));

    FastBinaryPredictorData<cv::Mat> data;

    // Get fit values - what does fit standfor ... fitness? ask Kristin
    data.fit = cv::Mat(mat.size(), CV_32FC1, cv::Scalar(-param_.offset));

    for (int i=0; i<mat.rows;i++)
    {
        for (int j=0; j<mat.cols; j++)
        {
            for (int k=0; k<param_.stumpVector.size(); k++)
            {
                StumpData stumpData = param_.stumpVector[k];

                float chanValue = 0.0;
                if (mat.type() == CV_64FC3)
                {
                    cv::Vec3d elem = mat.at<cv::Vec3d>(i,j);
                    chanValue = float(elem[stumpData.channel]);
                }
                else
                {
                    cv::Vec3f elem = mat.at<cv::Vec3f>(i,j);
                    chanValue = elem[stumpData.channel];
                }

                if (chanValue < stumpData.threshold)
                {
                    data.fit.at<float>(i,j) = data.fit.at<float>(i,j) - stumpData.value;
                }

            }  // for (int k
        } // for (int j 
    } // for (int i 

    // Get labels - is there any reason not to use binary labels?
    data.label = cv::Mat(data.fit.size(),CV_32FC1,cv::Scalar(0.0));
    cv::threshold(data.fit,data.label,0.0,1.0,CV_THRESH_BINARY);
    data.label = 255.0*data.label;
    data.label.convertTo(data.label,CV_8UC1);
    return data;
}

 
FastBinaryPredictorData<double> FastBinaryPredictor::predict(std::vector<double> vec)
{
    FastBinaryPredictorData<double> data;

    data.fit = -param_.offset;

    for (int k=0; k<param_.stumpVector.size(); k++)
    {
        StumpData stumpData = param_.stumpVector[k];
        if (vec[stumpData.channel] < stumpData.threshold)
        {
            data.fit -= stumpData.value;
        }
    }
    if (data.fit > 0.0)
    {
        data.label = 1.0;
    }
    return data;
}

