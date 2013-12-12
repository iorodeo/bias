#ifndef HOG_POSITION_FITTER
#define HOG_POSITION_FITTER
#include "parameters.hpp"
#include "fly_segmenter.hpp"
#include <list>
#include <vector>
#include <string>
#include <opencv2/core/core.hpp>

//Debug 
//--------------------------------------
#include <fstream>
//--------------------------------------

class PositionData
{
    public:

        bool success;
        bool isFly;
        bool isMultipleFlies;
        bool flipped;
        unsigned int bodyArea;
        double meanXRel;
        double meanYRel;
        double meanXAbs;
        double meanYAbs;
        double ellipseMajorAxis;
        double ellipseMinorAxis;
        double ellipseAngle;
        cv::Mat covarianceMatrix;
        cv::Mat rotBoundingImageLUV;
        std::vector<double> pixelFeatureVector;
        SegmentData segmentData;
        unsigned long frameCount;

        PositionData();
        std::string toStdString(unsigned int indent=0);
        void print(unsigned int indent=0);

        static const bool DEFAULT_IS_FLY;
        static const bool DEFAULT_IS_MULTIPLE_FLIES;
};
typedef std::list<PositionData> PositionDataList;


class HogPositionFitterData
{
    public:
        PositionDataList positionDataList;
        HogPositionFitterData();
};


class HogPositionFitter
{
    public:

        HogPositionFitter();
        HogPositionFitter(HogPositionFitterParam param);
        void setParam(HogPositionFitterParam param);
        void trainingDataWriteEnable(std::string fileNamePrefix);
        void trainingDataWriteDisable();

        HogPositionFitterData fit(
                FlySegmenterData flySegmenterData, 
                unsigned long frameCount, 
                cv::Mat img
                );

    private:
        bool showDebugWindow_;
        bool writeTrainingData_;
        std::string trainingFileNamePrefix_;
        HogPositionFitterParam param_;

        cv::Mat getFillMask(cv::Mat image);
        std::vector<double> getPixelFeatureVector(cv::Mat image);

        std::vector<double> getHistGradMag(
                cv::Mat normGradMag, 
                cv::Mat mask
                );

        std::vector<double> getHistGradOri(
                cv::Mat gradOri, 
                cv::Mat normGradMag, 
                cv::Mat mask
                );

        std::vector<double> getHistColor(
                cv::Mat subImage, 
                cv::Mat mask
                );

        void createTrainingData(
                unsigned long frameCount, 
                PositionData posData,
                cv::Mat img
                );

        void writePixelFeatureVector(
                std::string fileName, 
                std::vector<double> pixVector
                );

};


enum GradientMethod
{
    GRAD_METHOD_SOBEL,
    GRAD_METHOD_SCHARR,
};


class GradientData
{
    public:
        cv::Mat dx;              // derivative w.r.t. x
        cv::Mat dy;              // derivative w.r.t. y
        cv::Mat mag;             // gradient magnitude
        cv::Mat ori;             // gradient orientation
        cv::Mat normMag;         // normalized gradient magnitude
        cv::Mat normMagMax;      // maximum (over all channels) of normalized gradient magnitude
        cv::Mat oriOfNormMagMax; // orientation of maximum gradient
};


GradientData getGradientData(
        cv::Mat image, 
        unsigned int normRadius, 
        double normConst,
        GradientMethod method
        );

cv::Mat getTriangleFilter1D(unsigned int normRadius);
cv::Mat getTriangleFilter2D(unsigned int normRadius);
double modRange(double input, double lower, double upper);




#endif
