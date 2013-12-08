#include "parameters.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include <iostream>

const double PixelScaleFactor = 255.0;

// StumpData
// ---------------------------------------------------------------------------
StumpData::StumpData()
{
    channel = 0;
    threshold = 0.0;
    value = 0.0;
}


StumpData::StumpData(unsigned int chan, float thresh, float val)
{
    channel = chan;
    threshold = thresh;
    value = val;
}

// ClassifierParam
// ----------------------------------------------------------------------------
ClassifierParam::ClassifierParam()
{
    offset = 0.0;
}

// FlySegmenterParam
// ----------------------------------------------------------------------------
// TEMPORARY 
ClassifierParam FlySegmenterParam::createDefaultClassifierParam()
{
    ClassifierParam classifierParam;
    classifierParam.offset = 1.0784;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(1, 0.31038210,  1.90413685));
    stumpVector.push_back(StumpData(0, 0.24879919, -1.66404788));
    stumpVector.push_back(StumpData(1, 0.32196228,  1.27919675));
    stumpVector.push_back(StumpData(0, 0.27552019, -1.20679509));
    stumpVector.push_back(StumpData(0, 0.21504118, -0.80305969));
    stumpVector.push_back(StumpData(0, 0.29528950, -1.04935294));
    stumpVector.push_back(StumpData(0, 0.24879919,  0.54471722));
    stumpVector.push_back(StumpData(1, 0.32824126,  0.71242407));
    stumpVector.push_back(StumpData(0, 0.28644574, -0.75598466));
    stumpVector.push_back(StumpData(1, 0.31038520, -0.32418233));
    stumpVector.push_back(StumpData(2, 0.52677906,  0.38645774));
    stumpVector.push_back(StumpData(0, 0.23060922, -0.39011362));
    stumpVector.push_back(StumpData(1, 0.31038520, -0.33751456));
    stumpVector.push_back(StumpData(0, 0.19429915, -0.91714671));
    stumpVector.push_back(StumpData(2, 0.47833817, -0.81114635));
    stumpVector.push_back(StumpData(1, 0.29249324,  0.48032049));
    stumpVector.push_back(StumpData(0, 0.24879919,  0.29919018));
    stumpVector.push_back(StumpData(2, 0.51842082,  0.28624030));
    stumpVector.push_back(StumpData(1, 0.32204734, -0.26596010));
    stumpVector.push_back(StumpData(0, 0.23514625, -0.27858646));
    classifierParam.stumpVector = stumpVector;
    return classifierParam;
};
const ClassifierParam FlySegmenterParam::DEFAULT_CLASSIFIER_PARAM = 
createDefaultClassifierParam();


FlySegmenterParam::FlySegmenterParam()
{
    classifier = DEFAULT_CLASSIFIER_PARAM;
};

// BinParam
// ----------------------------------------------------------------------------
BinParam::BinParam()
{
    numX = 0;
    numY = 0;
}

BinParam::BinParam(unsigned int numX_, unsigned int numY_)
{
    numX = numX_;
    numY = numY_;
}


// PixelFeatureVectorParam
// ----------------------------------------------------------------------------
const unsigned int PixelFeatureVectorParam::DEFAULT_GRAD_NORM_RADIUS = 5;
const double PixelFeatureVectorParam::DEFAULT_GRAD_NORM_CONST = 0.005;  
const unsigned int PixelFeatureVectorParam::DEFAULT_FILL_BNDRY_ERODE_RADIUS = 2;

// TEMPORARY  
std::vector<cv::Scalar> PixelFeatureVectorParam::createDefaultColorEdgeVector()
{
    std::vector<cv::Scalar> colorEdgeVector;
    colorEdgeVector.push_back(cv::Scalar(0.0000000, 0.0000000, 0.0000000));
    colorEdgeVector.push_back(cv::Scalar(0.2510943, 0.3135227, 0.4994996));
    colorEdgeVector.push_back(cv::Scalar(0.2709672, 0.3162611, 0.5043570));
    colorEdgeVector.push_back(cv::Scalar(0.2850360, 0.3181373, 0.5068931));
    colorEdgeVector.push_back(cv::Scalar(0.2927038, 0.3213659, 0.5119556));
    colorEdgeVector.push_back(cv::Scalar(0.3014454, 0.3250251, 0.5217634));
    colorEdgeVector.push_back(cv::Scalar(0.3122305, 0.3280643, 0.5441338));
    colorEdgeVector.push_back(cv::Scalar(0.3233073, 0.3335537, 0.5617753));
    colorEdgeVector.push_back(cv::Scalar(0.3317617, 0.3394488, 0.5740660));
    colorEdgeVector.push_back(cv::Scalar(0.3351096, 0.3517826, 0.5846382));
    colorEdgeVector.push_back(cv::Scalar(2.0000000, 2.0000000, 2.0000000));
    return colorEdgeVector;
}
const std::vector<cv::Scalar> PixelFeatureVectorParam::DEFAULT_COLOR_EDGE_VECTOR = 
createDefaultColorEdgeVector();

