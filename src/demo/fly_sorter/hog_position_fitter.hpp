#ifndef HOG_POSITION_FITTER
#define HOG_POSITION_FITTER
#include "parameters.hpp"
#include "fly_segmenter.hpp"
#include <list>

class PositionData
{
    public:
        bool isFly;
        static const bool DEFAULT_IS_FLY;
        unsigned int bodyArea;
        static const unsigned int DEFAULT_BODY_AREA;
        PositionData();
};
typedef std::list<PositionData> PositionDataList;


class HogPositionFitterData
{
    public:
        PositionDataList positionDataList;
        HogPositionFitterData();
};


class HogPositionFitter
{
    public:
        HogPositionFitter();
        HogPositionFitter(HogPositionFitterParam param);
        void setParam(HogPositionFitterParam param);
        HogPositionFitterData fit(FlySegmenterData flySegmenterData);

    private:
        bool showDebugWindow_;
        HogPositionFitterParam param_;

};


#endif
