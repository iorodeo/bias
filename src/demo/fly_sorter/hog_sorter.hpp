#ifndef HOG_SORTER_HPP
#define HOG_SORTER_HPP
#include "parameters.hpp"
#include "fast_binary_predictor.hpp"
#include "hog_position_fitter.hpp"
#include <string>
#include <QList>

class SorterData
{
    public:

        enum Classification {TRUE=0, FALSE, UNKNOWN};

        Classification classification;
        PositionData positionData;
        bool havePredictorData;
        FastBinaryPredictorData<double> predictorData;

        SorterData();
        std::string toStdString(unsigned int indent=0);
        void print(unsigned int indent=0);
};
typedef QList<SorterData> SorterDataList;


class HogSorterData
{
    public:
        SorterDataList sorterDataList;
        HogSorterData() {};
        std::string name;
};


class HogSorter 
{
    public:
        HogSorter();
        HogSorter(HogSorterParam param);
        HogSorterData sort(HogPositionFitterData hogData);
        void setParam(HogSorterParam param);
        static std::string ClassificationToString(SorterData::Classification value);
        static std::string ClassificationToLabel(SorterData::Classification value);

    private:
        HogSorterParam param_;

};

#endif 

