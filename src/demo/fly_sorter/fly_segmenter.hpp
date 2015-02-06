#ifndef FLY_SEGMENTER_HPP
#define FLY_SEGMENTER_HPP
#include "parameters.hpp"
#include "blob_finder.hpp"
#include "fast_binary_predictor.hpp"
#include <string>
#include <QList>


class SegmentData 
{
    public:
        FastBinaryPredictorData<cv::Mat> predictorData;
        cv::Mat boundingImageLUV;
        BlobData blobData;

        SegmentData();
        std::string toStdString(unsigned int indent=0);
        void print(unsigned int indent=0);
};
typedef QList<SegmentData> SegmentDataList;


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
        FlySegmenterData segment(BlobFinderData blobFinderData);

    private:

        bool showDebugWindow_;
        FlySegmenterParam param_;
        FastBinaryPredictor fastBinaryPredictor_;
};


#endif // #ifndef FLY_SEGMENTER_HPP