// TEMPORARY  
std::vector<float> PixelFeatureVectorParam::createDefaultGradMagEdgeVector()
{
    std::vector<float> gradMagEdgeVector;
    gradMagEdgeVector.push_back(0.0 );
    gradMagEdgeVector.push_back(0.0999999493360519);
    gradMagEdgeVector.push_back(0.447888605296612);
    gradMagEdgeVector.push_back(0.717480659484863);
    gradMagEdgeVector.push_back(1.08998268842697);
    gradMagEdgeVector.push_back(100.0);
    return gradMagEdgeVector;
}
const std::vector<float> PixelFeatureVectorParam::DEFAULT_GRAD_MAG_EDGE_VECTOR = 
createDefaultGradMagEdgeVector();

// TEMPORARY 
std::vector<float> PixelFeatureVectorParam::createDefaultGradOriCentVector()
{
    std::vector<float> gradOriCentVector;
    gradOriCentVector.push_back(0.523598775598299);
    gradOriCentVector.push_back(1.5707963267949);
    gradOriCentVector.push_back(2.61799387799149);
    return gradOriCentVector;
}
const std::vector<float> PixelFeatureVectorParam::DEFAULT_GRAD_ORI_CENT_VECTOR = 
createDefaultGradOriCentVector();

// TEMPORARY 
std::vector<BinParam> PixelFeatureVectorParam::createDefaultBinParam()
{
    std::vector<BinParam> binParam;
    binParam.push_back(BinParam(1,5));
    binParam.push_back(BinParam(3,5));
    binParam.push_back(BinParam(3,1));
    return binParam;
}
const std::vector<BinParam> PixelFeatureVectorParam::DEFAULT_BIN_PARAM = 
createDefaultBinParam();

PixelFeatureVectorParam::PixelFeatureVectorParam()
{
    gradNormRadius = DEFAULT_GRAD_NORM_RADIUS;
    gradNormConst = DEFAULT_GRAD_NORM_CONST;
    fillBndryErodeRadius = DEFAULT_FILL_BNDRY_ERODE_RADIUS;
    colorEdgeVector = DEFAULT_COLOR_EDGE_VECTOR;
    gradMagEdgeVector = DEFAULT_GRAD_MAG_EDGE_VECTOR;
    gradOriCentVector = DEFAULT_GRAD_ORI_CENT_VECTOR;
    binParam = DEFAULT_BIN_PARAM;
};

// HogPositionFitterParam
// ----------------------------------------------------------------------------

const unsigned int HogPositionFitterParam::DEFAULT_CLOSE_RADIUS = 15;
const unsigned int HogPositionFitterParam::DEFAULT_OPEN_AREA = 3400;
const unsigned int HogPositionFitterParam::DEFAULT_MAX_BODY_AREA = 16600;
const cv::Scalar HogPositionFitterParam::DEFAULT_FILL_VALUES_LUV =
cv::Scalar(0.3703704,0.3259259,0.4962940); 
const double HogPositionFitterParam::DEFAULT_PAD_BORDER = 15.0;

