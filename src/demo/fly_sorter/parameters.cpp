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

    // segparams updated by Kristin 20140117T173726
    classifierParam.offset =  0.73614195;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(1,  0.31276043,  1.94334469));
    stumpVector.push_back(StumpData(0,  0.23544378, -1.77419269));
    stumpVector.push_back(StumpData(1,  0.32148740,  1.30897762));
    stumpVector.push_back(StumpData(0,  0.29092346, -1.21521216));
    stumpVector.push_back(StumpData(0,  0.20872100, -0.86132894));
    stumpVector.push_back(StumpData(0,  0.29197285, -1.03318829));
    stumpVector.push_back(StumpData(0,  0.19470003, -1.04885953));
    stumpVector.push_back(StumpData(0,  0.23544378,  0.35036685));
    stumpVector.push_back(StumpData(0,  0.24825421, -0.48337332));
    stumpVector.push_back(StumpData(0,  0.23544378,  0.32232223));
    stumpVector.push_back(StumpData(2,  0.52794573,  0.39155564));
    stumpVector.push_back(StumpData(2,  0.47351477, -0.76623862));
    stumpVector.push_back(StumpData(1,  0.28482068,  0.96753708));
    stumpVector.push_back(StumpData(0,  0.29197285, -0.76822353));
    stumpVector.push_back(StumpData(1,  0.33939977,  1.01911647));
    stumpVector.push_back(StumpData(1,  0.32078697, -0.24614010));
    stumpVector.push_back(StumpData(1,  0.32752502,  0.38768252));
    stumpVector.push_back(StumpData(1,  0.32262014, -0.20891932));
    stumpVector.push_back(StumpData(1,  0.29844978,  0.30034203));
    stumpVector.push_back(StumpData(1,  0.31277548, -0.23097036));

    /*classifierParam.offset = -0.8721;
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
    stumpVector.push_back(StumpData(0, 0.2369, -0.2400));*/
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
const unsigned int HogPositionFitterParam::DEFAULT_MAX_BODY_AREA = 22000;
const cv::Scalar HogPositionFitterParam::DEFAULT_FILL_VALUES_LUV =
cv::Scalar(0.3703704,0.3259259,0.4962940); 
const double HogPositionFitterParam::DEFAULT_PAD_BORDER = 15.0;

