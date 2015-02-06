#ifndef GENDER_SORTER_HPP
#define GENDER_SORTER_HPP
#include "parameters.hpp"
#include "fast_binary_predictor.hpp"
#include "hog_position_fitter.hpp"
#include <string>
#include <QList>


class GenderData
{
    public:
        enum Gender {MALE=0, FEMALE, UNKNOWN};
        Gender gender;
        PositionData positionData;
        bool havePredictorData;
        FastBinaryPredictorData<double> predictorData;
        GenderData();
        std::string toStdString(unsigned int indent=0);
        void print(unsigned int indent=0);
};
typedef QList<GenderData> GenderDataList;


class GenderSorterData
{
    public:
        GenderDataList genderDataList;
        GenderSorterData() {};
};


class GenderSorter 
{
    public:
        GenderSorter();
        GenderSorter(GenderSorterParam param);
        GenderSorterData sort(HogPositionFitterData hogData);
        void setParam(GenderSorterParam param);
        static std::string GenderToString(GenderData::Gender gender);
        static std::string GenderToLetter(GenderData::Gender gender);

    private:
        GenderSorterParam param_;

};


#endif
