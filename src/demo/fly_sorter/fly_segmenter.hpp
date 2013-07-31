#ifndef FLY_SEGMENTER_HPP
#define FLY_SEGMENTER_HPP
#include "parameters.hpp"
#include "blob_finder.hpp"

class FlySegmenterData
{};

class FlySegmenter
{
    public:

        FlySegmenter();
        FlySegmenter(FlySegmenterParam param);
        void setParam(FlySegmenterParam param);
        FlySegmenterData segment(BlobDataList blobDataList);

    private:

        FlySegmenterParam param_;

};



#endif // #ifndef FLY_SEGMENTER_HPP