// TEMPORARY 
ClassifierParam HogPositionFitterParam::createDefaultOrientClassifierParam()
{
    ClassifierParam classifierParam;
    classifierParam.offset = -16.7966;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(112, 0.002833, 1.983723));
    stumpVector.push_back(StumpData(232, 0.002833, -1.984202));
    stumpVector.push_back(StumpData(115, 0.008012, -1.959708));
    stumpVector.push_back(StumpData(235, 0.008211, 1.950887));
    stumpVector.push_back(StumpData(37, 0.306614, -1.851272));
    stumpVector.push_back(StumpData(25, 0.306401, 1.875527));
    stumpVector.push_back(StumpData(236, 0.016120, 1.680340));
    stumpVector.push_back(StumpData(202, 0.000407, -1.743562));
    stumpVector.push_back(StumpData(146, 0.027317, -1.706087));
    stumpVector.push_back(StumpData(142, 0.000407, 1.763328));
    stumpVector.push_back(StumpData(116, 0.015109, -1.735205));
    stumpVector.push_back(StumpData(206, 0.027317, 1.629330));
    stumpVector.push_back(StumpData(115, 0.011560, -1.437190));
    stumpVector.push_back(StumpData(236, 0.015109, 1.630453));
    stumpVector.push_back(StumpData(201, 0.011108, -1.603780));
    stumpVector.push_back(StumpData(141, 0.011108, 1.616170));
    stumpVector.push_back(StumpData(235, 0.011560, 1.495133));
    stumpVector.push_back(StumpData(116, 0.016120, -1.601133));
    stumpVector.push_back(StumpData(899, 0.275501, 1.520732));
    stumpVector.push_back(StumpData(911, 0.278920, -1.632418));
    stumpVector.push_back(StumpData(232, 0.003976, -1.556266));
    stumpVector.push_back(StumpData(202, 0.001525, -1.577791));
    stumpVector.push_back(StumpData(112, 0.003976, 1.648567));
    stumpVector.push_back(StumpData(142, 0.001525, 1.586624));
    stumpVector.push_back(StumpData(204, 0.019942, 1.588415));
    stumpVector.push_back(StumpData(416, 0.031681, -1.499144));
    stumpVector.push_back(StumpData(236, 0.012951, 1.621361));
    stumpVector.push_back(StumpData(111, 0.006901, 1.644504));
    stumpVector.push_back(StumpData(34, 0.311323, -1.517389));
    stumpVector.push_back(StumpData(235, 0.011560, 1.553500));
    stumpVector.push_back(StumpData(202, 0.002242, -1.598525));
    stumpVector.push_back(StumpData(141, 0.012292, 1.522593));
    stumpVector.push_back(StumpData(116, 0.012951, -1.594934));
    stumpVector.push_back(StumpData(232, 0.002833, -1.586161));
    stumpVector.push_back(StumpData(536, 0.031681, 1.591355));
    stumpVector.push_back(StumpData(144, 0.019891, -1.513393));
    stumpVector.push_back(StumpData(201, 0.003731, -1.542618));
    stumpVector.push_back(StumpData(28, 0.313313, 1.369082));
    stumpVector.push_back(StumpData(116, 0.016711, -1.449301));
    stumpVector.push_back(StumpData(142, 0.000071, 1.430221));
    stumpVector.push_back(StumpData(231, 0.007153, -1.574709));
    stumpVector.push_back(StumpData(144, 0.019942, -1.558897));
    stumpVector.push_back(StumpData(536, 0.031681, 1.438624));
    stumpVector.push_back(StumpData(115, 0.011560, -1.582286));
    stumpVector.push_back(StumpData(64, 0.329356, -1.562409));
    stumpVector.push_back(StumpData(384, 0.009421, 1.402641));
    stumpVector.push_back(StumpData(116, 0.013351, -1.419670));
    stumpVector.push_back(StumpData(28, 0.313648, 1.458448));
    stumpVector.push_back(StumpData(124, 0.064568, -1.353391));
    stumpVector.push_back(StumpData(232, 0.003976, -1.546833));
    stumpVector.push_back(StumpData(202, 0.000071, -1.448950));
    stumpVector.push_back(StumpData(112, 0.002833, 1.625674));
    stumpVector.push_back(StumpData(236, 0.016711, 1.558370));
    stumpVector.push_back(StumpData(899, 0.278920, 1.478915));
    stumpVector.push_back(StumpData(224, 0.010015, 1.430835));
    stumpVector.push_back(StumpData(111, 0.008508, 1.570713));
    stumpVector.push_back(StumpData(187, 0.267372, -1.617410));
    stumpVector.push_back(StumpData(807, 0.069382, 1.576354));
    stumpVector.push_back(StumpData(416, 0.032240, -1.557547));
    stumpVector.push_back(StumpData(142, 0.000156, 1.445432));
    stumpVector.push_back(StumpData(25, 0.306548, 1.598108));
    stumpVector.push_back(StumpData(236, 0.013351, 1.516873));
    stumpVector.push_back(StumpData(34, 0.313382, -1.527913));
    stumpVector.push_back(StumpData(115, 0.011560, -1.454917));
    stumpVector.push_back(StumpData(142, 0.001052, 1.581576));
    stumpVector.push_back(StumpData(144, 0.019942, -1.538615));
    stumpVector.push_back(StumpData(37, 0.306727, -1.625760));
    stumpVector.push_back(StumpData(116, 0.016711, -1.523061));
    stumpVector.push_back(StumpData(235, 0.011560, 1.427218));
    stumpVector.push_back(StumpData(202, 0.000914, -1.565892));
    stumpVector.push_back(StumpData(204, 0.019891, 1.565615));
    stumpVector.push_back(StumpData(141, 0.012014, 1.623432));
    stumpVector.push_back(StumpData(236, 0.016711, 1.335551));
    stumpVector.push_back(StumpData(201, 0.011121, -1.536721));
    stumpVector.push_back(StumpData(127, 0.228118, 1.583106));
    stumpVector.push_back(StumpData(911, 0.278920, -1.405823));
    stumpVector.push_back(StumpData(232, 0.002833, -1.540978));
    stumpVector.push_back(StumpData(116, 0.012951, -1.595306));
    stumpVector.push_back(StumpData(536, 0.031681, 1.536796));
    stumpVector.push_back(StumpData(787, 0.068775, -1.476896));
    stumpVector.push_back(StumpData(187, 0.272392, -1.488366));
    stumpVector.push_back(StumpData(112, 0.002833, 1.546401));
    stumpVector.push_back(StumpData(142, 0.000914, 1.513372));
    stumpVector.push_back(StumpData(269, 0.112592, 0.809542));
    stumpVector.push_back(StumpData(269, 0.112592, 0.997646));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    stumpVector.push_back(StumpData(269, 0.112592, 1.000000));
    classifierParam.stumpVector = stumpVector;
    return classifierParam;
};
const ClassifierParam HogPositionFitterParam::DEFAULT_ORIENT_CLASSIFIER_PARAM = 
createDefaultOrientClassifierParam();

