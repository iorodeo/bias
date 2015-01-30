#include "hog_sorter.hpp"
#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>

// SorterData
// ----------------------------------------------------------------------------
SorterData::SorterData()
{
    classification = UNKNOWN;
    havePredictorData = false;
};


std::string SorterData::toStdString(unsigned int indent)
{
    std::stringstream ss;
    std::string indentStr0 = getIndentString(indent);
    std::string indentStr1 = getIndentString(indent+1);
    std::string indentStr2 = getIndentString(indent+2);

    std::string classificationStr = HogSorter::ClassificationToString(classification);
    ss << indentStr0 << "SorterData: " << std::endl;
    ss << indentStr1 << "classification: " << classificationStr << std::endl; 
    ss << indentStr1 << "havePredictorData: " << havePredictorData << std::endl;
    ss << indentStr1 << "predictorData: "; 
    if (havePredictorData)
    {
        ss << std::endl;
        ss << indentStr2 << "label: " << predictorData.label << std::endl;
        ss << indentStr2 << "fit: " << predictorData.fit << std::endl;
    }
    else
    {
        ss << " none" << std::endl;
    }
    ss << positionData.toStdString(indent+1);
    return ss.str();
}


void SorterData::print(unsigned int indent)
{
    std::cout << toStdString(indent);
}


// HogSorter
// ----------------------------------------------------------------------------
HogSorter::HogSorter() {};


HogSorter::HogSorter(HogSorterParam param) : HogSorter()
{
    setParam(param);
}


void HogSorter::setParam(HogSorterParam param)
{
    param_ = param;
}

HogSorterData HogSorter::sort(HogPositionFitterData hogData)
{
    HogSorterData hogSorterData;
    PositionDataList::iterator it;
    int cnt = 0;
    for (it=hogData.positionDataList.begin(); it!=hogData.positionDataList.end(); it++)
    {
        SorterData sorterData;
        sorterData.classification = UNKNOWN;
        sorterData.havePredictorData = false;
        sorterData.positionData = *it;

        if (sorterData.positionData.success)
        {
            FastBinaryPredictor predictor = FastBinaryPredictor(param_.classifier);
            sorterData.predictorData = predictor.predict(sorterData.positionData.pixelFeatureVector);
            sorterData.havePredictorData = true;
            if (sorterData.predictorData.fit >= param_.minConfidence)
            {
                sorterData.classification = TRUE;
            }
            if (sorterData.predictorData.fit <= -param_.minConfidence)
            {
                sorterData.classification = FALSE;
            }
            cnt++;
        }
        hogSorterData.sorterDataList.push_back(sorterData);
    }
    return hogSorterData;
}

std::string HogSorter::ClassificationToString(Classification classification)
{
    std::string classificationString;

    switch (classification)
    {
        case TRUE:
            classificationString = std::string("true");
            break;

        case FALSE:
            classificationString = std::string("false");
            break;

        default:
            classificationString = std::string("unknown");
            break;

    }
    return classificationString;
}


std::string HogSorter::ClassificationToLabel(Classification classification)
{
    std::string labelString;
    switch (classification)
    {

    }
    return labelString;
}
