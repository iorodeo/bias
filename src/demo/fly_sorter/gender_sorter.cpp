#include "gender_sorter.hpp"
#include <cmath>
#include <iostream>

// GenderData
// ----------------------------------------------------------------------------
GenderData::GenderData()
{
    gender = UNKNOWN;
    havePredictorData = false;
};


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
