#include "blob_finder_param.hpp"
#include <sstream>
#include <iostream>

namespace bias 
{
    const double BlobFinderParam::DEFAULT_THRESHOLD         = 100.0;
    const double BlobFinderParam::DEFAULT_THRESHOLD_MAXVAL  = 255.0;
    const double BlobFinderParam::DEFAULT_MINIMUM_AREA      = 100.0;
    const double BlobFinderParam::DEFAULT_MAXIMUM_AREA      = 640.0*480.0;
    
    
    BlobFinderParam::BlobFinderParam()
    {
        threshold = DEFAULT_THRESHOLD;
        thresholdMaxVal = DEFAULT_THRESHOLD_MAXVAL;
        minimumArea = DEFAULT_MINIMUM_AREA;
        maximumArea = DEFAULT_MAXIMUM_AREA;
    }


    QVariantMap BlobFinderParam::toMap()
    {
        QVariantMap paramMap;
        paramMap.insert("threshold", threshold);
        paramMap.insert("thresholdMaxVal", thresholdMaxVal);
        paramMap.insert("minimumArea", minimumArea);
        paramMap.insert("maximumArea", maximumArea);
        return paramMap;
    }
    
    
    std::string BlobFinderParam::toStdString()
    {
        std::stringstream ss;
        ss << "threshold:       " << threshold        << std::endl;
        ss << "thresholdMaxVal: " << thresholdMaxVal  << std::endl;
        ss << "minimumArea:     " << minimumArea      << std::endl;
        ss << "maximumArea:     " << maximumArea      << std::endl;
        return ss.str();
    }
    
    
    void BlobFinderParam::print()
    {
        std::cout << toStdString();
    }

}


