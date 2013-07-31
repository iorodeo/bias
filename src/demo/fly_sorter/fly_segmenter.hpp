#ifndef FLY_SEGMENTER_HPP
#define FLY_SEGMENTER_HPP
#include "parameters.hpp"
#include "blob_finder.hpp"
#include "fast_binary_predictor.hpp"


class FlySegmenterData : public FastBinaryPredictorData
{
    public:
        FlySegmenterData();
        void setPredictorData(FastBinaryPredictorData predictorData);
};


class FlySegmenter
{
    public:

        FlySegmenter();
        FlySegmenter(FlySegmenterParam param);
        void setParam(FlySegmenterParam param);
        FlySegmenterData segment(BlobDataList blobDataList);

    private:

        FlySegmenterParam param_;
        FastBinaryPredictor fastBinaryPredictor_;

};



#endif // #ifndef FLY_SEGMENTER_HPP
