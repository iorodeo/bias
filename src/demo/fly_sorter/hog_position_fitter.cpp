#include "hog_position_fitter.hpp"
#include "basic_image_proc.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// PositionData
// ----------------------------------------------------------------------------
const bool PositionData::DEFAULT_IS_FLY = false;
const unsigned int PositionData::DEFAULT_BODY_AREA = 0;

PositionData::PositionData()
{
    isFly = DEFAULT_IS_FLY;
    bodyArea = DEFAULT_BODY_AREA;
}

// HogPositionFitterData
// ----------------------------------------------------------------------------
HogPositionFitterData::HogPositionFitterData() {};


//  HogPositionFitter
//  ---------------------------------------------------------------------------
HogPositionFitter::HogPositionFitter() {};

HogPositionFitter::HogPositionFitter(HogPositionFitterParam param)
{
    setParam(param);
    showDebugWindow_ = false;
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


HogPositionFitterData HogPositionFitter::fit(FlySegmenterData flySegmenterData)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    HogPositionFitterData fitterData;
    SegmentDataList segmentDataList = flySegmenterData.segmentDataList;
    SegmentDataList::iterator it;
    unsigned int cnt;

    for (it=segmentDataList.begin(), cnt=0; it!=segmentDataList.end(); it++, cnt++)
    {
        SegmentData segmentData = *it;
        PositionData posData;
        std::cout << "  processing segment " << cnt << " data " << std::endl;

        // Detect Body pixels 
        cv::Mat closeMat = imCloseWithDiskElem(
                segmentData.predictorData.label,
                param_.closeRadius
                );

        cv::Mat isBodyMat = bwAreaOpen(closeMat,param_.openArea);
        posData.bodyArea = cv::countNonZero(isBodyMat);


        // Ensure that bodyArea is above minimum for fly
        if (posData.bodyArea < param_.openArea)
        {
            // Note,  with the current implementation of bwAreaOpen you can
            // degenerate cases where bodyArea > 0 but less than openArea. For
            // example when the image is all 255. I'm not worrying about this
            // right now, but you fix this to make is more is line with
            // matlab's function. 
            posData.isFly = false;
        }
        else
        {
            // This is a fly - get just the largest connected component
            posData.isFly = true;
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


        fitterData.positionDataList.push_back(posData);

        std::cout << "    bodyArea:       " << posData.bodyArea << std::endl;
        std::cout << "    isBodyMat.cols: " << isBodyMat.cols << std::endl;
        std::cout << "    isBodyMat.rows: " << isBodyMat.rows << std::endl;
        std::cout << "    isFly:          " << posData.isFly << std::endl;
       
    } // for (it=segementDataList.begin() 

    return fitterData;

}



