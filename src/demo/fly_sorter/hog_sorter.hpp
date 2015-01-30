#ifndef HOG_SORTER_HPP
#define HOG_SORTER_HPP
#include "parameters.hpp"
#include "fast_binary_predictor.hpp"
#include "hog_position_fitter.hpp"
#include <list>
#include <string>

enum Classification 
{
    TRUE = 0,
    FALSE,
    UNKNOWN
};

class SorterData
{
    public:
        Classification classification;

        PositionData positionData;
        bool havePredictorData;
        FastBinaryPredictorData<double> predictorData;

        SorterData();
        std::string toStdString(unsigned int indent=0);
        void print(unsigned int indent=0);
};
typedef std::list<SorterData> SorterDataList;


class HogSorterData
{
    public:
        SorterDataList sorterDataList;
        HogSorterData() {};
};


class HogSorter 
{
    public:
        HogSorter();
        HogSorter(HogSorterParam param);
        HogSorterData sort(HogPositionFitterData hogData);
        void setParam(HogSorterParam param);
        static std::string ClassificationToString(Classification value);
        static std::string ClassificationToLabel(Classification value);

    private:
        HogSorterParam param_;

};

#endif 

