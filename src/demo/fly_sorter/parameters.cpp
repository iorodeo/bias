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
    classifierParam.offset = 4.761805;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(111, 0.005153, 1.997284));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.997300));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.980325));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.981098));   
    stumpVector.push_back(StumpData(233, 0.015812, 1.896073));    
    stumpVector.push_back(StumpData(66,  0.313967, -1.770360));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.763635));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.808662));   
    stumpVector.push_back(StumpData(54,  0.312939, 1.613732));    
    stumpVector.push_back(StumpData(531, 0.002510, -1.486224));   
    stumpVector.push_back(StumpData(110, 0.014397, 1.479566));    
    stumpVector.push_back(StumpData(411, 0.002510, 1.635630));    
    stumpVector.push_back(StumpData(66,  0.313208, -1.370592));   
    stumpVector.push_back(StumpData(145, 0.043154, -1.532381));   
    stumpVector.push_back(StumpData(108, 0.054891, -1.454999));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.605764));    
    stumpVector.push_back(StumpData(394, 0.043492, -1.398667));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.566748));   
    stumpVector.push_back(StumpData(230, 0.027309, -1.433288));   
    stumpVector.push_back(StumpData(205, 0.043154, 1.503750));    
    stumpVector.push_back(StumpData(525, 0.024992, 1.466903));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.568232));   
    stumpVector.push_back(StumpData(228, 0.059592, 1.510708));    
    stumpVector.push_back(StumpData(0,   0.446984, -1.472128));   
    stumpVector.push_back(StumpData(931, 0.506259, -1.498702));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.547230));   
    stumpVector.push_back(StumpData(110, 0.026033, 1.385918));    
    stumpVector.push_back(StumpData(111, 0.005153, 1.577274));    
    stumpVector.push_back(StumpData(536, 0.161432, -1.283514));   
    stumpVector.push_back(StumpData(235, 0.022309, 1.497967));    
    stumpVector.push_back(StumpData(931, 0.484208, -1.415908));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.542772));    
    stumpVector.push_back(StumpData(937, 0.484208, 1.554802));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.592181));   
    stumpVector.push_back(StumpData(108, 0.053606, -1.415506));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.570012));    
    stumpVector.push_back(StumpData(230, 0.018522, -1.398479));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.550855));   
    stumpVector.push_back(StumpData(514, 0.043377, 1.327065));    
    stumpVector.push_back(StumpData(111, 0.005153, 1.529283));    
    stumpVector.push_back(StumpData(54,  0.312456, 1.398503));    
    stumpVector.push_back(StumpData(531, 0.001236, -1.541429));   
    stumpVector.push_back(StumpData(365, 0.141630, -1.442234));   
    stumpVector.push_back(StumpData(665, 0.141630, -1.493103));   
    stumpVector.push_back(StumpData(545, 0.135142, 1.444903));    
    stumpVector.push_back(StumpData(245, 0.135151, 1.481946));    
    stumpVector.push_back(StumpData(111, 0.005153, 1.540022));    
    stumpVector.push_back(StumpData(937, 0.507110, 1.521267));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.545716));   
    stumpVector.push_back(StumpData(931, 0.507110, -1.553907));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.542458));    
    stumpVector.push_back(StumpData(136, 0.042262, -1.479371));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.527223));   
    stumpVector.push_back(StumpData(394, 0.043015, -1.423756));   
    stumpVector.push_back(StumpData(115, 0.020788, -1.485082));   
    stumpVector.push_back(StumpData(937, 0.505944, 1.401107));    
    stumpVector.push_back(StumpData(111, 0.005153, 1.326513));    
    stumpVector.push_back(StumpData(416, 0.171521, 1.289109));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.545095));   
    stumpVector.push_back(StumpData(530, 0.085553, -1.367397));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.503630));    
    stumpVector.push_back(StumpData(937, 0.507110, 1.466218));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.564686));   
    stumpVector.push_back(StumpData(108, 0.055406, -1.438592));   
    stumpVector.push_back(StumpData(115, 0.020788, -1.526195));   
    stumpVector.push_back(StumpData(405, 0.022682, -1.335076));   
    stumpVector.push_back(StumpData(531, 0.002510, -1.471806));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.477652));    
    stumpVector.push_back(StumpData(848, 0.398383, -1.456283));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.466253));   
    stumpVector.push_back(StumpData(838, 0.407427, 1.375775));    
    stumpVector.push_back(StumpData(111, 0.005153, 1.500592));    
    stumpVector.push_back(StumpData(110, 0.026033, 1.368588));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.506868));   
    stumpVector.push_back(StumpData(514, 0.043014, 1.424255));    
    stumpVector.push_back(StumpData(111, 0.005153, 1.589899));    
    stumpVector.push_back(StumpData(931, 0.507110, -1.484133));   
    stumpVector.push_back(StumpData(235, 0.022309, 1.516570));    
    stumpVector.push_back(StumpData(937, 0.507110, 1.511945));    
    stumpVector.push_back(StumpData(411, 0.000712, 1.455992));    
    stumpVector.push_back(StumpData(66,  0.313843, -1.277873));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.535591));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.572875));    
    stumpVector.push_back(StumpData(528, 0.169284, 1.562822));    
    stumpVector.push_back(StumpData(110, 0.015213, 1.351780));    
    stumpVector.push_back(StumpData(231, 0.005649, -1.486438));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.556383));    
    stumpVector.push_back(StumpData(230, 0.025051, -1.508728));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.402381));   
    stumpVector.push_back(StumpData(108, 0.053859, -1.413812));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.443157));    
    stumpVector.push_back(StumpData(409, 0.062626, 1.477420));    
    stumpVector.push_back(StumpData(880, 0.058444, 1.471332));    
    stumpVector.push_back(StumpData(558, 0.013811, -1.299228));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.364377));   
    stumpVector.push_back(StumpData(258, 0.013649, -1.333313));   
    stumpVector.push_back(StumpData(123, 0.073918, -1.331727));   
    stumpVector.push_back(StumpData(111, 0.005153, 1.516822));    
    stumpVector.push_back(StumpData(230, 0.015138, -1.476504));   
    stumpVector.push_back(StumpData(231, 0.005649, -1.415337));   
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
    classifierParam.offset = 0.794138;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(237, 0.042877, 1.679062));
    stumpVector.push_back(StumpData(23,  0.270118, -1.316312));
    stumpVector.push_back(StumpData(237, 0.026423, 1.245334));
    stumpVector.push_back(StumpData(195, 0.025084, 1.080749));
    stumpVector.push_back(StumpData(722, 0.823248, 1.085636));
    stumpVector.push_back(StumpData(224, 0.006561, 1.054964));
    stumpVector.push_back(StumpData(744, 0.019471, -0.971514));
    stumpVector.push_back(StumpData(26,  0.237611, -1.067094));
    stumpVector.push_back(StumpData(537, 0.010454, 1.048541));
    stumpVector.push_back(StumpData(508, 0.038238, -1.008219));
    stumpVector.push_back(StumpData(393, 0.025607, -1.019474));
    stumpVector.push_back(StumpData(3,   0.573373, -0.826515));
    stumpVector.push_back(StumpData(536, 0.099670, 0.816819));
    stumpVector.push_back(StumpData(116, 0.111588, 0.837094));
    stumpVector.push_back(StumpData(237, 0.077446, 0.999763));
    stumpVector.push_back(StumpData(24,  0.308657, 0.865594));
    stumpVector.push_back(StumpData(124, 0.068966, 0.988446));
    stumpVector.push_back(StumpData(201, 0.000182, -0.907388));
    stumpVector.push_back(StumpData(793, 0.432495, 0.776103));
    stumpVector.push_back(StumpData(234, 0.052994, -0.875330));
    stumpVector.push_back(StumpData(115, 0.014838, 0.834883));
    stumpVector.push_back(StumpData(104, 0.004432, -0.817576));
    stumpVector.push_back(StumpData(196, 0.065199, 0.720465));
    stumpVector.push_back(StumpData(714, 0.013537, -0.829476));
    stumpVector.push_back(StumpData(774, 0.014311, -0.953926));
    stumpVector.push_back(StumpData(102, 0.780728, -0.755754));
    stumpVector.push_back(StumpData(208, 0.050641, -1.107044));
    stumpVector.push_back(StumpData(538, 0.001475, 0.894135));
    stumpVector.push_back(StumpData(236, 0.078608, 0.758925));
    stumpVector.push_back(StumpData(835, 0.159578, -0.824315));
    stumpVector.push_back(StumpData(71,  0.241703, -1.090395));
    stumpVector.push_back(StumpData(41,  0.242177, -1.052162));
    stumpVector.push_back(StumpData(744, 0.015131, -0.772634));
    stumpVector.push_back(StumpData(53,  0.246856, -0.935544));
    stumpVector.push_back(StumpData(224, 0.013737, 0.868950));
    stumpVector.push_back(StumpData(237, 0.079535, 0.790108));
    stumpVector.push_back(StumpData(746, 0.214250, 0.948540));
    stumpVector.push_back(StumpData(393, 0.033372, -0.799251));
    stumpVector.push_back(StumpData(35,  0.260653, 0.745195));
    stumpVector.push_back(StumpData(536, 0.077373, 0.824087));
    stumpVector.push_back(StumpData(116, 0.117831, 0.716873));
    stumpVector.push_back(StumpData(507, 0.811787, 0.838280));
    stumpVector.push_back(StumpData(29,  0.231383, -0.756047));
    stumpVector.push_back(StumpData(237, 0.004145, 1.137388));
    stumpVector.push_back(StumpData(227, 0.018530, -0.788310));
    stumpVector.push_back(StumpData(529, 0.054265, 0.734466));
    stumpVector.push_back(StumpData(102, 0.760770, -0.781407));
    stumpVector.push_back(StumpData(714, 0.015378, -0.704683));
    stumpVector.push_back(StumpData(483, 0.013178, -0.818376));
    stumpVector.push_back(StumpData(774, 0.013395, -0.891377));
    stumpVector.push_back(StumpData(208, 0.050641, -1.099520));
    stumpVector.push_back(StumpData(784, 0.174671, 0.710960));
    stumpVector.push_back(StumpData(744, 0.015131, -0.779436));
    stumpVector.push_back(StumpData(842, 0.001353, 0.880371));
    stumpVector.push_back(StumpData(474, 0.003228, 0.642934));
    stumpVector.push_back(StumpData(940, 0.285064, -0.754200));
    stumpVector.push_back(StumpData(537, 0.065706, 0.686359));
    stumpVector.push_back(StumpData(58,  0.524756, -0.808472));
    stumpVector.push_back(StumpData(393, 0.026245, -0.749056));
    stumpVector.push_back(StumpData(23,  0.266840, -0.799431));
    stumpVector.push_back(StumpData(714, 0.015850, -0.650027));
    stumpVector.push_back(StumpData(236, 0.103153, 0.726719));
    stumpVector.push_back(StumpData(416, 0.179039, 0.805731));
    stumpVector.push_back(StumpData(196, 0.079775, 0.750679));
    stumpVector.push_back(StumpData(393, 0.033372, -0.746353));
    stumpVector.push_back(StumpData(774, 0.015826, -0.721723));
    stumpVector.push_back(StumpData(224, 0.007777, 0.709855));
    stumpVector.push_back(StumpData(507, 0.727346, 0.711090));
    stumpVector.push_back(StumpData(23,  0.272076, -0.761647));
    stumpVector.push_back(StumpData(465, 0.003272, -0.770431));
    stumpVector.push_back(StumpData(744, 0.015454, -0.726664));
    stumpVector.push_back(StumpData(183, 0.102914, -0.785659));
    stumpVector.push_back(StumpData(536, 0.087596, 0.652548));
    stumpVector.push_back(StumpData(26,  0.235446, -0.901130));
    stumpVector.push_back(StumpData(469, 0.338153, -0.686035));
    stumpVector.push_back(StumpData(795, 0.177315, -0.821412));
    stumpVector.push_back(StumpData(722, 0.837404, 0.896744));
    stumpVector.push_back(StumpData(374, 0.005113, 0.654352));
    stumpVector.push_back(StumpData(744, 0.013449, -0.743855));
    stumpVector.push_back(StumpData(674, 0.005025, 0.801374));
    stumpVector.push_back(StumpData(162, 0.440108, -0.890458));
    stumpVector.push_back(StumpData(216, 0.069877, 0.716413));
    stumpVector.push_back(StumpData(508, 0.162047, -1.097683));
    stumpVector.push_back(StumpData(200, 0.022885, -0.712419));
    stumpVector.push_back(StumpData(713, 0.020344, -0.774992));
    stumpVector.push_back(StumpData(851, 0.067847, -0.868439));
    stumpVector.push_back(StumpData(23,  0.268746, -0.684516));
    stumpVector.push_back(StumpData(538, 0.000231, 0.808025));
    stumpVector.push_back(StumpData(93,  0.014283, -0.984035));
    stumpVector.push_back(StumpData(111, 0.052289, 0.683440));
    stumpVector.push_back(StumpData(537, 0.010454, 0.822474));
    stumpVector.push_back(StumpData(748, 0.010431, -0.762674));
    stumpVector.push_back(StumpData(175, 0.032175, -0.802316));
    stumpVector.push_back(StumpData(782, 0.026338, 0.615958));
    stumpVector.push_back(StumpData(937, 0.273159, 0.691528));
    stumpVector.push_back(StumpData(102, 0.770478, -0.678255));
    stumpVector.push_back(StumpData(537, 0.088277, 0.679651));
    stumpVector.push_back(StumpData(223, 0.019141, 0.789716));
    stumpVector.push_back(StumpData(229, 0.019736, 0.745659));
    stumpVector.push_back(StumpData(910, 0.229892, -0.716654));
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



