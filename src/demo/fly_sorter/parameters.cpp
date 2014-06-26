#include "parameters.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include <iostream>
#include <QFile>
#include <QDir>
#include <QTextStream>

const double PixelScaleFactor = 255.0;
const QString CLASSIFIER_DIRECTORY = QString("classifiers");


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
    QString parseErrMsg = QString("Unable to parse classifier file %1").arg(fileName);

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

    } // for (int i

    if (stumpVectorTemp.size() < 0)
    {
        rtnStatus.message = parseErrMsg;
        rtnStatus.message += QString(" - stump data missige");
        rtnStatus.success = false;
        return rtnStatus;
    }

    offset = offsetTemp;
    stumpVector = stumpVectorTemp;

    rtnStatus.message = QString("");
    rtnStatus.success = true;
}


QString ClassifierParam::toString()
{
    QString outString;
    QTextStream outStream(&outString);

    outStream << QString::number(offset) << endl;
    for (int i=0; i<stumpVector.size(); i++)
    {
        outStream << QString::number(stumpVector[i].channel)   << " ";
        outStream << QString::number(stumpVector[i].threshold) << " "; 
        outStream << QString::number(stumpVector[i].value)     <<  endl;
    }
    return outString;
}


std::string ClassifierParam::toStdString()
{
    return toString().toStdString();
}


void ClassifierParam::print()
{
    std::cout << toStdString(); 
}


// FlySegmenterParam
// ----------------------------------------------------------------------------
const QString FlySegmenterParam::DEFAULT_CLASSIFIER_FILENAME = 
QString("segmentation_classifier.txt");

FlySegmenterParam::FlySegmenterParam()
{
    classifier.fileName = DEFAULT_CLASSIFIER_FILENAME;
};


QVariantMap FlySegmenterParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("classifierFile", classifier.fileName);
    return paramMap;
}

RtnStatus FlySegmenterParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;

    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("FlySegmenter parameter map is empty");
        return rtnStatus;
    }

    // Get classifier file name 
    // ------------------------
    if (!paramMap.contains("classifierFile"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'classifierFile' not found in FlySegmenter parameters");
        return rtnStatus;
    }
    if (!paramMap["classifierFile"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert FlySegmenter parameter 'classifierFile' to QString");
        return rtnStatus;
    }
     QString fileName = paramMap["classifierFile"].toString();

    // Check that parameters can be loaded from file
    ClassifierParam classifierTemp;
    classifierTemp.fileName = fileName;
    rtnStatus = classifierTemp.loadFromFile(CLASSIFIER_DIRECTORY);
    if (!rtnStatus.success)
    {
        rtnStatus.message = QString("FlySegmenter: %1").arg(rtnStatus.message);
        return rtnStatus;
    }
    classifier = classifierTemp;

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}



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
const unsigned int PixelFeatureVectorParam::COLOR_EDGE_VECTOR_MIN_SIZE = 2;
const unsigned int PixelFeatureVectorParam::GRAD_MAG_EDGE_VECTOR_MIN_SIZE = 2;
const unsigned int PixelFeatureVectorParam::GRAD_ORI_CENT_VECTOR_MIN_SIZE = 2;
const unsigned int PixelFeatureVectorParam::BIN_PARAM_MIN_SIZE = 2;

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

QVariantMap PixelFeatureVectorParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("gradNormRadius", gradNormRadius);
    paramMap.insert("gradNormConst", gradNormConst);
    paramMap.insert("fillBndryErodeRadius", fillBndryErodeRadius);

    // Convert colorEdgeVector to list of QVariants
    QList<QVariant> colorEdgeList;
    for (int i=0; i<colorEdgeVector.size(); i++)
    {
        QList<QVariant> data;
        for (int j=0; j<3; j++)
        {
            data.push_back(QVariant(colorEdgeVector[i][j]));
        }
        colorEdgeList.push_back(QVariant(data));
    }
    paramMap.insert("colorEdgeVector", colorEdgeList);

    // Convert gradMagEdgeVector to list of QVariants
    QList<QVariant> gradMagEdgeList;
    for (int i=0; i<gradMagEdgeVector.size(); i++)
    {
        gradMagEdgeList.push_back(QVariant(gradMagEdgeVector[i]));
    }
    paramMap.insert("gradMagEdgeVector", gradMagEdgeList);

    // Convert gradOriCentVector to list of QVariants
    QList<QVariant> gradOriCentList;
    for (int i=0; i<gradOriCentVector.size(); i++)
    {
        gradOriCentList.push_back(QVariant(gradOriCentVector[i]));
    }
    paramMap.insert("gradOriCentVector", gradOriCentList);

    // Convert binParam to list of QVariaints
    QList<QVariant> binParamList;
    for (int i=0; i<binParam.size(); i++)
    {
        QList<QVariant> valueList;
        valueList.push_back(QVariant(binParam[i].numX));
        valueList.push_back(QVariant(binParam[i].numY));
        binParamList.push_back(QVariant(valueList));
    }
    paramMap.insert("binParam",binParamList);

    return paramMap;
}