// TEMPORARY 
ClassifierParam HogPositionFitterParam::createDefaultOrientClassifierParam()
{
    ClassifierParam classifierParam;

    // fitparams updated by Kristin 20140120T092316
    classifierParam.offset =  3.52734583;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData( 231,  0.00433047, -1.99217993));
    stumpVector.push_back(StumpData( 111,  0.00510562,  1.96838774));
    stumpVector.push_back(StumpData( 113,  0.01552412, -1.82337523));
    stumpVector.push_back(StumpData(  66,  0.31352213, -1.42060354));
    stumpVector.push_back(StumpData( 212,  0.28589125,  1.17558894));
    stumpVector.push_back(StumpData( 393,  0.04954102, -1.13703989));
    stumpVector.push_back(StumpData( 931,  0.32719810,  0.99743578));
    stumpVector.push_back(StumpData( 937,  0.32985195, -0.94480162));
    stumpVector.push_back(StumpData(  36,  0.30584932, -1.04445683));
    stumpVector.push_back(StumpData( 242,  0.07034156, -0.99589116));
    stumpVector.push_back(StumpData( 362,  0.07166760,  1.00116727));
    stumpVector.push_back(StumpData( 435,  0.03260064, -1.00933615));
    stumpVector.push_back(StumpData( 141,  0.00101472,  0.91704653));
    stumpVector.push_back(StumpData( 542,  0.07019174, -0.91396022));
    stumpVector.push_back(StumpData( 203,  0.03306559,  0.99134923));
    stumpVector.push_back(StumpData( 495,  0.03259708,  0.92829392));
    stumpVector.push_back(StumpData( 416,  0.18174656,  0.94001127));
    stumpVector.push_back(StumpData( 134,  0.01697758, -0.84652582));
    stumpVector.push_back(StumpData( 125,  0.28486411,  0.86989468));
    stumpVector.push_back(StumpData( 235,  0.01375048,  1.05186704));
    stumpVector.push_back(StumpData( 238,  0.00017490,  0.85947690));
    stumpVector.push_back(StumpData( 237,  0.02615592, -1.11193017));
    stumpVector.push_back(StumpData( 662,  0.07159743,  0.74698929));
    stumpVector.push_back(StumpData( 928,  0.36011947,  0.85300434));
    stumpVector.push_back(StumpData( 803,  0.62172833, -0.79788397));
    stumpVector.push_back(StumpData( 216,  0.42144009,  0.92069764));
    stumpVector.push_back(StumpData( 443,  0.01724115, -1.01097122));
    stumpVector.push_back(StumpData( 229,  0.03007054, -0.92436518));
    stumpVector.push_back(StumpData( 114,  0.00516050, -1.00935845));
    stumpVector.push_back(StumpData( 498,  0.29914955, -0.85645734));
    stumpVector.push_back(StumpData(  54,  0.31252188,  0.70146753));
    stumpVector.push_back(StumpData( 394,  0.03489866,  0.94316198));
    stumpVector.push_back(StumpData( 940,  0.36312280, -0.80342977));
    stumpVector.push_back(StumpData(  62,  0.20137945, -0.91297341));
    stumpVector.push_back(StumpData( 852,  0.01222247, -0.77309355));
    stumpVector.push_back(StumpData(  92,  0.27530334, -0.94469569));
    stumpVector.push_back(StumpData( 790,  0.14098412, -0.63910542));
    stumpVector.push_back(StumpData( 194,  0.01820960,  0.73730268));
    stumpVector.push_back(StumpData( 215,  0.51556271,  0.90791733));
    stumpVector.push_back(StumpData( 234,  0.00516050,  0.91740744));
    stumpVector.push_back(StumpData( 207,  0.39737556,  0.67630727));
    stumpVector.push_back(StumpData( 531,  0.06032561, -0.98927221));
    stumpVector.push_back(StumpData( 115,  0.01375048, -0.92914779));
    stumpVector.push_back(StumpData( 447,  0.68409338, -0.74148611));
    stumpVector.push_back(StumpData(  24,  0.30576027,  0.76902155));
    stumpVector.push_back(StumpData( 524,  0.01477767, -0.93783901));
    stumpVector.push_back(StumpData( 139,  0.13757937,  0.66954519));
    stumpVector.push_back(StumpData(  96,  0.19490717,  0.67120763));
    stumpVector.push_back(StumpData(  96,  0.42144009, -0.81947721));
    stumpVector.push_back(StumpData( 111,  0.02971637,  0.82999280));
    stumpVector.push_back(StumpData( 907,  0.26277293,  0.73469238));
    stumpVector.push_back(StumpData( 854,  0.35845892,  0.90063501));
    stumpVector.push_back(StumpData( 536,  0.21495675, -0.80988801));
    stumpVector.push_back(StumpData( 937,  0.32721301, -0.65146105));
    stumpVector.push_back(StumpData(  24,  0.29821021,  1.10043063));
    stumpVector.push_back(StumpData(  37,  0.48635259,  0.77490206));
    stumpVector.push_back(StumpData( 529,  0.00342526, -1.09942592));
    stumpVector.push_back(StumpData( 109,  0.03007054,  0.77116895));
    stumpVector.push_back(StumpData( 438,  0.22394498,  0.64640246));
    stumpVector.push_back(StumpData( 141,  0.00078756,  0.70288590));
    stumpVector.push_back(StumpData( 438,  0.47246133,  0.99116307));
    stumpVector.push_back(StumpData( 800,  0.13994240,  0.64356416));
    stumpVector.push_back(StumpData(  56,  0.20055552,  0.75447223));
    stumpVector.push_back(StumpData( 507,  0.55637299, -0.92196167));
    stumpVector.push_back(StumpData( 118,  0.00017490, -0.73699547));
    stumpVector.push_back(StumpData(  24,  0.30576027,  0.81306584));
    stumpVector.push_back(StumpData( 147,  0.39840989, -0.86145533));
    stumpVector.push_back(StumpData( 531,  0.06032561, -0.86459059));
    stumpVector.push_back(StumpData( 393,  0.19924506, -1.06854361));
    stumpVector.push_back(StumpData( 404,  0.01404138,  0.78833184));
    stumpVector.push_back(StumpData(  11,  0.44930960,  1.09201700));
    stumpVector.push_back(StumpData( 791,  0.10653392, -0.96280510));
    stumpVector.push_back(StumpData( 411,  0.06032561,  0.89555985));
    stumpVector.push_back(StumpData( 498,  0.47246133, -0.98970178));
    stumpVector.push_back(StumpData( 803,  0.62208176, -0.69891254));
    stumpVector.push_back(StumpData( 802,  0.02919351,  0.81380363));
    stumpVector.push_back(StumpData( 513,  0.04962270,  0.67938764));
    stumpVector.push_back(StumpData( 507,  0.68573859,  0.69880613));
    stumpVector.push_back(StumpData(  36,  0.30584932, -0.78712958));
    stumpVector.push_back(StumpData( 801,  0.09733206,  0.82222170));
    stumpVector.push_back(StumpData( 201,  0.00091325, -0.71542182));
    stumpVector.push_back(StumpData( 514,  0.04813764, -0.72936257));
    stumpVector.push_back(StumpData(  36,  0.29821021, -1.02526379));
    stumpVector.push_back(StumpData(  58,  0.52803123, -0.64406424));
    stumpVector.push_back(StumpData( 443,  0.01814573, -0.89106943));
    stumpVector.push_back(StumpData( 503,  0.01722874,  0.85716827));
    stumpVector.push_back(StumpData( 799,  0.27226047,  0.84122186));
    stumpVector.push_back(StumpData( 444,  0.00136816, -0.70660544));
    stumpVector.push_back(StumpData(  62,  0.19842071, -0.90364336));
    stumpVector.push_back(StumpData( 784,  0.32278489, -0.93094119));
    stumpVector.push_back(StumpData( 901,  0.25935876, -0.68366244));
    stumpVector.push_back(StumpData( 114,  0.00516050, -0.82586748));
    stumpVector.push_back(StumpData(  25,  0.49023360, -0.64908920));
    stumpVector.push_back(StumpData( 444,  0.01772461, -0.94805758));
    stumpVector.push_back(StumpData( 786,  0.05025244, -0.67711307));
    stumpVector.push_back(StumpData(   4,  0.42371166,  0.82802652));
    stumpVector.push_back(StumpData( 832,  0.01122690,  0.60569568));
    stumpVector.push_back(StumpData(  93,  0.01922718, -0.81277842));
    stumpVector.push_back(StumpData( 194,  0.01798670,  0.67094715));
    stumpVector.push_back(StumpData(  37,  0.49023217,  0.64724399));

    /* 
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
    */


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

    // classifyparams updated by Kristin 20140120T092603
    classifierParam.offset =  1.18248158;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData( 237,  0.02471136,  1.40326083));
    stumpVector.push_back(StumpData(  23,  0.27051006, -1.30399325));
    stumpVector.push_back(StumpData( 237,  0.05244871,  1.10529684));
    stumpVector.push_back(StumpData( 748,  0.00556087, -1.08379787));
    stumpVector.push_back(StumpData( 237,  0.00854558,  1.00379993));
    stumpVector.push_back(StumpData(  26,  0.23818526, -0.99473626));
    stumpVector.push_back(StumpData( 747,  0.61649896,  1.03412624));
    stumpVector.push_back(StumpData( 416,  0.19268831,  0.86358426));
    stumpVector.push_back(StumpData( 223,  0.01196712,  0.70163357));
    stumpVector.push_back(StumpData( 744,  0.02150941, -0.67887279));
    stumpVector.push_back(StumpData( 940,  0.22765781, -0.67012095));
    stumpVector.push_back(StumpData(   3,  0.54251363, -0.68735824));
    stumpVector.push_back(StumpData( 393,  0.03138413, -0.66256328));
    stumpVector.push_back(StumpData( 204,  0.06349167, -0.77987107));
    stumpVector.push_back(StumpData( 745,  0.07530800,  0.77619866));
    stumpVector.push_back(StumpData( 234,  0.05167496, -0.68420196));
    stumpVector.push_back(StumpData( 748,  0.03179249, -0.92419994));
    stumpVector.push_back(StumpData( 722,  0.83880151,  0.76019743));
    stumpVector.push_back(StumpData( 744,  0.01401811, -0.71219827));
    stumpVector.push_back(StumpData( 111,  0.04214038,  0.71534685));
    stumpVector.push_back(StumpData( 466,  0.02108432, -0.70887282));
    stumpVector.push_back(StumpData( 714,  0.01494192, -0.60813572));
    stumpVector.push_back(StumpData( 774,  0.01522874, -0.70880495));
    stumpVector.push_back(StumpData(  23,  0.26816564, -0.71071249));
    stumpVector.push_back(StumpData( 536,  0.04881695,  0.60945549));
    stumpVector.push_back(StumpData( 194,  0.01715390,  0.58673049));
    stumpVector.push_back(StumpData( 507,  0.75174107,  0.63459316));
    stumpVector.push_back(StumpData( 724,  0.03539805, -0.59735507));
    stumpVector.push_back(StumpData( 842,  0.00396335,  0.47099450));
    stumpVector.push_back(StumpData(  71,  0.24190818, -0.78324076));
    stumpVector.push_back(StumpData( 204,  0.07809971, -0.85300970));
    stumpVector.push_back(StumpData(  41,  0.24197207, -0.86590619));
    stumpVector.push_back(StumpData( 102,  0.76955951, -0.62206441));
    stumpVector.push_back(StumpData( 173,  0.03221380, -0.71467410));
    stumpVector.push_back(StumpData( 744,  0.01746727, -0.51976912));
    stumpVector.push_back(StumpData( 224,  0.00939989,  0.50727622));
    stumpVector.push_back(StumpData( 107,  0.00717385,  0.62165325));
    stumpVector.push_back(StumpData( 508,  0.00232423, -0.43680219));
    stumpVector.push_back(StumpData( 937,  0.27861972,  0.45693652));
    stumpVector.push_back(StumpData( 910,  0.19835013, -0.45717917));
    stumpVector.push_back(StumpData( 853,  0.32128401,  0.59906855));
    stumpVector.push_back(StumpData( 409,  0.12003075,  0.83180298));
    stumpVector.push_back(StumpData( 714,  0.01161940, -0.52491771));
    stumpVector.push_back(StumpData( 416,  0.12662429,  0.75732672));
    stumpVector.push_back(StumpData( 657,  0.29019733,  0.75291472));
    stumpVector.push_back(StumpData( 940,  0.37514197, -0.94162507));
    stumpVector.push_back(StumpData( 528,  0.00128950,  0.59585302));
    stumpVector.push_back(StumpData( 227,  0.01497655, -0.47113698));
    stumpVector.push_back(StumpData( 200,  0.03079778, -0.46465845));
    stumpVector.push_back(StumpData( 197,  0.05139876, -0.62725561));
    stumpVector.push_back(StumpData( 196,  0.03084996,  0.92384134));
    stumpVector.push_back(StumpData(  80,  0.27018611,  0.41598678));
    stumpVector.push_back(StumpData( 232,  0.75232476, -0.45505973));
    stumpVector.push_back(StumpData(  50,  0.26885590,  0.48408483));
    stumpVector.push_back(StumpData( 116,  0.13452687,  0.71831350));
    stumpVector.push_back(StumpData( 237,  0.04734259,  0.48717001));
    stumpVector.push_back(StumpData( 748,  0.01680188, -0.60156293));
    stumpVector.push_back(StumpData( 163,  0.01575929,  0.74481258));
    stumpVector.push_back(StumpData( 408,  0.04073378,  0.40262535));
    stumpVector.push_back(StumpData( 803,  0.48180665,  0.44798398));
    stumpVector.push_back(StumpData( 201,  0.00019369, -0.58368898));
    stumpVector.push_back(StumpData( 394,  0.03977620, -0.48742254));
    stumpVector.push_back(StumpData( 755,  0.25165495,  0.59358820));
    stumpVector.push_back(StumpData( 225,  0.02430439,  0.79991282));
    stumpVector.push_back(StumpData( 182,  0.65448296,  0.39423737));
    stumpVector.push_back(StumpData( 800,  0.14852331, -0.43571100));
    stumpVector.push_back(StumpData( 529,  0.00059121,  0.44826536));
    stumpVector.push_back(StumpData( 205,  0.17103854,  0.86680821));
    stumpVector.push_back(StumpData( 234,  0.06694495, -0.86335195));
    stumpVector.push_back(StumpData( 723,  0.02466554,  0.46536672));
    stumpVector.push_back(StumpData( 695,  0.23143558,  0.74895463));
    stumpVector.push_back(StumpData( 712,  0.75837159, -0.57220286));
    stumpVector.push_back(StumpData(  82,  0.48203585,  0.51144881));
    stumpVector.push_back(StumpData( 146,  0.09659405,  0.40296108));
    stumpVector.push_back(StumpData( 448,  0.01084641,  0.43266379));
    stumpVector.push_back(StumpData( 748,  0.03818647, -0.73656355));
    stumpVector.push_back(StumpData( 237,  0.04525957,  0.51487906));
    stumpVector.push_back(StumpData( 111,  0.08900999,  0.44949710));
    stumpVector.push_back(StumpData( 835,  0.12803405, -0.46223537));
    stumpVector.push_back(StumpData( 784,  0.18080402,  0.42965728));
    stumpVector.push_back(StumpData(  29,  0.22636354, -0.95685792));
    stumpVector.push_back(StumpData( 203,  0.06062277, -0.49623887));
    stumpVector.push_back(StumpData( 940,  0.26958400, -0.45714030));
    stumpVector.push_back(StumpData( 774,  0.01162693, -0.50261635));
    stumpVector.push_back(StumpData( 714,  0.01602963, -0.43208454));
    stumpVector.push_back(StumpData( 205,  0.03941264,  0.87732375));
    stumpVector.push_back(StumpData( 478,  0.03019972, -0.79610385));
    stumpVector.push_back(StumpData( 744,  0.00739740, -0.91698388));
    stumpVector.push_back(StumpData( 746,  0.25920412,  0.94657261));
    stumpVector.push_back(StumpData( 934,  0.25517573,  0.52433723));
    stumpVector.push_back(StumpData( 106,  0.02998070, -0.96829976));
    stumpVector.push_back(StumpData( 357,  0.29060273,  0.83795352));
    stumpVector.push_back(StumpData( 774,  0.01891571, -0.40361276));
    stumpVector.push_back(StumpData( 126,  0.00160811, -0.48744529));
    stumpVector.push_back(StumpData( 784,  0.16683911,  0.51512007));
    stumpVector.push_back(StumpData( 103,  0.01090919, -0.77543436));
    stumpVector.push_back(StumpData( 507,  0.58763683,  0.40428634));
    stumpVector.push_back(StumpData( 931,  0.46665183,  0.80918989));
    stumpVector.push_back(StumpData(  58,  0.52544716, -0.44323188));
    stumpVector.push_back(StumpData( 234,  0.04017276, -0.42709309));

    /*
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
    */
    classifierParam.stumpVector = stumpVector;
    return classifierParam;
}
const ClassifierParam GenderSorterParam::DEFAULT_GENDER_CLASSIFIER_PARAM = 
createDefaultGenderClassifierParam();

