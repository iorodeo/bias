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
    stumpVector.push_back(StumpData(747, 0.552426, 1.328007 ));
    stumpVector.push_back(StumpData(748, 0.047331, -1.176707));
    stumpVector.push_back(StumpData(742, 0.209210, -1.168777));
    stumpVector.push_back(StumpData(744, 0.010089, -1.034803));
    stumpVector.push_back(StumpData(746, 0.144843, 1.037088 ));
    stumpVector.push_back(StumpData(117, 0.037167, 0.919600 ));
    stumpVector.push_back(StumpData(237, 0.037307, 1.079700 ));
    stumpVector.push_back(StumpData(748, 0.080281, -0.904561));
    stumpVector.push_back(StumpData(744, 0.012593, -0.832781));
    stumpVector.push_back(StumpData(722, 0.833534, 1.029874 ));
    stumpVector.push_back(StumpData(774, 0.010362, -0.830556));
    stumpVector.push_back(StumpData(748, 0.007793, -0.678468));
    stumpVector.push_back(StumpData(117, 0.025455, 0.766135 ));
    stumpVector.push_back(StumpData(237, 0.025455, 0.753638 ));
    stumpVector.push_back(StumpData(88,  0.513583, -0.780945));
    stumpVector.push_back(StumpData(747, 0.573543, 0.884628 ));
    stumpVector.push_back(StumpData(107, 0.029338, -0.737257));
    stumpVector.push_back(StumpData(234, 0.049589, -0.905012));
    stumpVector.push_back(StumpData(712, 0.715817, -0.559004));
    stumpVector.push_back(StumpData(714, 0.010984, -0.768567));
    stumpVector.push_back(StumpData(152, 0.604613, 0.640303 ));
    stumpVector.push_back(StumpData(114, 0.049481, -0.901765));
    stumpVector.push_back(StumpData(227, 0.029421, -0.615051));
    stumpVector.push_back(StumpData(742, 0.234878, -0.550624));
    stumpVector.push_back(StumpData(605, 0.152754, 0.626072 ));
    stumpVector.push_back(StumpData(734, 0.024880, 0.738883 ));
    stumpVector.push_back(StumpData(871, 0.137090, -0.974634));
    stumpVector.push_back(StumpData(87,  0.329897, 0.766801 ));
    stumpVector.push_back(StumpData(747, 0.602612, 1.015492 ));
    stumpVector.push_back(StumpData(694, 0.102143, -0.656196));
    stumpVector.push_back(StumpData(748, 0.001308, -1.008044));
    stumpVector.push_back(StumpData(117, 0.005885, 0.864977 ));
    stumpVector.push_back(StumpData(164, 0.029264, -0.863017));
    stumpVector.push_back(StumpData(237, 0.003953, 0.929472 ));
    stumpVector.push_back(StumpData(744, 0.005962, -0.928824));
    stumpVector.push_back(StumpData(748, 0.099934, -0.836794));
    stumpVector.push_back(StumpData(37,  0.494834, 0.767344 ));
    stumpVector.push_back(StumpData(733, 0.036080, 0.520066 ));
    stumpVector.push_back(StumpData(112, 0.804892, -0.609155));
    stumpVector.push_back(StumpData(893, 0.518515, 0.532040 ));
    stumpVector.push_back(StumpData(722, 0.813944, 0.577629 ));
    stumpVector.push_back(StumpData(744, 0.019939, -0.555738));
    stumpVector.push_back(StumpData(745, 0.077640, 1.060644 ));
    stumpVector.push_back(StumpData(234, 0.040515, -0.743278));
    stumpVector.push_back(StumpData(116, 0.101662, 0.634621 ));
    stumpVector.push_back(StumpData(236, 0.100970, 0.543552 ));
    stumpVector.push_back(StumpData(114, 0.040810, -0.779448));
    stumpVector.push_back(StumpData(25,  0.494835, 0.775433 ));
    stumpVector.push_back(StumpData(469, 0.295316, -0.506533));
    stumpVector.push_back(StumpData(748, 0.039653, -0.603211));
    stumpVector.push_back(StumpData(747, 0.525455, 0.633338 ));
    stumpVector.push_back(StumpData(724, 0.045187, -0.618860));
    stumpVector.push_back(StumpData(895, 0.088737, -1.006523));
    stumpVector.push_back(StumpData(232, 0.805018, -0.599618));
    stumpVector.push_back(StumpData(888, 0.366577, -0.864050));
    stumpVector.push_back(StumpData(777, 0.234797, 0.965736 ));
    stumpVector.push_back(StumpData(717, 0.166292, 0.688325 ));
    stumpVector.push_back(StumpData(708, 0.038022, -0.588270));
    stumpVector.push_back(StumpData(745, 0.066947, 0.794975 ));
    stumpVector.push_back(StumpData(605, 0.102551, 0.468246 ));
    stumpVector.push_back(StumpData(739, 0.125365, -0.555096));
    stumpVector.push_back(StumpData(739, 0.220670, 0.840560 ));
    stumpVector.push_back(StumpData(617, 0.025901, 0.680421 ));
    stumpVector.push_back(StumpData(744, 0.008854, -0.703597));
    stumpVector.push_back(StumpData(746, 0.127821, 0.508122 ));
    stumpVector.push_back(StumpData(747, 0.571340, 0.808615 ));
    stumpVector.push_back(StumpData(870, 0.187573, -0.504692));
    stumpVector.push_back(StumpData(117, 0.002822, 0.976552 ));
    stumpVector.push_back(StumpData(710, 0.023810, 0.565375 ));
    stumpVector.push_back(StumpData(237, 0.003953, 0.976466 ));
    stumpVector.push_back(StumpData(837, 0.009045, 0.795523 ));
    stumpVector.push_back(StumpData(821, 0.151939, -1.053570));
    stumpVector.push_back(StumpData(237, 0.045661, 0.498195 ));
    stumpVector.push_back(StumpData(117, 0.037167, 0.559082 ));
    stumpVector.push_back(StumpData(847, 0.009045, 0.838301 ));
    stumpVector.push_back(StumpData(634, 0.024937, -0.815767));
    stumpVector.push_back(StumpData(738, 0.240009, -0.890572));
    stumpVector.push_back(StumpData(625, 0.118402, 0.752017 ));
    stumpVector.push_back(StumpData(153, 0.053881, 0.471172 ));
    stumpVector.push_back(StumpData(746, 0.154624, 0.514382 ));
    stumpVector.push_back(StumpData(747, 0.560162, 0.708078 ));
    stumpVector.push_back(StumpData(622, 0.399805, 0.568538 ));
    stumpVector.push_back(StumpData(617, 0.058626, 0.505751 ));
    stumpVector.push_back(StumpData(722, 0.836120, 1.022143 ));
    stumpVector.push_back(StumpData(774, 0.012316, -0.478431));
    stumpVector.push_back(StumpData(770, 0.068553, 0.689973 ));
    stumpVector.push_back(StumpData(84,  0.307930, 0.582869 ));
    stumpVector.push_back(StumpData(417, 0.003677, 0.901574 ));
    stumpVector.push_back(StumpData(537, 0.026378, 0.577327 ));
    stumpVector.push_back(StumpData(414, 0.040863, -0.568257));
    stumpVector.push_back(StumpData(748, 0.072650, -0.703587));
    stumpVector.push_back(StumpData(744, 0.012136, -0.577338));
    stumpVector.push_back(StumpData(612, 0.876586, -1.008477));
    stumpVector.push_back(StumpData(842, 0.001435, 0.496803 ));
    stumpVector.push_back(StumpData(474, 0.010021, 0.586252 ));
    stumpVector.push_back(StumpData(112, 0.749011, -0.489467));
    stumpVector.push_back(StumpData(895, 0.088737, -0.928044));
    stumpVector.push_back(StumpData(237, 0.045661, 0.496098 ));
    stumpVector.push_back(StumpData(417, 0.059349, 0.535036 ));
    stumpVector.push_back(StumpData(718, 0.005873, -0.685709));
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



