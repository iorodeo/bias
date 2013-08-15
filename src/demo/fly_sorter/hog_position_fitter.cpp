#include "hog_position_fitter.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

    SegmentDataList segmentDataList = flySegmenterData.segmentDataList;
    SegmentDataList::iterator it;

    for (it=segmentDataList.begin(); it!=segmentDataList.end(); it++)
    {
        SegmentData segmentData = *it;
        std::cout << "  processing segment data" << std::endl;
        
        // Perform morphological close and open operations
        unsigned int elemDiam = 2*param_.closeRadius + 1;
        cv::Size elemSize = cv::Size(elemDiam,elemDiam);
        cv::Mat closeStructElem = cv::getStructuringElement(cv::MORPH_ELLIPSE, elemSize);
        cv::Mat fitMat = segmentData.predictorData.fit;
        cv::Mat closeMat = cv::Mat(fitMat.size(),fitMat.type());
        cv::morphologyEx(fitMat,closeMat,cv::MORPH_CLOSE,closeStructElem);

        // Apply bwarea open
    }

}



