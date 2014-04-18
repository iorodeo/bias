#include "parameters.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include <iostream>
#include <QFile>
#include <QDir>
#include <QTextStream>

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


RtnStatus ClassifierParam::loadFromFile(QString dirName)
{
    RtnStatus rtnStatus;

    // Check that directory exists
    QDir dir(dirName);
    if (!dir.exists())
    {
        rtnStatus.message = QString("Classifier directory, %1, does not exist").arg(dirName);
        rtnStatus.success = false;
        return rtnStatus;
    }

    // Check the file exisits
    std::cout << dir.absoluteFilePath(fileName).toStdString() << std::endl;
    QFile file(dir.absoluteFilePath(fileName));
    if (!file.exists())
    {
        rtnStatus.message = QString("Classifier file, %1, does not exist").arg(fileName);
        rtnStatus.success = false;
        return rtnStatus;
    }

    // Try to open file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        rtnStatus.message = QString("Unable to open classifier file %1").arg(fileName);
        rtnStatus.success = false;
        return rtnStatus;

    }

    // Read data from file
    QTextStream fileStream(&file);
    QStringList lineList;
    while (!fileStream.atEnd())
    {
        QString line = fileStream.readLine();
        lineList.push_back(line);
    }
    file.close();

    // Parse file data
    float offsetTemp;
    std::vector<StumpData> stumpVectorTemp; 
    QString parseErrMsg = QString("Unalbe to parse classifier file %1").arg(fileName);

    if (lineList.size() < 2)
    {
        rtnStatus.message = parseErrMsg;
        rtnStatus.message += QString(" - missing data");
        rtnStatus.success = false;
        return rtnStatus;
    }



    for (int i=0; i<lineList.size(); i++)
    {
        QString line = lineList[i];
        QStringList splitList = line.split(" ", QString::SkipEmptyParts);
        int splitListSize = splitList.size();

        if (i==0)
        {
            // Get classifier offset
            if (!splitListSize == 1)
            {
                rtnStatus.message = parseErrMsg + QString(" - offset is missing");  
                rtnStatus.success = false;
                return rtnStatus;
            }
            else
            {
                bool ok = true;
                offsetTemp = splitList[0].toFloat(&ok);
                if (!ok)
                {
                    rtnStatus.message = parseErrMsg + QString(" - can't convert offset to float");
                    rtnStatus.success = false;
                    return rtnStatus;
                }
                continue;
            }
        }
        else
        {
            // Emptpy line - just keep going
            if (splitListSize == 0)
            {
                continue;
            }

            // Get classifier stumps
            if (splitListSize != 3)
            {
                rtnStatus.message = parseErrMsg; 
                rtnStatus.message += QString(" - more than 3 items in stump");
                rtnStatus.success = false;
                return rtnStatus;
            }
            else
            {
                bool ok;
                StumpData stumpData;

                stumpData.channel = splitList[0].toUInt(&ok); 
                if (!ok)
                {
                    rtnStatus.message = parseErrMsg; 
                    rtnStatus.message += QString(" - unable to convert channel to uint");
                    rtnStatus.success = false;
                    return rtnStatus;
                }


                stumpData.threshold = splitList[1].toFloat(&ok);
                if (!ok)
                {
                    rtnStatus.message = parseErrMsg; 
                    rtnStatus.message += QString(" - unable to convert threshold to float");
                    rtnStatus.success = false;
                    return rtnStatus;
                }

                stumpData.value = splitList[2].toFloat(&ok);
                if (!ok)
                {
                    rtnStatus.message = parseErrMsg;
                    rtnStatus.message += QString(" - unable to convert value to float");
                    rtnStatus.success = false;
                    return rtnStatus;
                }

                stumpVectorTemp.push_back(stumpData);
            }
        }

        if (stumpVectorTemp.size() < 0)
        {
            rtnStatus.message = parseErrMsg;
            rtnStatus.message += QString(" - stump data missige");
            rtnStatus.success = false;
            return rtnStatus;
        }

        offset = offsetTemp;
        stumpVector = stumpVectorTemp;

    }

    rtnStatus.message = QString("");
    rtnStatus.success = true;


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

    classifierParam.stumpVector = stumpVector;
    return classifierParam;
};
const ClassifierParam FlySegmenterParam::DEFAULT_CLASSIFIER_PARAM = 
createDefaultClassifierParam();

const QString FlySegmenterParam::DEFAULT_CLASSIFIER_FILENAME = 
QString("segmentation_classifier.txt");

FlySegmenterParam::FlySegmenterParam()
{
    classifier = DEFAULT_CLASSIFIER_PARAM;
    classifier.fileName = DEFAULT_CLASSIFIER_FILENAME;
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
    modes << QString("camera");    // first is default value
    modes << QString("file");      // from video file
    modes << QString("directory"); // from cropped images in directory
    return modes;
}
const QStringList ImageGrabberParam::ALLOWED_CAPTURE_MODES = getAllowedCaptureModes();
const QString ImageGrabberParam::DEFAULT_CAPTURE_INPUT_FILE = QString("test.avi");
const QString ImageGrabberParam::DEFAULT_CAPTURE_INPUT_DIR = QString("test_images");

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
    captureInputDir = DEFAULT_CAPTURE_INPUT_DIR;
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


    // Get caputre input dir
    // ---------------------
    if (!paramMap.contains("captureInputDir"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'caputreInputDir' not found in image grabber parameters");
        return rtnStatus;
    }
    if (!paramMap["captureInputFile"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert image grabber parameter 'captureInputDir' to string");
        return rtnStatus;
    }
    captureInputDir = paramMap["captureInputDir"].toString();


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