const double GenderSorterParam::DEFAULT_MIN_CONFIDENCE = 2.318072;
//const double GenderSorterParam::DEFAULT_MIN_CONFIDENCE = 2.2412; 

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
// motion params updated by Kristin 20140120T095233
const float IdentityTrackerParam::DEFAULT_MEAN_DX = 0.000000;
const float IdentityTrackerParam::DEFAULT_MEAN_DY = 542.500000;
const float IdentityTrackerParam::DEFAULT_MEAN_WIDTH = 6.000000;
const float IdentityTrackerParam::DEFAULT_MEAN_HEIGHT = 6.000000;
const float IdentityTrackerParam::DEFAULT_STD_DX = 3.605022;
const float IdentityTrackerParam::DEFAULT_STD_DY = 8.652053;
const float IdentityTrackerParam::DEFAULT_STD_WIDTH = 5.768036;
const float IdentityTrackerParam::DEFAULT_STD_HEIGHT = 10.094062;
const float IdentityTrackerParam::DEFAULT_MAX_COST = 8.921086;
/*
const float IdentityTrackerParam::DEFAULT_MEAN_DX = -5.5;
const float IdentityTrackerParam::DEFAULT_MEAN_DY = 574.0;
const float IdentityTrackerParam::DEFAULT_MEAN_WIDTH = 2.0;
const float IdentityTrackerParam::DEFAULT_MEAN_HEIGHT = 4.0;
const float IdentityTrackerParam::DEFAULT_STD_DX = 3.6050;
const float IdentityTrackerParam::DEFAULT_STD_DY = 14.4201;
const float IdentityTrackerParam::DEFAULT_STD_WIDTH = 5.7680;
const float IdentityTrackerParam::DEFAULT_STD_HEIGHT = 8.6521;
const float IdentityTrackerParam::DEFAULT_MAX_COST = 32.7148;
*/
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