HogPositionFitterParam::HogPositionFitterParam()
{
    closeRadius = DEFAULT_CLOSE_RADIUS;
    openArea = DEFAULT_OPEN_AREA;
    maxBodyArea = DEFAULT_MAX_BODY_AREA;
    fillValuesLUV = DEFAULT_FILL_VALUES_LUV;
    padBorder = DEFAULT_PAD_BORDER;
    orientClassifier = DEFAULT_ORIENT_CLASSIFIER_PARAM;
}

// GenderSorterParam
// ----------------------------------------------------------------------------

ClassifierParam GenderSorterParam::createDefaultGenderClassifierParam()
{
    ClassifierParam classifierParam;
    classifierParam.offset = -1.604457;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(238, 0.084434, 1.674599));
    stumpVector.push_back(StumpData(24,  0.278435, -1.232384));
    stumpVector.push_back(StumpData(238, 0.062391, 1.288068));
    stumpVector.push_back(StumpData(750, 0.001465, -1.190372));
    stumpVector.push_back(StumpData(238, 0.116674, 1.324515));
    stumpVector.push_back(StumpData(394, 0.029850, -1.000583));
    stumpVector.push_back(StumpData(93,  0.239109, 0.899523));
    stumpVector.push_back(StumpData(744, 0.018012, -0.885773));
    stumpVector.push_back(StumpData(229, 0.026950, -0.870549));
    stumpVector.push_back(StumpData(239, 0.001517, 0.764537));
    stumpVector.push_back(StumpData(749, 0.013050, -1.114222));
    stumpVector.push_back(StumpData(238, 0.028924, 1.011049));
    stumpVector.push_back(StumpData(749, 0.126848, -0.915492));
    stumpVector.push_back(StumpData(238, 0.121497, 1.169906));
    stumpVector.push_back(StumpData(833, 0.019828, -0.811894));
    stumpVector.push_back(StumpData(194, 0.053240, 0.994033));
    stumpVector.push_back(StumpData(185, 0.049640, -0.732560));
    stumpVector.push_back(StumpData(224, 0.017239, 0.853175));
    stumpVector.push_back(StumpData(530, 0.006605, 0.644783));
    stumpVector.push_back(StumpData(849, 0.305672, -0.818103));
    stumpVector.push_back(StumpData(164, 0.039244, -0.808899));
    stumpVector.push_back(StumpData(62,  0.527040, -1.090032));
    stumpVector.push_back(StumpData(538, 0.037647, 0.878776));
    stumpVector.push_back(StumpData(744, 0.012403, -1.020699));
    stumpVector.push_back(StumpData(938, 0.422793, 0.910620));
    stumpVector.push_back(StumpData(748, 0.555650, 0.899145));
    stumpVector.push_back(StumpData(929, 0.436754, -0.734846));
    stumpVector.push_back(StumpData(417, 0.077721, 0.843679));
    stumpVector.push_back(StumpData(510, 0.000547, -0.730515));
    stumpVector.push_back(StumpData(239, 0.006395, 0.893858));
    stumpVector.push_back(StumpData(506, 0.073350, 0.755999));
    stumpVector.push_back(StumpData(184, 0.067602, -0.797213));
    stumpVector.push_back(StumpData(64,  0.324437, -0.746603));
    stumpVector.push_back(StumpData(68,  0.499986, 0.692089));
    stumpVector.push_back(StumpData(126, 0.070462, 0.817883));
    stumpVector.push_back(StumpData(748, 0.487694, 0.686983));
    stumpVector.push_back(StumpData(62,  0.527040, -1.055904));
    stumpVector.push_back(StumpData(744, 0.019774, -0.790157));
    stumpVector.push_back(StumpData(113, 0.729929, -0.892345));
    stumpVector.push_back(StumpData(238, 0.038652, 0.754194));
    stumpVector.push_back(StumpData(185, 0.073443, -0.752404));
    stumpVector.push_back(StumpData(65,  0.519061, -0.950757));
    stumpVector.push_back(StumpData(238, 0.116044, 1.012158));
    stumpVector.push_back(StumpData(394, 0.022576, -0.895473));
    stumpVector.push_back(StumpData(54,  0.241851, -0.762248));
    stumpVector.push_back(StumpData(935, 0.380384, 0.768588));
    stumpVector.push_back(StumpData(510, 0.006829, -1.021979));
    stumpVector.push_back(StumpData(238, 0.062840, 0.702809));
    stumpVector.push_back(StumpData(237, 0.166578, -0.920166));
    stumpVector.push_back(StumpData(150, 0.000204, -0.620677));
    stumpVector.push_back(StumpData(792, 0.081822, 0.686082));
    stumpVector.push_back(StumpData(393, 0.490157, 0.879772));
    stumpVector.push_back(StumpData(501, 0.064878, -0.910076));
    stumpVector.push_back(StumpData(419, 0.022526, -0.973311));
    stumpVector.push_back(StumpData(239, 0.013651, 1.028860));
    stumpVector.push_back(StumpData(209, 0.004232, -0.901006));
    stumpVector.push_back(StumpData(238, 0.028924, 0.843551));
    stumpVector.push_back(StumpData(210, 0.000176, -0.659727));
    stumpVector.push_back(StumpData(106, 0.007611, 0.642773));
    stumpVector.push_back(StumpData(749, 0.143113, -0.833789));
    stumpVector.push_back(StumpData(239, 0.001517, 0.678634));
    stumpVector.push_back(StumpData(506, 0.079232, 0.710684));
    stumpVector.push_back(StumpData(938, 0.417708, 0.823056));
    stumpVector.push_back(StumpData(496, 0.131829, 0.790748));
    stumpVector.push_back(StumpData(197, 0.054917, -0.722815));
    stumpVector.push_back(StumpData(238, 0.068751, 0.643655));
    stumpVector.push_back(StumpData(113, 0.747618, -0.783980));
    stumpVector.push_back(StumpData(480, 0.000589, -0.763001));
    stumpVector.push_back(StumpData(127, 0.176899, 0.986837));
    stumpVector.push_back(StumpData(227, 0.022686, -0.662733));
    stumpVector.push_back(StumpData(538, 0.109119, 0.668535));
    stumpVector.push_back(StumpData(848, 0.000522, -0.642701));
    stumpVector.push_back(StumpData(62,  0.526630, -1.079848));
    stumpVector.push_back(StumpData(184, 0.067184, -0.737352));
    stumpVector.push_back(StumpData(94,  0.019178, -0.649458));
    stumpVector.push_back(StumpData(744, 0.013311, -0.873964));
    stumpVector.push_back(StumpData(154, 0.039989, -1.056767));
    stumpVector.push_back(StumpData(834, 0.500646, -0.885769));
    stumpVector.push_back(StumpData(394, 0.022576, -0.836019));
    stumpVector.push_back(StumpData(538, 0.046264, 0.709547));
    stumpVector.push_back(StumpData(240, 0.000357, 0.811416));
    stumpVector.push_back(StumpData(210, 0.004297, -1.040518));
    stumpVector.push_back(StumpData(229, 0.025967, -0.631054));
    stumpVector.push_back(StumpData(238, 0.121497, 1.097822));
    stumpVector.push_back(StumpData(54,  0.241201, -0.725406));
    stumpVector.push_back(StumpData(714, 0.028121, -0.593168));
    stumpVector.push_back(StumpData(774, 0.028121, -0.694201));
    stumpVector.push_back(StumpData(748, 0.553173, 0.830704));
    stumpVector.push_back(StumpData(183, 0.613997, 0.616241));
    stumpVector.push_back(StumpData(225, 0.004288, 1.025345));
    stumpVector.push_back(StumpData(164, 0.033674, -0.662720));
    stumpVector.push_back(StumpData(201, 0.023513, -1.010363));
    stumpVector.push_back(StumpData(723, 0.833923, 1.117179));
    stumpVector.push_back(StumpData(239, 0.005765, 0.717575));
    stumpVector.push_back(StumpData(493, 0.089478, 0.631682));
    stumpVector.push_back(StumpData(394, 0.030234, -0.616291));
    stumpVector.push_back(StumpData(185, 0.048949, -0.608983));
    stumpVector.push_back(StumpData(929, 0.450759, -0.739179));
    stumpVector.push_back(StumpData(238, 0.037773, 0.678025));
    stumpVector.push_back(StumpData(62,  0.527091, -1.006367));
    classifierParam.stumpVector = stumpVector;
    return classifierParam;
}
const ClassifierParam GenderSorterParam::DEFAULT_GENDER_CLASSIFIER_PARAM = 
createDefaultGenderClassifierParam();

