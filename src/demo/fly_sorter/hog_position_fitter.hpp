#ifndef HOG_POSITION_FITTER
#define HOG_POSITION_FITTER
#include "parameters.hpp"
#include "fly_segmenter.hpp"

class HogPositionData
{
};

class HogPositionFitter
{
    public:
        HogPositionFitter();
        HogPositionFitter(HogPositionFitterParam param);
        void setParam(HogPositionFitterParam param);
        HogPositionData fit(FlySegmenterData flySegmenterData);

    private:
        HogPositionFitterParam param_;

};

#endif
