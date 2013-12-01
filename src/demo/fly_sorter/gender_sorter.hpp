#ifndef GENDER_SORTER_HPP
#define GENDER_SORTER_HPP
#include "parameters.hpp"
#include "fast_binary_predictor.hpp"
#include "hog_position_fitter.hpp"
#include <list>

enum Gender 
{
    MALE = 0,
    FEMALE,
    UNKNOWN, 
};
    

class GenderData
{
    public:
        Gender gender;
        PositionData positionData;
        FastBinaryPredictorData<double> predictorData;
        GenderData();
};
typedef std::list<GenderData> GenderDataList;


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

    private:
        GenderSorterParam param_;

};


#endif