const double GenderSorterParam::DEFAULT_MIN_CONFIDENCE = 3.9405; 

GenderSorterParam::GenderSorterParam()
{
    minConfidence = DEFAULT_MIN_CONFIDENCE;
    genderClassifier = DEFAULT_GENDER_CLASSIFIER_PARAM;
}

// ImageGrabberParam
// ----------------------------------------------------------------------------

// Notes:  really should get minimum and maximum values from camera.

const float ImageGrabberParam::DEFAULT_FRAMERATE = 10.0;
const float ImageGrabberParam::MINIMUM_FRAMERATE = 0.5;
const float ImageGrabberParam::MAXIMUM_FRAMERATE = 50.0;

const float ImageGrabberParam::DEFAULT_GAIN = 10.0;
const float ImageGrabberParam::MINIMUM_GAIN = 0.0;
const float ImageGrabberParam::MAXIMUM_GAIN = 24.0;

const float ImageGrabberParam::DEFAULT_SHUTTER = 20.0;
const float ImageGrabberParam::MINIMUM_SHUTTER = 0.01;
const float ImageGrabberParam::MAXIMUM_SHUTTER = 99.0;

const unsigned int ImageGrabberParam::DEFAULT_BRIGHTNESS = 16;
const unsigned int ImageGrabberParam::MAXIMUM_BRIGHTNESS = 255;

