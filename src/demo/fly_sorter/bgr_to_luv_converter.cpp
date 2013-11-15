#include "bgr_to_luv_converter.hpp"
#include <cmath>
#include <cassert>
#include <iostream>

// Constants
// ----------------------------------------------------------------------------

const int BgrToLuvConverter::LOOKUP_TABLE_SIZE = 1025;

std::vector<float> BgrToLuvConverter::createLookupTable()
{
    std::vector<float> table;
    const float y0 = ((6.0/29)*(6.0/29)*(6.0/29));
    const float a = ((29.0/3)*(29.0/3)*(29.0/3));
    for (int i=0; i<LOOKUP_TABLE_SIZE; i++)
    {
        float y = float(i)/float(LOOKUP_TABLE_SIZE);
        float value = y > y0 ? 116.0*pow(y,1.0/3.0)-16.0 : y*a;
        value *= (1.0/270.0);
        table.push_back(value);
    }
    return table;
}
const std::vector<float> BgrToLuvConverter::LOOKUP_TABLE = 
BgrToLuvConverter::createLookupTable();

const cv::Matx33f BgrToLuvConverter::BGR_TO_LUV_MATRIX_FLT = cv::Matx33f( 
        0.178325, 0.341550, 0.430574,
        0.071330, 0.706655, 0.222015,
        0.939180, 0.129553, 0.020183 
        );

const cv::Matx33f BgrToLuvConverter::BGR_TO_LUV_MATRIX_8U = 
(1.0/255.0)*BgrToLuvConverter::BGR_TO_LUV_MATRIX_FLT;


// Methods
// ----------------------------------------------------------------------------

cv::Mat BgrToLuvConverter::convert(cv::Mat bgrImg)
{
    const float minu = -88.0/270.0; 
    const float minv = -134.0/270.0;
    const float un = 0.197833; 
    const float vn = 0.468331;
    const int bgrImgDepth = bgrImg.depth();
    cv::Mat luvImg = cv::Mat(bgrImg.size(),CV_32FC3,cv::Scalar(0.0,0.0,0.0));
    assert(
            (bgrImg.type()==CV_8UC3)  ||
            (bgrImg.type()==CV_32FC3) ||
            (bgrImg.type()==CV_64FC3)
            );

    cv::Matx33f convMat;
    if (bgrImg.type() == CV_8UC3)
    {
        convMat = BGR_TO_LUV_MATRIX_8U;
    }
    else
    {
        convMat = BGR_TO_LUV_MATRIX_FLT;
    }

    for (int i=0; i<bgrImg.rows; i++)
    {
        for (int j=0; j<bgrImg.cols; j++)
        {

            // Compute x,y,z values from bgr values
            cv::Vec3f bgrElem;
            switch (bgrImgDepth)
            {
                case CV_8U:
                    bgrElem = cv::Vec3f(bgrImg.at<cv::Vec3b>(i,j)); 
                    break;

                case CV_64F:
                    bgrElem = cv::Vec3f(bgrImg.at<cv::Vec3d>(i,j)); 
                    break;

                default:
                    bgrElem = bgrImg.at<cv::Vec3f>(i,j);
                    break;

            }  

            cv::Vec3f xyzElem = cv::Vec3f(convMat*bgrElem);
            float x = xyzElem[0];
            float y = xyzElem[1];
            float z = xyzElem[2];

            // Get "l" value from y using lookup table
            int lInd = int(y*(LOOKUP_TABLE_SIZE-1));
            float lVal;
            if (lInd < LOOKUP_TABLE_SIZE)
            { 
                lVal = LOOKUP_TABLE[lInd];
            }
            else
            {
                lVal = LOOKUP_TABLE[LOOKUP_TABLE_SIZE-1];
            }

            // Calculate u and v values
            float temp = 1.0/(x + 15.0*y + 3.0*z + 1.0e-35);
            float uVal = lVal*(13.0*4.0*x*temp - 13.0*un) - minu;
            float vVal = lVal*(13.0*9.0*y*temp - 13.0*vn) - minv;

            // Assign to matrix
            cv::Vec3f luvElem = cv::Vec3f(lVal,uVal,vVal);
            luvImg.at<cv::Vec3f>(i,j) = luvElem;

        } // for j

    } // for i

    return luvImg;
}
