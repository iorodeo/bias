#ifndef FLY_SEGMENTER_HPP
#define FLY_SEGMENTER_HPP
#include "parameters.hpp"
#include "blob_finder.hpp"
#include "fast_binary_predictor.hpp"
#include <list>


class SegmentData 
{
    public:
        SegmentData() {};
        FastBinaryPredictorData predictorData;
        cv::Mat boundingImageLUV;
        BlobData blobData;
};
typedef std::list<SegmentData> SegmentDataList;


class FlySegmenterData
{
    public:
        FlySegmenterData() {};
        SegmentDataList segmentDataList;
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