const float ImageGrabberParam::DEFAULT_GAMMA = 1.0;
const float ImageGrabberParam::MINIMUM_GAMMA = 0.5;
const float ImageGrabberParam::MAXIMUM_GAMMA = 4.0;

const float ImageGrabberParam::DEFAULT_SATURATION = 100.0;
const float ImageGrabberParam::MINIMUM_SATURATION = 0.0;
const float ImageGrabberParam::MAXIMUM_SATURATION = 399.0;

const unsigned int ImageGrabberParam::DEFAULT_WHITE_BALANCE_RED = 550;
const unsigned int ImageGrabberParam::DEFAULT_WHITE_BALANCE_BLUE = 500;
const unsigned int ImageGrabberParam::MINIMUM_WHITE_BALANCE = 0;
const unsigned int ImageGrabberParam::MAXIMUM_WHITE_BALANCE = 1023;

QStringList getAllowedCaptureModes()
{
    QStringList modes;
    modes << QString("camera"); // first is default value
    modes << QString("file");
    return modes;
}
const QStringList ImageGrabberParam::ALLOWED_CAPTURE_MODES = getAllowedCaptureModes();
const QString ImageGrabberParam::DEFAULT_CAPTURE_INPUT_FILE = QString("test.avi");

ImageGrabberParam::ImageGrabberParam()
{
    frameRate = DEFAULT_FRAMERATE;
    gain = DEFAULT_GAIN;
    shutter = DEFAULT_SHUTTER;
    brightness = DEFAULT_BRIGHTNESS;
    gamma = DEFAULT_GAMMA;
    saturation = DEFAULT_SATURATION;
    whiteBalanceRed = DEFAULT_WHITE_BALANCE_RED;
    whiteBalanceBlue = DEFAULT_WHITE_BALANCE_BLUE;
    captureMode = ALLOWED_CAPTURE_MODES.front();
    captureInputFile = DEFAULT_CAPTURE_INPUT_FILE;
}

QVariantMap ImageGrabberParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("frameRate", frameRate);
    paramMap.insert("gain", gain);
    paramMap.insert("shutter", shutter);
    paramMap.insert("brightness", brightness);
    paramMap.insert("gamma", gamma);
    paramMap.insert("saturation", saturation);
    paramMap.insert("whiteBalanceRed", whiteBalanceRed);
    paramMap.insert("whiteBalanceBlue", whiteBalanceBlue);
    paramMap.insert("captureMode", captureMode);
    paramMap.insert("captureInputFile", captureInputFile);
    return paramMap;
}



