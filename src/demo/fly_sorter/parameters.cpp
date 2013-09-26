#include "parameters.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include <iostream>


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

// TEMPORARY - kludgey, but I just need some data for developing 
// the fly segmenter. We will read this from a file later.
// ----------------------------------------------------------------------------
ClassifierParam FlySegmenterParam::createDefaultClassifierParam()
{
    ClassifierParam classifierParam;
    classifierParam.offset = 1.0784;
    std::vector<StumpData> stumpVector;
    stumpVector.push_back(StumpData(1, 0.3104,  1.9041));
    stumpVector.push_back(StumpData(0, 0.2488, -1.6640));
    stumpVector.push_back(StumpData(1, 0.3220,  1.2792));
    stumpVector.push_back(StumpData(0, 0.2755, -1.2068));
    stumpVector.push_back(StumpData(0, 0.2150, -0.8031));
    stumpVector.push_back(StumpData(0, 0.2953, -1.0494));
    stumpVector.push_back(StumpData(0, 0.2488,  0.5447));
    stumpVector.push_back(StumpData(1, 0.3282,  0.7124));
    stumpVector.push_back(StumpData(0, 0.2864, -0.7560));
    stumpVector.push_back(StumpData(1, 0.3104, -0.3242));
    stumpVector.push_back(StumpData(2, 0.5268,  0.3865));
    stumpVector.push_back(StumpData(0, 0.2306, -0.3901));
    stumpVector.push_back(StumpData(1, 0.3104, -0.3375));
    stumpVector.push_back(StumpData(0, 0.1943, -0.9171));
    stumpVector.push_back(StumpData(2, 0.4783, -0.8111));
    stumpVector.push_back(StumpData(1, 0.2925,  0.4803));
    stumpVector.push_back(StumpData(0, 0.2488,  0.2992));
    stumpVector.push_back(StumpData(2, 0.5184,  0.2862));
    stumpVector.push_back(StumpData(1, 0.3220, -0.2660));
    stumpVector.push_back(StumpData(0, 0.2351, -0.2786));
    classifierParam.stumpVector = stumpVector;
    return classifierParam;
};
const ClassifierParam FlySegmenterParam::DEFAULT_CLASSIFIER_PARAM = 
createDefaultClassifierParam();
// ----------------------------------------------------------------------------


FlySegmenterParam::FlySegmenterParam()
{
    classifier = DEFAULT_CLASSIFIER_PARAM;
};


// PixelFeatureVectorParam
// ----------------------------------------------------------------------------
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

std::vector<double> PixelFeatureVectorParam::createDefaultGradMagEdgeVector()
{
    std::vector<double> gradMagEdgeVector;
    gradMagEdgeVector.push_back(0.0 );
    gradMagEdgeVector.push_back(0.0999999493360519);
    gradMagEdgeVector.push_back(0.447888605296612);
    gradMagEdgeVector.push_back(0.717480659484863);
    gradMagEdgeVector.push_back(1.08998268842697);
    gradMagEdgeVector.push_back(100.0);
    return gradMagEdgeVector;
}

const std::vector<double> PixelFeatureVectorParam::DEFAULT_GRAD_MAG_EDGE_VECTOR = 
createDefaultGradMagEdgeVector();

std::vector<double> PixelFeatureVectorParam::createDefaultGradOriEdgeVector()
{
    std::vector<double> gradOriEdgeVector;
    gradOriEdgeVector.push_back(0.523598775598299);
    gradOriEdgeVector.push_back(1.5707963267949);
    gradOriEdgeVector.push_back(2.61799387799149);
    return gradOriEdgeVector;
}

const std::vector<double> PixelFeatureVectorParam::DEFAULT_GRAD_ORI_EDGE_VECTOR = 
createDefaultGradOriEdgeVector();

PixelFeatureVectorParam::PixelFeatureVectorParam()
{

};

// HogPositionFitterParam
// ----------------------------------------------------------------------------

const unsigned int HogPositionFitterParam::DEFAULT_CLOSE_RADIUS = 15;
const unsigned int HogPositionFitterParam::DEFAULT_OPEN_AREA = 3400;
const unsigned int HogPositionFitterParam::DEFAULT_MAX_BODY_AREA = 16600;
const cv::Scalar HogPositionFitterParam::DEFAULT_FILL_VALUES_LUV =
cv::Scalar(0.3703704,0.3259259,0.4962940); 
const double HogPositionFitterParam::DEFAULT_PAD_BORDER = 1.1;

// TEMPORARY - kludgey, but I just need some data for developing 
// the fly segmenter. We will read this from a file later.
// ----------------------------------------------------------------------------
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

const float IdentityTrackerParam::DEFAULT_RADIUS = 40.0;       // pixel
const float IdentityTrackerParam::DEFAULT_VELOCITY_X = 50.0;   // pixel/sec
const float IdentityTrackerParam::DEFAULT_VELOCITY_Y = 0.0;    // pixel/sec


IdentityTrackerParam::IdentityTrackerParam()
{
    radius = DEFAULT_RADIUS;
    velocityX = DEFAULT_VELOCITY_X;
    velocityY = DEFAULT_VELOCITY_Y;
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



