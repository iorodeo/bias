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
    classifierParam.offset = -0.8721;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(1, 0.3150,  1.9093));
    stumpVector.push_back(StumpData(0, 0.2288, -1.6624));
    stumpVector.push_back(StumpData(1, 0.3262,  1.12870));
    stumpVector.push_back(StumpData(0, 0.2573, -1.0649));
    stumpVector.push_back(StumpData(2, 0.4612, -1.0616));
    stumpVector.push_back(StumpData(1, 0.3373,  0.8464));
    stumpVector.push_back(StumpData(0, 0.2845, -0.8007));
    stumpVector.push_back(StumpData(0, 0.2288,  0.4021));
    stumpVector.push_back(StumpData(1, 0.3060,  0.5734));
    stumpVector.push_back(StumpData(0, 0.1959, -0.9434));
    stumpVector.push_back(StumpData(1, 0.3151, -0.4005));
    stumpVector.push_back(StumpData(0, 0.2157, -0.3686));
    stumpVector.push_back(StumpData(2, 0.4714, -0.7371));
    stumpVector.push_back(StumpData(1, 0.2985,  0.5921));
    stumpVector.push_back(StumpData(0, 0.2288,  0.2553));
    stumpVector.push_back(StumpData(0, 0.2830, -0.5507));
    stumpVector.push_back(StumpData(0, 0.3009,  1.0140));
    stumpVector.push_back(StumpData(1, 0.3421,  1.0053));
    stumpVector.push_back(StumpData(2, 0.5436, -0.5496));
    stumpVector.push_back(StumpData(0, 0.2369, -0.2400));
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
    stumpVector.push_back(StumpData(111,  0.002833, 1.983723));
    stumpVector.push_back(StumpData(231,  0.002833, -1.984202));
    stumpVector.push_back(StumpData(114,  0.008012, -1.959708));
    stumpVector.push_back(StumpData(234,  0.008211, 1.950887));
    stumpVector.push_back(StumpData(36,  0.306614, -1.851272));
    stumpVector.push_back(StumpData(24,  0.306401, 1.875527));
    stumpVector.push_back(StumpData(235,  0.016120, 1.680340));
    stumpVector.push_back(StumpData(201,  0.000407, -1.743562));
    stumpVector.push_back(StumpData(145,  0.027317, -1.706087));
    stumpVector.push_back(StumpData(141,  0.000407, 1.763328));
    stumpVector.push_back(StumpData(115,  0.015109, -1.735205));
    stumpVector.push_back(StumpData(205,  0.027317, 1.629330));
    stumpVector.push_back(StumpData(114,  0.011560, -1.437190));
    stumpVector.push_back(StumpData(235,  0.015109, 1.630453));
    stumpVector.push_back(StumpData(200,  0.011108, -1.603780));
    stumpVector.push_back(StumpData(140,  0.011108, 1.616170));
    stumpVector.push_back(StumpData(234,  0.011560, 1.495133));
    stumpVector.push_back(StumpData(115,  0.016120, -1.601133));
    stumpVector.push_back(StumpData(898,  0.275501, 1.520732));
    stumpVector.push_back(StumpData(910,  0.278920, -1.632418));
    stumpVector.push_back(StumpData(231,  0.003976, -1.556266));
    stumpVector.push_back(StumpData(201,  0.001525, -1.577791));
    stumpVector.push_back(StumpData(111,  0.003976, 1.648567));
    stumpVector.push_back(StumpData(141,  0.001525, 1.586624));
    stumpVector.push_back(StumpData(203,  0.019942, 1.588415));
    stumpVector.push_back(StumpData(415,  0.031681, -1.499144));
    stumpVector.push_back(StumpData(235,  0.012951, 1.621361));
    stumpVector.push_back(StumpData(110,  0.006901, 1.644504));
    stumpVector.push_back(StumpData(33,  0.311323, -1.517389));
    stumpVector.push_back(StumpData(234,  0.011560, 1.553500));
    stumpVector.push_back(StumpData(201,  0.002242, -1.598525));
    stumpVector.push_back(StumpData(140,  0.012292, 1.522593));
    stumpVector.push_back(StumpData(115,  0.012951, -1.594934));
    stumpVector.push_back(StumpData(231,  0.002833, -1.586161));
    stumpVector.push_back(StumpData(535,  0.031681, 1.591355));
    stumpVector.push_back(StumpData(143,  0.019891, -1.513393));
    stumpVector.push_back(StumpData(200,  0.003731, -1.542618));
    stumpVector.push_back(StumpData(27,  0.313313, 1.369082));
    stumpVector.push_back(StumpData(115,  0.016711, -1.449301));
    stumpVector.push_back(StumpData(141,  0.000071, 1.430221));
    stumpVector.push_back(StumpData(230,  0.007153, -1.574709));
    stumpVector.push_back(StumpData(143,  0.019942, -1.558897));
    stumpVector.push_back(StumpData(535,  0.031681, 1.438624));
    stumpVector.push_back(StumpData(114,  0.011560, -1.582286));
    stumpVector.push_back(StumpData(63,  0.329356, -1.562409));
    stumpVector.push_back(StumpData(383,  0.009421, 1.402641));
    stumpVector.push_back(StumpData(115,  0.013351, -1.419670));
    stumpVector.push_back(StumpData(27,  0.313648, 1.458448));
    stumpVector.push_back(StumpData(123,  0.064568, -1.353391));
    stumpVector.push_back(StumpData(231,  0.003976, -1.546833));
    stumpVector.push_back(StumpData(201,  0.000071, -1.448950));
    stumpVector.push_back(StumpData(111,  0.002833, 1.625674));
    stumpVector.push_back(StumpData(235,  0.016711, 1.558370));
    stumpVector.push_back(StumpData(898,  0.278920, 1.478915));
    stumpVector.push_back(StumpData(223,  0.010015, 1.430835));
    stumpVector.push_back(StumpData(110,  0.008508, 1.570713));
    stumpVector.push_back(StumpData(186,  0.267372, -1.617410));
    stumpVector.push_back(StumpData(806,  0.069382, 1.576354));
    stumpVector.push_back(StumpData(415,  0.032240, -1.557547));
    stumpVector.push_back(StumpData(141,  0.000156, 1.445432));
    stumpVector.push_back(StumpData(24,  0.306548, 1.598108));
    stumpVector.push_back(StumpData(235,  0.013351, 1.516873));
    stumpVector.push_back(StumpData(33,  0.313382, -1.527913));
    stumpVector.push_back(StumpData(114,  0.011560, -1.454917));
    stumpVector.push_back(StumpData(141,  0.001052, 1.581576));
    stumpVector.push_back(StumpData(143,  0.019942, -1.538615));
    stumpVector.push_back(StumpData(36,  0.306727, -1.625760));
    stumpVector.push_back(StumpData(115,  0.016711, -1.523061));
    stumpVector.push_back(StumpData(234,  0.011560, 1.427218));
    stumpVector.push_back(StumpData(201,  0.000914, -1.565892));
    stumpVector.push_back(StumpData(203,  0.019891, 1.565615));
    stumpVector.push_back(StumpData(140,  0.012014, 1.623432));
    stumpVector.push_back(StumpData(235,  0.016711, 1.335551));
    stumpVector.push_back(StumpData(200,  0.011121, -1.536721));
    stumpVector.push_back(StumpData(126,  0.228118, 1.583106));
    stumpVector.push_back(StumpData(910,  0.278920, -1.405823));
    stumpVector.push_back(StumpData(231,  0.002833, -1.540978));
    stumpVector.push_back(StumpData(115,  0.012951, -1.595306));
    stumpVector.push_back(StumpData(535,  0.031681, 1.536796));
    stumpVector.push_back(StumpData(786,  0.068775, -1.476896));
    stumpVector.push_back(StumpData(186,  0.272392, -1.488366));
    stumpVector.push_back(StumpData(111,  0.002833, 1.546401));
    stumpVector.push_back(StumpData(141,  0.000914, 1.513372));
    stumpVector.push_back(StumpData(268,  0.112592, 0.809542));
    stumpVector.push_back(StumpData(268,  0.112592, 0.997646));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
    stumpVector.push_back(StumpData(268,  0.112592, 1.000000));
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
    classifierParam.offset = -4.330167;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(747, 0.552426,  1.332248  ));
    stumpVector.push_back(StumpData(748, 0.047331,  -1.181514 ));
    stumpVector.push_back(StumpData(742, 0.209210,  -1.170904 ));
    stumpVector.push_back(StumpData(744, 0.010089,  -1.037700 ));
    stumpVector.push_back(StumpData(746, 0.144843,  1.035015  ));
    stumpVector.push_back(StumpData(117, 0.037167,  0.917172  ));
    stumpVector.push_back(StumpData(237, 0.037307,  1.076680  ));
    stumpVector.push_back(StumpData(748, 0.080281,  -0.907830 ));
    stumpVector.push_back(StumpData(744, 0.012593,  -0.829523 ));
    stumpVector.push_back(StumpData(722, 0.833534,  1.031670  ));
    stumpVector.push_back(StumpData(774, 0.010362,  -0.836172 ));
    stumpVector.push_back(StumpData(748, 0.007793,  -0.687378 ));
    stumpVector.push_back(StumpData(117, 0.025455,  0.769925  ));
    stumpVector.push_back(StumpData(237, 0.025455,  0.759249  ));
    stumpVector.push_back(StumpData(88,  0.513583,  -0.771606 ));
    stumpVector.push_back(StumpData(747, 0.573543,  0.886818  ));
    stumpVector.push_back(StumpData(107, 0.029338,  -0.740754 ));
    stumpVector.push_back(StumpData(237, 0.003953,  0.998502  ));
    stumpVector.push_back(StumpData(171, 0.000075,  0.533546  ));
    stumpVector.push_back(StumpData(166, 0.048281,  0.618541  ));
    stumpVector.push_back(StumpData(164, 0.029264,  -0.831243 ));
    stumpVector.push_back(StumpData(722, 0.813944,  0.581440  ));
    stumpVector.push_back(StumpData(744, 0.019939,  -0.592049 ));
    stumpVector.push_back(StumpData(735, 0.015591,  -0.694330 ));
    stumpVector.push_back(StumpData(117, 0.005885,  0.895744  ));
    stumpVector.push_back(StumpData(227, 0.027985,  -0.605341 ));
    stumpVector.push_back(StumpData(747, 0.590928,  0.972457  ));
    stumpVector.push_back(StumpData(708, 0.038157,  -0.623028 ));
    stumpVector.push_back(StumpData(745, 0.070582,  0.907067  ));
    stumpVector.push_back(StumpData(85,  0.493653,  0.515453  ));
    stumpVector.push_back(StumpData(694, 0.102143,  -0.624770 ));
    stumpVector.push_back(StumpData(895, 0.088737,  -1.009756 ));
    stumpVector.push_back(StumpData(747, 0.525455,  0.520227  ));
    stumpVector.push_back(StumpData(748, 0.037681,  -0.638402 ));
    stumpVector.push_back(StumpData(777, 0.233445,  1.010429  ));
    stumpVector.push_back(StumpData(735, 0.032632,  0.597317  ));
    stumpVector.push_back(StumpData(86,  0.216773,  -0.903492 ));
    stumpVector.push_back(StumpData(114, 0.041495,  -0.735267 ));
    stumpVector.push_back(StumpData(234, 0.041074,  -0.760666 ));
    stumpVector.push_back(StumpData(770, 0.068553,  0.763883  ));
    stumpVector.push_back(StumpData(37,  0.495089,  0.864682  ));
    stumpVector.push_back(StumpData(25,  0.494835,  0.794975  ));
    stumpVector.push_back(StumpData(744, 0.007176,  -0.743857 ));
    stumpVector.push_back(StumpData(605, 0.115354,  0.497490  ));
    stumpVector.push_back(StumpData(29,  0.238983,  -0.600424 ));
    stumpVector.push_back(StumpData(469, 0.295565,  -0.537216 ));
    stumpVector.push_back(StumpData(74,  0.272703,  -0.743861 ));
    stumpVector.push_back(StumpData(868, 0.428153,  0.535790  ));
    stumpVector.push_back(StumpData(417, 0.003677,  0.937953  ));
    stumpVector.push_back(StumpData(717, 0.166292,  0.671475  ));
    stumpVector.push_back(StumpData(748, 0.099934,  -0.844479 ));
    stumpVector.push_back(StumpData(744, 0.008854,  -0.686170 ));
    stumpVector.push_back(StumpData(272, 0.381746,  0.503284  ));
    stumpVector.push_back(StumpData(733, 0.037054,  0.479805  ));
    stumpVector.push_back(StumpData(748, 0.001123,  -1.006814 ));
    stumpVector.push_back(StumpData(116, 0.096507,  0.533441  ));
    stumpVector.push_back(StumpData(537, 0.026718,  0.642790  ));
    stumpVector.push_back(StumpData(837, 0.010107,  0.838267  ));
    stumpVector.push_back(StumpData(634, 0.024839,  -0.822938 ));
    stumpVector.push_back(StumpData(117, 0.054646,  0.587862  ));
    stumpVector.push_back(StumpData(197, 0.048240,  -0.488476 ));
    stumpVector.push_back(StumpData(747, 0.563768,  0.780115  ));
    stumpVector.push_back(StumpData(746, 0.127821,  0.579283  ));
    stumpVector.push_back(StumpData(710, 0.000601,  -0.786123 ));
    stumpVector.push_back(StumpData(314, 0.049101,  -0.947039 ));
    stumpVector.push_back(StumpData(739, 0.198971,  0.573109  ));
    stumpVector.push_back(StumpData(734, 0.025315,  0.742586  ));
    stumpVector.push_back(StumpData(93,  0.016105,  -0.666657 ));
    stumpVector.push_back(StumpData(237, 0.046837,  0.531537  ));
    stumpVector.push_back(StumpData(137, 0.048680,  -0.535374 ));
    stumpVector.push_back(StumpData(537, 0.005014,  0.900371  ));
    stumpVector.push_back(StumpData(305, 0.048826,  0.644678  ));
    stumpVector.push_back(StumpData(712, 0.670698,  -0.542980 ));
    stumpVector.push_back(StumpData(747, 0.419740,  -0.683620 ));
    stumpVector.push_back(StumpData(417, 0.049194,  0.597477  ));
    stumpVector.push_back(StumpData(213, 0.016073,  -0.750703 ));
    stumpVector.push_back(StumpData(718, 0.005868,  -0.727902 ));
    stumpVector.push_back(StumpData(236, 0.109333,  0.658714  ));
    stumpVector.push_back(StumpData(777, 0.152128,  0.472960  ));
    stumpVector.push_back(StumpData(847, 0.009769,  0.770571  ));
    stumpVector.push_back(StumpData(749, 0.002882,  -1.069233 ));
    stumpVector.push_back(StumpData(117, 0.037167,  0.526733  ));
    stumpVector.push_back(StumpData(741, 0.062062,  0.516998  ));
    stumpVector.push_back(StumpData(949, 0.412067,  -0.678172 ));
    stumpVector.push_back(StumpData(747, 0.581525,  0.886188  ));
    stumpVector.push_back(StumpData(774, 0.012316,  -0.514763 ));
    stumpVector.push_back(StumpData(694, 0.116054,  -0.636291 ));
    stumpVector.push_back(StumpData(821, 0.151939,  -1.025935 ));
    stumpVector.push_back(StumpData(156, 0.041279,  -1.031093 ));
    stumpVector.push_back(StumpData(236, 0.046746,  0.518403  ));
    stumpVector.push_back(StumpData(739, 0.125365,  -0.507752 ));
    stumpVector.push_back(StumpData(754, 0.082342,  -0.521292 ));
    stumpVector.push_back(StumpData(744, 0.014482,  -0.512433 ));
    stumpVector.push_back(StumpData(746, 0.152860,  0.555454  ));
    stumpVector.push_back(StumpData(934, 0.209993,  0.703834  ));
    stumpVector.push_back(StumpData(775, 0.117112,  -0.852262 ));
    stumpVector.push_back(StumpData(606, 0.167780,  -0.475018 ));
    stumpVector.push_back(StumpData(448, 0.206307,  -0.903193 ));
    stumpVector.push_back(StumpData(712, 0.731218,  -0.475890 ));
    stumpVector.push_back(StumpData(714, 0.014587,  -0.471714 ));
    classifierParam.stumpVector = stumpVector;
    return classifierParam;
}
const ClassifierParam GenderSorterParam::DEFAULT_GENDER_CLASSIFIER_PARAM = 
createDefaultGenderClassifierParam();

const double GenderSorterParam::DEFAULT_MIN_CONFIDENCE = 2.2412; 

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
    float brightnessTemp = paramMap["brightness"].toUInt();
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
    gamma = gammaTemp;

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
const MotionDirection IdentityTrackerParam::DEFAULT_MOTION_DIRECTION = MOTION_DIRECTION_Y;


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
    motionDirection = DEFAULT_MOTION_DIRECTION;
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