RtnStatus ImageGrabberParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;
    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter map is empty");
        return rtnStatus;
    }

    // Get frameRate
    // --------------
    if (!paramMap.contains("frameRate"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'frameRate' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["frameRate"].canConvert<float>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'frameRate' to float");
        return rtnStatus;
    }
    float frameRateTemp = paramMap["frameRate"].toFloat();
    if (frameRateTemp < MINIMUM_FRAMERATE) 
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'frameRate' less than minimum");
        return rtnStatus;
    }
    if (frameRateTemp > MAXIMUM_FRAMERATE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'frameRate' greater than maximum");
        return rtnStatus;
    }
    frameRate = frameRateTemp;

    // Get gain
    // --------------
    if (!paramMap.contains("gain"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'gain' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["gain"].canConvert<float>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'gain' to float");
        return rtnStatus;

    }
    float gainTemp = paramMap["gain"].toFloat();
    if (gainTemp < MINIMUM_GAIN)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'gain' less than minimum");
        return rtnStatus;

    }
    if (gainTemp > MAXIMUM_GAIN)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'gain' greater than maximum");
        return rtnStatus;

    }
    gain = gainTemp;
   

    // Get shutter
    // --------------
    if (!paramMap.contains("shutter"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'shutter' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["shutter"].canConvert<float>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'shutter' to float");
        return rtnStatus;

    }
    float shutterTemp = paramMap["shutter"].toFloat();
    if (shutterTemp < MINIMUM_SHUTTER)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'shutter' less than minimum");
        return rtnStatus;

    }
    if (shutterTemp > MAXIMUM_SHUTTER)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'shutter' greater than maximum");
        return rtnStatus;

    }
    shutter = shutterTemp;

    // Get brightness
    // ---------------
    if (!paramMap.contains("brightness"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'brightness' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["brightness"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'brightness' to unsigned int");
        return rtnStatus;

    }
    float brightnessTemp = paramMap["shutter"].toUInt();
    if (brightnessTemp > MAXIMUM_BRIGHTNESS)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'brightness' greater than maximum");
        return rtnStatus;
    }
    brightness = brightnessTemp; 

    // Get gamma 
    // -----------------
    if (!paramMap.contains("gamma"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'gamma' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["gamma"].canConvert<float>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'gamma' to float");
        return rtnStatus;
    }
    float gammaTemp = paramMap["gamma"].toFloat();
    if (gammaTemp < MINIMUM_GAMMA)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'gamma' less than minimum");
        return rtnStatus;
    }
    if (gammaTemp > MAXIMUM_GAMMA)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'gamma' greater than maximum");
        return rtnStatus;
    }

    // Get saturation 
    if (!paramMap.contains("saturation"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'saturation' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["saturation"].canConvert<float>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'saturation' to float");
        return rtnStatus;
    }
    float saturationTemp = paramMap["saturation"].toFloat();
    if (saturationTemp < MINIMUM_SATURATION)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'saturation' less than minimum");
        return rtnStatus;
    }
    if (saturationTemp > MAXIMUM_SATURATION)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'saturation' greater than maximum");
        return rtnStatus;
    }
    saturation = saturationTemp;

    
    // Get whiteBalanceRed 
    if (!paramMap.contains("whiteBalanceRed"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'whiteBalanceRed' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["whiteBalanceRed"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'whiteBalanceRed' to float");
        return rtnStatus;
    }
    unsigned int whiteBalanceRedTemp = paramMap["whiteBalanceRed"].toUInt();
    if (whiteBalanceRedTemp < MINIMUM_WHITE_BALANCE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'whiteBalanceRed' less than minimum");
        return rtnStatus;
    }
    if (whiteBalanceRedTemp > MAXIMUM_WHITE_BALANCE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'whiteBalanceRed' greater than maximum");
        return rtnStatus;
    }
    whiteBalanceRed = whiteBalanceRedTemp;
    
    // Get whiteBalanceBlue (unsigned int)
    if (!paramMap.contains("whiteBalanceBlue"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'whiteBalanceBlue' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["whiteBalanceBlue"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'whiteBalanceBlue' to float");
        return rtnStatus;
    }
    unsigned int whiteBalanceBlueTemp = paramMap["whiteBalanceBlue"].toUInt();
    if (whiteBalanceBlueTemp < MINIMUM_WHITE_BALANCE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'whiteBalanceBlue' less than minimum");
        return rtnStatus;
    }
    if (whiteBalanceBlueTemp > MAXIMUM_WHITE_BALANCE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'whiteBalanceBlue' greater than maximum");
        return rtnStatus;
    }
    whiteBalanceBlue = whiteBalanceBlueTemp;

    // Get capture mode
    // -----------------
    if (!paramMap.contains("captureMode"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'captureMode' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["captureMode"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'captureMode' to string");
        return rtnStatus;
    }
    QString captureModeTemp = paramMap["captureMode"].toString().toLower();
    if (!ALLOWED_CAPTURE_MODES.contains(captureModeTemp))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Image grabber parameter 'captureMode' not found in list of allowed values");
        return rtnStatus;
    }
    captureMode = captureModeTemp;

    // Get capture file
    // -----------------
    if (!paramMap.contains("captureInputFile"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'captureInputFile' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["captureInputFile"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'captureInputFile' to string");
        return rtnStatus;
    }
    captureInputFile = paramMap["captureInputFile"].toString();


    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}

// ServerParam
// ----------------------------------------------------------------------------

const unsigned int ServerParam::DEFAULT_PORT = 8080;
const QString ServerParam::DEFAULT_ADDRESS = QString("127.0.0.1");
const bool ServerParam::DEFAULT_ENABLED = false;

ServerParam::ServerParam()
{
    enabled = DEFAULT_ENABLED;
    address = DEFAULT_ADDRESS;
    port = DEFAULT_PORT;
}

QVariantMap ServerParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("enabled", enabled);
    paramMap.insert("address", address);
    paramMap.insert("port", port);
    return paramMap;
}


RtnStatus ServerParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;

    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Server parameter map is empty");
        return rtnStatus;
    }

    if (!paramMap.contains("enabled"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'enabled' field not found in server parameters");
        return rtnStatus;
    }
    if (!paramMap["enabled"].canConvert<bool>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert server parameter 'enabled' to bool");
        return rtnStatus;
    }
    enabled = paramMap["enabled"].toBool();


    if (!paramMap.contains("address"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'address' field not found in server parameters");
        return rtnStatus;
    }
    if (!paramMap["address"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert server parameter 'address' to string");
        return rtnStatus;
    }
    address = paramMap["address"].toString();


    if (!paramMap.contains("port"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'port' field not found in server parameters");
        return rtnStatus;
    }
    if (!paramMap["port"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unabled to convert server parameter 'port' to unsigned int");
        return rtnStatus;
    }
    port = paramMap["port"].toUInt();

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}


// IdentityTrackerParam
// -----------------------------------------------------------------------------
const float IdentityTrackerParam::DEFAULT_MEAN_DX = -5.5;
const float IdentityTrackerParam::DEFAULT_MEAN_DY = 574.0;
const float IdentityTrackerParam::DEFAULT_MEAN_WIDTH = 2.0;
const float IdentityTrackerParam::DEFAULT_MEAN_HEIGHT = 4.0;
const float IdentityTrackerParam::DEFAULT_STD_DX = 3.6050;
const float IdentityTrackerParam::DEFAULT_STD_DY = 14.4201;
const float IdentityTrackerParam::DEFAULT_STD_WIDTH = 5.7680;
const float IdentityTrackerParam::DEFAULT_STD_HEIGHT = 8.6521;
const float IdentityTrackerParam::DEFAULT_MAX_COST = 32.7148;


IdentityTrackerParam::IdentityTrackerParam()
{
    meanDx = DEFAULT_MEAN_DX;
    meanDy = DEFAULT_MEAN_DY;
    meanWidth = DEFAULT_MEAN_WIDTH;
    meanHeight = DEFAULT_MEAN_HEIGHT;
    stdDx = DEFAULT_STD_DX;
    stdDy = DEFAULT_STD_DY;
    stdWidth = DEFAULT_STD_WIDTH;
    stdHeight = DEFAULT_STD_HEIGHT;
    maxCost = DEFAULT_MAX_COST;
}


QVariantMap IdentityTrackerParam::toMap()
{
    // TO DO ///////////////////////////////////////
    QVariantMap paramMap;
    return paramMap;
}


RtnStatus IdentityTrackerParam::fromMap(QVariantMap paramMap)
{
    // TO DO /////////////////////////////////////
    RtnStatus rtnStatus;
    return rtnStatus;
}

// FlySorterParam
// ----------------------------------------------------------------------------

FlySorterParam::FlySorterParam()
{
    //server = ServerParam();
    //imageGrabber = ImageGrabberParam();
    //blobFinder = BlobFinderParam();
    //flySegmenter = FlySegmenterParam();
    //hogPositionFitter = HogPositionFitterParam();
    
    // Devel
    // -------------------------------
    genderMode = GenderModeMaleOnly;
    // -------------------------------
}


// Devel
// ----------------------------------------------------------------------------
static QMap<GenderMode, QString> createGenderModeToStringMap()
{
    QMap<GenderMode, QString> map;
    map[GenderModeMaleOnly] = QString("maleOnly");
    map[GenderModeEveryOther] = QString("everyOther");
    map[GenderModeRandom] = QString("random");
    return map;
}
static QMap<GenderMode, QString> genderModeToStringMap = createGenderModeToStringMap();
// -----------------------------------------------------------------------------

QVariantMap FlySorterParam::toMap()
{
    QVariantMap paramMap;
    QVariantMap serverParamMap = server.toMap();
    QVariantMap imageGrabberParamMap = imageGrabber.toMap();
    QVariantMap blobFinderParamMap = blobFinder.toMap();

    paramMap.insert("server", serverParamMap);
    paramMap.insert("imageGrabber", imageGrabberParamMap);
    paramMap.insert("blobFinder", blobFinderParamMap);

    // Devel
    // --------------------------------------------------------------
    paramMap.insert("genderMode", genderModeToStringMap[genderMode]);
    // --------------------------------------------------------------

    return paramMap;
}


QByteArray FlySorterParam::toJson()
{
    bool ok;
    QVariantMap paramMap = toMap();
    QByteArray json = QtJson::serialize(paramMap,ok);

    return json;
}


RtnStatus FlySorterParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;

    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("FlySorter parameter map is empty");
        return rtnStatus;
    }

    QVariantMap serverParamMap = paramMap["server"].toMap();
    rtnStatus = server.fromMap(serverParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    QVariantMap imageGrabberParamMap = paramMap["imageGrabber"].toMap();
    rtnStatus = imageGrabber.fromMap(imageGrabberParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    QVariantMap blobFinderParamMap = paramMap["blobFinder"].toMap();
    rtnStatus = blobFinder.fromMap(blobFinderParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    // Devel
    // --------------------------------------------------------------------------------------
    if (!paramMap.contains("genderMode"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'genderMode' field not found in parameters");
        return rtnStatus;
    }
    if (!paramMap["genderMode"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert 'genderMode' to string");
        return rtnStatus;
    }
    QString genderModeString = paramMap["genderMode"].toString();
    GenderMode genderModeTemp = genderModeToStringMap.key(genderModeString, GenderModeNotFound);
    if (genderModeTemp == GenderModeNotFound)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("unknown genderMode: ") + genderModeString;
        return rtnStatus;
    }
    genderMode = genderModeTemp;
    // -----------------------------------------------------------------------------------------

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;

}


RtnStatus FlySorterParam::fromJson(QByteArray paramJson)
{
    RtnStatus rtnStatus;
    return rtnStatus;
}



