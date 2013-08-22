#include "hog_position_fitter.hpp"
#include "basic_image_proc.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// HogPositionData
// ----------------------------------------------------------------------------
const bool HogPositionData::DEFAULT_IS_FLY = false;
const unsigned int HogPositionData::DEFAULT_BODY_AREA = 0;

HogPositionData::HogPositionData()
{
    isFly = DEFAULT_IS_FLY;
    bodyArea = DEFAULT_BODY_AREA;
}


//  HogPositionFitter
//  ---------------------------------------------------------------------------
HogPositionFitter::HogPositionFitter() {};

HogPositionFitter::HogPositionFitter(HogPositionFitterParam param)
{
    setParam(param);
};

void HogPositionFitter::setParam(HogPositionFitterParam param)
{
    param_ = param;
}


HogPositionData HogPositionFitter::fit(FlySegmenterData flySegmenterData)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    HogPositionData posData;
    SegmentDataList segmentDataList = flySegmenterData.segmentDataList;
    SegmentDataList::iterator it;

    for (it=segmentDataList.begin(); it!=segmentDataList.end(); it++)
    {
        SegmentData segmentData = *it;
        std::cout << "  processing segment data (hello)" << std::endl;

        // Detect Body pixels 
        cv::Mat closeMat = imCloseWithDiskElem(
                segmentData.predictorData.label,
                param_.closeRadius
                );
        cv::Mat openCloseMat = bwAreaOpen(closeMat,param_.openArea);
        posData.bodyArea = cv::countNonZero(openCloseMat);
        if (posData.bodyArea == 0)
        {
            posData.isFly = false;
           return posData; 
        }
        else
        {
            posData.isFly = true;
        }







    }
    return posData;

}



