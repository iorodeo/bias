#include "gender_sorter.hpp"
#include <cmath>
#include <sstream>
#include <iostream>

// GenderData
// ----------------------------------------------------------------------------
GenderData::GenderData()
{
    gender = UNKNOWN;
    havePredictorData = false;
};


std::string GenderData::toStdString(unsigned int indent)
{
    std::stringstream ss;
    std::string indentStr0 = getIndentString(indent);
    std::string indentStr1 = getIndentString(indent+1);
    std::string indentStr2 = getIndentString(indent+2);
    std::string genderStr = GenderSorter::GenderToString(gender);
    ss << indentStr0 << "GenderData: " << std::endl;
    ss << indentStr1 << "gender: " << genderStr << std::endl; 
    ss << indentStr1 << "havePredictorData: " << havePredictorData << std::endl;
    ss << indentStr1 << "predictorData: " << std::endl;
    if (havePredictorData)
    {
        ss << indentStr2 << "labal: " << predictorData.label << std::endl;
        ss << indentStr2 << "fit: " << predictorData.fit << std::endl;
    }
    ss << positionData.toStdString(indent+1);
    return ss.str();
}


void GenderData::print(unsigned int indent)
{
    std::cout << toStdString(indent);
}


// GenderSorter
// ----------------------------------------------------------------------------
GenderSorter::GenderSorter() {};


GenderSorter::GenderSorter(GenderSorterParam param) : GenderSorter()
{
    setParam(param);
}


void GenderSorter::setParam(GenderSorterParam param)
{
    param_ = param;
}

GenderSorterData GenderSorter::sort(HogPositionFitterData hogData)
{
    GenderSorterData sorterData;
    PositionDataList::iterator it;
    for (it=hogData.positionDataList.begin(); it!=hogData.positionDataList.end(); it++)
    {
        GenderData genderData;
        genderData.gender = UNKNOWN;
        genderData.havePredictorData = false;

        genderData.positionData = *it;
        if (genderData.positionData.success)
        {
            FastBinaryPredictor genderPred = FastBinaryPredictor(param_.genderClassifier);
            genderData.predictorData = genderPred.predict(genderData.positionData.pixelFeatureVector);
            genderData.havePredictorData = true;
            if (genderData.predictorData.fit >= param_.minConfidence)
            {
                genderData.gender = FEMALE;
            }
            if (genderData.predictorData.fit <= -param_.minConfidence)
            {
                genderData.gender = MALE;
            }
        }
        sorterData.genderDataList.push_back(genderData);
    }
    return sorterData;
}

std::string GenderSorter::GenderToString(Gender gender)
{
    std::string genderString;

    switch (gender)
    {
        case FEMALE:
            genderString = std::string("female");
            break;

        case MALE:
            genderString = std::string("male");
            break;

        default:
            genderString = std::string("unknown");
            break;

    }
    return genderString;
}