RtnStatus PixelFeatureVectorParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;
    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("pixelFeatureVector parameter map is empty");
        return rtnStatus;
    }

    // Get gradNormRadius
    // ----------------------------
    if (!paramMap.contains("gradNormRadius"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'gradNormRadius' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["gradNormRadius"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter 'gradNormRadius' to unsigned int");
        return rtnStatus;
    }
    gradNormRadius = paramMap["gradNormRadius"].toUInt();


    // Get gradNormConst
    // ----------------------------
    if (!paramMap.contains("gradNormConst"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'gradNormConst' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["gradNormConst"].canConvert<double>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter 'gradNormConst' to double");
        return rtnStatus;
    }
    gradNormConst = paramMap["gradNormConst"].toDouble();

    // Get fillBndryErodeRadius
    // ----------------------------
    if (!paramMap.contains("fillBndryErodeRadius"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'fillBndryErodeRadius' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["fillBndryErodeRadius"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter 'fillBnderyErodeRadius' to double");
        return rtnStatus;
    }
    fillBndryErodeRadius = paramMap["fillBndryErodeRadius"].toUInt();

    // Get colorEdgeVector
    // ----------------------------
    if (!paramMap.contains("colorEdgeVector"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'colorEdgeVector' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["colorEdgeVector"].canConvert<QList<QVariant>>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter colorEdgeVector to QList<QVariant>");
        return rtnStatus;
    }

    QList<QVariant> colorEdgeList = paramMap["colorEdgeVector"].toList();
    if (colorEdgeList.size() < COLOR_EDGE_VECTOR_MIN_SIZE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("pixelFeatureVector -> colorEdgeVector must have at least"); 
        rtnStatus.message += QString("%1 rows").arg(COLOR_EDGE_VECTOR_MIN_SIZE);
        return rtnStatus;
    }
    std::vector<cv::Scalar> colorEdgeVectorTemp(colorEdgeList.size());

    for (int i=0; i<colorEdgeList.size(); i++)
    {
        if (!colorEdgeList[i].canConvert<QList<QVariant>>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert pixelFeatureVector -> colorEdgeVector row to QList<QVariant>");
            return rtnStatus;
        }
        QList<QVariant> rowList = colorEdgeList[i].toList();
        if (rowList.size() != 3)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("each row of pixelFeatureVector -> colorEdgeVector must have 3 elements.");
            return rtnStatus;
        }
        for (int j=0; j<rowList.size();j++)
        {
            if (!rowList[j].canConvert<double>())
            {
                rtnStatus.success = false;
                rtnStatus.message = QString("unable to convert pixelFeatureVector -> colorEdgeVector element to double");
                return rtnStatus;
            }
            colorEdgeVectorTemp[i][j] = rowList[j].toDouble();
        }
    }
    colorEdgeVector = colorEdgeVectorTemp;

    // Get gradMagEdgeVector
    // ----------------------------
    // std::vector<float> gradMagEdgeVector;     
    if (!paramMap.contains("gradMagEdgeVector"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'gradMagEdgeVector' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["gradMagEdgeVector"].canConvert<QList<QVariant>>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter 'gradMagEdgeVector' to QList<QVariant>");
        return rtnStatus;
    }
    QList<QVariant> gradMagEdgeList = paramMap["gradMagEdgeVector"].toList();
    if (gradMagEdgeList.size() < GRAD_MAG_EDGE_VECTOR_MIN_SIZE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("pixelFeatureVector -> gradMagEdgeVector must have at least");
        rtnStatus.message += QString("%1 elements").arg(GRAD_MAG_EDGE_VECTOR_MIN_SIZE);
        return rtnStatus;
    }
    std::vector<float> gradMagEdgeVectorTemp(gradMagEdgeList.size());     
    for (int i=0; i<gradMagEdgeList.size(); i++)
    {
        if (!gradMagEdgeList[i].canConvert<float>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert pixelFeatureVector -> gradMagEdgeVector element to float");
            return rtnStatus;
        }
        gradMagEdgeVectorTemp[i] = gradMagEdgeList[i].toFloat();
    }
    gradMagEdgeVector = gradMagEdgeVectorTemp;


    // Get gradOriCentVector
    // ----------------------------
    if (!paramMap.contains("gradOriCentVector"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'gradOriCentVector' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["gradOriCentVector"].canConvert<QList<QVariant>>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter 'gradOriCentVector' to QList<QVariant>");
        return rtnStatus;
    }
    QList<QVariant> gradOriCentList = paramMap["gradOriCentVector"].toList();
    if (gradOriCentList.size() < GRAD_ORI_CENT_VECTOR_MIN_SIZE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("pixelFeatureVector -> gradOriCentVector must have at least");
        rtnStatus.message += QString("%1 elements").arg(GRAD_ORI_CENT_VECTOR_MIN_SIZE);
        return rtnStatus;
    }
    std::vector<float> gradOriCentVectorTemp(gradOriCentList.size());
    for (int i=0; i<gradOriCentList.size(); i++)
    {
        if (!gradOriCentList[i].canConvert<float>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable to convert pixelFeatureVector -> gradOriCentVector element to float");
            return rtnStatus;
        }
        gradOriCentVectorTemp[i] = gradOriCentList[i].toFloat();
    }
    gradOriCentVector = gradOriCentVectorTemp;


    // Get binParam
    // ----------------------------
    if (!paramMap.contains("binParam"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'binParam' not found in pixelFeatureVector parameters");
        return rtnStatus;
    }
    if (!paramMap["binParam"].canConvert<QList<QVariant>>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert pixelFeatureVector parameter 'binParam' to QList<QVariant>");
        return rtnStatus;
    }
    QList<QVariant> binParamList = paramMap["binParam"].toList();
    if (binParamList.size() < BIN_PARAM_MIN_SIZE)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("pixelFeatureVector -> binParam must have at least");
        rtnStatus.message += QString("%1 rows").arg(BIN_PARAM_MIN_SIZE);
        return rtnStatus;
    }
    std::vector<BinParam> binParamTemp(binParamList.size());
    for (int i=0; i<binParamList.size(); i++)
    {
        if (!binParamList[i].canConvert<QList<QVariant>>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("pixelFeatureVector -> binParam unable to convert row to QList<QVariant>");
            return rtnStatus;
        }
        QList<QVariant> rowList = binParamList[i].toList();
        if (rowList.size() != 2)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("pixelFeatureVector -> binParam rows must have 2 elements");
            return rtnStatus;
        }
        for (int j=0; j<2; j++)
        {
            if (!rowList[j].canConvert<unsigned int>())
            {
                rtnStatus.success = false;
                rtnStatus.message = QString("pixelFeatureVector -> binParam unable to convert element to unsigned int");
                return rtnStatus;
            }
        }
        binParamTemp[i].numX = rowList[0].toUInt();
        binParamTemp[i].numY = rowList[1].toUInt();
    }
    binParam = binParamTemp;

    return rtnStatus;
}

// HogPositionFitterParam
// ----------------------------------------------------------------------------

const unsigned int HogPositionFitterParam::DEFAULT_CLOSE_RADIUS = 15;
const unsigned int HogPositionFitterParam::DEFAULT_OPEN_AREA = 3400;
const unsigned int HogPositionFitterParam::DEFAULT_MAX_BODY_AREA = 22000;
const double HogPositionFitterParam::DEFAULT_PAD_BORDER = 15.0;
const cv::Scalar HogPositionFitterParam::DEFAULT_FILL_VALUES_LUV =
cv::Scalar(0.3703704,0.3259259,0.4962940); 
const QString HogPositionFitterParam::DEFAULT_ORIENT_CLASSIFIER_FILENAME = 
QString("orientation_classifier.txt");

HogPositionFitterParam::HogPositionFitterParam()
{
    closeRadius = DEFAULT_CLOSE_RADIUS;
    openArea = DEFAULT_OPEN_AREA;
    maxBodyArea = DEFAULT_MAX_BODY_AREA;
    padBorder = DEFAULT_PAD_BORDER;
    fillValuesLUV = DEFAULT_FILL_VALUES_LUV;
    orientClassifier.fileName = DEFAULT_ORIENT_CLASSIFIER_FILENAME;
}


QVariantMap HogPositionFitterParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("closeRadius", closeRadius);
    paramMap.insert("openArea", openArea);
    paramMap.insert("maxBodyArea", maxBodyArea);
    paramMap.insert("padBorder", padBorder);
    QList<QVariant> fillValuesList;
    for (int i=0; i<3; i++)
    {
        fillValuesList.push_back(QVariant(fillValuesLUV[i]));
    }
    paramMap.insert("fillValuesLUV", fillValuesList);
    paramMap.insert("orientationClassifierFile", orientClassifier.fileName);
    paramMap.insert("pixelFeatureVector", pixelFeatureVector.toMap());
    return paramMap;
}


RtnStatus HogPositionFitterParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;
    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("hogPositionFitter parameter map is empty");
        return rtnStatus;
    }

    // Get closeRadius
    // ---------------------
    if (!paramMap.contains("closeRadius"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'closeRadius' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["closeRadius"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert  hogPositionFitter parameter 'closeRadius' to unsigned int");
        return rtnStatus;
    }
    closeRadius = paramMap["closeRadius"].toUInt();

    // Get openArea
    // ---------------------
    if (!paramMap.contains("openArea"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'openArea' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["openArea"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert  hogPositionFitter parameter 'closeRadius' to unsigned int");
        return rtnStatus;
    }
    openArea = paramMap["openArea"].toUInt();

    // Get maxBodyArea
    // ---------------------
    if (!paramMap.contains("maxBodyArea"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'maxBodyArea' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["maxBodyArea"].canConvert<unsigned int>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert  hogPositionFitter parameter 'maxBodyArea' to unsigned int");
        return rtnStatus;
    }
    maxBodyArea = paramMap["maxBodyArea"].toUInt();

    // Get padBorder
    // ----------------------
    if (!paramMap.contains("padBorder"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'padBorder' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["padBorder"].canConvert<double>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert  hogPositionFitter parameter 'padBorder' to double");
        return rtnStatus;
    }
    double padBorderTemp = paramMap["padBorder"].toDouble();
    if (padBorderTemp < 0.0)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("hogPositionFitter parameter 'padBorder' must be >= 0");
        return rtnStatus;
    }
    padBorder = padBorderTemp;

    // Get fillValuesLUV
    // ------------------------
    if (!paramMap.contains("fillValuesLUV"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'fillValuesLUV' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["fillValuesLUV"].canConvert<QList<QVariant>>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert  hogPositionFitter parameter 'fillValuesLUV' to list");
        return rtnStatus;
    }
    QList<QVariant> fillValuesList = paramMap["fillValuesLUV"].toList();
    if (fillValuesList.size() != 3)
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("hogPositionFitter parameter 'fillValuesLUV' must have length=3");
        return rtnStatus;
    }
    for (int i=0; i<3; i++)
    {
        QVariant var = fillValuesList.at(i);
        if (!var.canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert element of hogPositionFitter parameter 'fillValuesLUV to double");
            return rtnStatus;
        }
        fillValuesLUV[i] = var.toDouble();
    }

    // Get classifier file name 
    // ------------------------
    if (!paramMap.contains("orientationClassifierFile"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'orientationClassifierFile' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["orientationClassifierFile"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert hogPositionFitter parameter 'orientationClassifierFile' to QString");
        return rtnStatus;
    }
     QString fileName = paramMap["orientationClassifierFile"].toString();

    // Check that parameters can be loaded from file
    ClassifierParam classifierTemp;
    classifierTemp.fileName = fileName;
    rtnStatus = classifierTemp.loadFromFile(CLASSIFIER_DIRECTORY);
    if (!rtnStatus.success)
    {
        rtnStatus.message = QString("hogPositionFitter: %1").arg(rtnStatus.message);
        return rtnStatus;
    }
    orientClassifier = classifierTemp;

    // Get Pixel feature vector
    if (!paramMap.contains("pixelFeatureVector"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'pixelFeatureVector' not found in hogPositionFitter parameters");
        return rtnStatus;
    }
    if (!paramMap["pixelFeatureVector"].canConvert<QVariantMap>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert hogPosition Fitter paramseter 'pixelFeatureVector' to QVariantMap");
        return rtnStatus;
    }
    QVariantMap pixelFeatureVectorParamMap = paramMap["pixelFeatureVector"].toMap();
    rtnStatus = pixelFeatureVector.fromMap(pixelFeatureVectorParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}


// GenderSorterParam
// ----------------------------------------------------------------------------
const QString GenderSorterParam::DEFAULT_GENDER_CLASSIFIER_FILENAME = 
QString("gender_classifier.txt");
const double GenderSorterParam::DEFAULT_MIN_CONFIDENCE = 2.318072;


GenderSorterParam::GenderSorterParam()
{
    minConfidence = DEFAULT_MIN_CONFIDENCE;
    classifier.fileName = DEFAULT_GENDER_CLASSIFIER_FILENAME;
}


RtnStatus GenderSorterParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;
    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("genderSorter parameter map is empty");
        return rtnStatus;
    }

    // Get minConfidence
    // -----------------------
    if (!paramMap.contains("minConfidence"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'minConfidence' not found in genderSorter parameters");
        return rtnStatus;
    }
    if (!paramMap["minConfidence"].canConvert<double>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert genderSorter parameter 'minConfidence' to double");
        return rtnStatus;
    }
    minConfidence = paramMap["minConfidence"].toDouble();


    // Get classifier file name 
    // ------------------------
    if (!paramMap.contains("classifierFile"))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'classifierFile' not found in genderSorter parameters");
        return rtnStatus;
    }
    if (!paramMap["classifierFile"].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert genderSorter parameter 'classifierFile' to QString");
        return rtnStatus;
    }
     QString fileName = paramMap["classifierFile"].toString();

    // Check that parameters can be loaded from file
    ClassifierParam classifierTemp;
    classifierTemp.fileName = fileName;
    rtnStatus = classifierTemp.loadFromFile(CLASSIFIER_DIRECTORY);
    if (!rtnStatus.success)
    {
        rtnStatus.message = QString("genderSorter: %1").arg(rtnStatus.message);
        return rtnStatus;
    }
    classifier = classifierTemp;

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
    return rtnStatus;
}


QVariantMap GenderSorterParam::toMap()
{
    QVariantMap paramMap;
    paramMap.insert("minConfidence", minConfidence);
    paramMap.insert("classifierFile", classifier.fileName);
    return paramMap;
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
    QVariantMap paramMap;
    paramMap.insert("meanDx", meanDx);        
    paramMap.insert("meanDy", meanDy);        
    paramMap.insert("meanWidth", meanWidth);
    paramMap.insert("meanHeight", meanHeight);
    paramMap.insert("stdDx", stdDx);
    paramMap.insert("stdDy", stdDy);
    paramMap.insert("stdWidth", stdWidth);
    paramMap.insert("stdHeight", stdHeight);
    paramMap.insert("maxCost",  maxCost);
    QString motionDirString = motionDirectionToString(motionDirection);
    paramMap.insert("motionDirection", motionDirString);
    return paramMap;
}


RtnStatus IdentityTrackerParam::fromMap(QVariantMap paramMap)
{
    RtnStatus rtnStatus;

    if (paramMap.isEmpty())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("IdentityTracker parameter map is empty");
        return rtnStatus;
    }

    // Get meanDx
    rtnStatus = getFloatFromQVariantMap("meanDx", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get meanDy
    rtnStatus = getFloatFromQVariantMap("meanDy", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get meanWidth
    rtnStatus = getFloatFromQVariantMap("meanWidth", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get meanHeight
    rtnStatus = getFloatFromQVariantMap("meanHeight", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get stdDx
    rtnStatus = getFloatFromQVariantMap("stdDx", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get stdDy
    rtnStatus = getFloatFromQVariantMap("stdDy", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get stdWidth
    rtnStatus = getFloatFromQVariantMap("stdWidth", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get stdHeight
    rtnStatus = getFloatFromQVariantMap("stdHeight", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    //Get maxCost
    rtnStatus = getFloatFromQVariantMap("maxCost", "identityTracker", paramMap, meanDx);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    rtnStatus = getMotionDirectionFromQVariantMap(
            "motionDirection", 
            "identityTracker", 
            paramMap, 
            motionDirection
            );
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
        

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}

// FlySorterParam
// ----------------------------------------------------------------------------

FlySorterParam::FlySorterParam() { }


QVariantMap FlySorterParam::toMap()
{
    QVariantMap paramMap;
    QVariantMap serverParamMap = server.toMap();
    QVariantMap imageGrabberParamMap = imageGrabber.toMap();
    QVariantMap blobFinderParamMap = blobFinder.toMap();
    QVariantMap identityTrackerParamMap = identityTracker.toMap();
    QVariantMap flySegmenterParamMap = flySegmenter.toMap();
    QVariantMap hogPositionFitterParamMap = hogPositionFitter.toMap();
    QVariantMap genderSorterParamMap = genderSorter.toMap();

    paramMap.insert("server", serverParamMap);
    paramMap.insert("imageGrabber", imageGrabberParamMap);
    paramMap.insert("blobFinder", blobFinderParamMap);
    paramMap.insert("identityTracker", identityTrackerParamMap);
    paramMap.insert("flySegmenter", flySegmenterParamMap);
    paramMap.insert("hogPositionFitter", hogPositionFitterParamMap);
    paramMap.insert("genderSorter", genderSorterParamMap);

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

    QVariantMap identityTrackerParamMap = paramMap["identityTracker"].toMap();
    rtnStatus = identityTracker.fromMap(identityTrackerParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    QVariantMap flySegmenterParamMap = paramMap["flySegmenter"].toMap();
    rtnStatus = flySegmenter.fromMap(flySegmenterParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    QVariantMap hogPositionFitterParamMap = paramMap["hogPositionFitter"].toMap();
    rtnStatus = hogPositionFitter.fromMap(hogPositionFitterParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    QVariantMap genderSorterParamMap = paramMap["genderSorter"].toMap();
    rtnStatus = genderSorter.fromMap(genderSorterParamMap);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}


RtnStatus FlySorterParam::fromJson(QByteArray paramJson)
{
    RtnStatus rtnStatus;
    return rtnStatus;
}


// Utility functions
// ----------------------------------------------------------------------------
QString motionDirectionToString(MotionDirection motionDir)
{
    QString motionDirString;
    if (motionDir == MOTION_DIRECTION_Y)
    {
        motionDirString = QString("y");
    }
    else
    {
        motionDirString = QString("x");
    }
    return motionDirString;
}


MotionDirection motionDirectionFromString(QString motionDirString)
{
    MotionDirection motionDir;
    if (QString::compare(motionDirString,QString("y")),Qt::CaseInsensitive)
    {
        motionDir = MOTION_DIRECTION_Y;
    }
    else
    {
        motionDir = MOTION_DIRECTION_X; 
    } 
    return motionDir;
}


RtnStatus getFloatFromQVariantMap(QString key, QString mapName, QVariantMap map, float &value)
{
    RtnStatus rtnStatus;
    if (!map.contains(key))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'%1' field not found in %2 parameters").arg(key).arg(mapName);
        return rtnStatus;
    }
    if (!map[key].canConvert<float>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert %1 parameter '%2' to float").arg(mapName).arg(key);
        return rtnStatus;
    }
    value = map[key].toFloat();
    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}


RtnStatus getStringFromQVariantMap(QString key, QString mapName, QVariantMap map, QString &value)
{
    RtnStatus rtnStatus;
    if (!map.contains(key))
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("'%1' field not found in %2 parameters").arg(key).arg(mapName);
        return rtnStatus;
    }
    if (!map[key].canConvert<QString>())
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unable to convert %1 parameter '%2' to QString").arg(mapName).arg(key);
        return rtnStatus;
    }
    value = map[key].toString();
    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}

RtnStatus getMotionDirectionFromQVariantMap(QString key, QString mapName, QVariantMap map, MotionDirection &value)
{
    RtnStatus rtnStatus;
    QString motionDirString;
    rtnStatus =  getStringFromQVariantMap(key,mapName,map,motionDirString);
    if (!rtnStatus.success)
    {
        return rtnStatus;
    }
    if (motionDirString.compare(QString("x"),Qt::CaseInsensitive) == 0)
    {
        value = MOTION_DIRECTION_X;
    }
    else if (motionDirString.compare(QString("y"),Qt::CaseInsensitive)==0)
    {
        value = MOTION_DIRECTION_Y;
    }
    else
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("Unknown motionDirection in %1- must be 'x' ro 'y'").arg(mapName);
        return rtnStatus;
    }
    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}
