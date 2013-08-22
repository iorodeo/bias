#ifndef HOG_POSITION_FITTER
#define HOG_POSITION_FITTER
#include "parameters.hpp"
#include "fly_segmenter.hpp"

class HogPositionData
{
    public:

        bool isFly;
        static const bool DEFAULT_IS_FLY;

        unsigned int bodyArea;
        static const unsigned int DEFAULT_BODY_AREA;

        HogPositionData();
};

class HogPositionFitter
{
    public:
        HogPositionFitter();
        HogPositionFitter(HogPositionFitterParam param);
        void setParam(HogPositionFitterParam param);
        HogPositionData fit(FlySegmenterData flySegmenterData);

    private:
        bool showDebugWindow_;
        HogPositionFitterParam param_;

};


#endif
