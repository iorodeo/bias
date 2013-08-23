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
    showDebugWindow_ = true;
    if (showDebugWindow_)
    {
        cv::namedWindow(
                "hogPosLabel",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
        cv::namedWindow(
                "hogPosClose",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
        cv::namedWindow(
                "hogPosIsBody",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
        cv::namedWindow(
                "hogPosMaxComp",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
    }
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
    unsigned int cnt;

    for (it=segmentDataList.begin(), cnt=0; it!=segmentDataList.end(); it++, cnt++)
    {
        SegmentData segmentData = *it;
        std::cout << "  processing segment " << cnt << " data " << std::endl;

        // Detect Body pixels 
        cv::Mat closeMat = imCloseWithDiskElem(
                segmentData.predictorData.label,
                param_.closeRadius
                );

        cv::Mat isBodyMat = bwAreaOpen(closeMat,param_.openArea);
        posData.bodyArea = cv::countNonZero(isBodyMat);

        std::cout << "    bodyArea: " << posData.bodyArea << std::endl;
        std::cout << "    cols:     " << isBodyMat.cols << std::endl;
        std::cout << "    rows:     " << isBodyMat.rows << std::endl;

        //if (posData.bodyArea < param_.openArea)
        //{
        //    // Note, even with bwAreaOpen you can degenerate cases where bodyArea > 0
        //    // but less than openArea. For example when the image is all 255. I'm not 
        //    // worrying about this right now, but you fix this to make is more is line
        //    // with matlab's function. 
        //    posData.isFly = false;
        //    std::cout << "    isFly:    " << posData.isFly << std::endl;
        //    continue;
        //}
        //else
        //{
        //    posData.isFly = true;
        //    std::cout << "    isFly:    " << posData.isFly << std::endl;
        //}

        cv::Mat maxCompMat = findMaxConnectedComponent(isBodyMat);


        if (showDebugWindow_)
        {
            if (cnt==0)
            {
                cv::imshow("hogPosLabel", segmentData.predictorData.label);
                cv::imshow("hogPosClose", closeMat);
                cv::imshow("hogPosIsBody", isBodyMat);
                cv::imshow("hogPosMaxComp", maxCompMat);
            }
        }


    }
    return posData;

}



