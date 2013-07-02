#include "blob_finder_param.hpp"
#include <sstream>
#include <iostream>

namespace bias 
{
    const double BlobFinderParam::DEFAULT_THRESHOLD         = 100.0;
    const double BlobFinderParam::DEFAULT_THRESHOLD_MAXVAL  = 255.0;
    const double BlobFinderParam::DEFAULT_MINIMUM_AREA      = 100.0;
    const double BlobFinderParam::DEFAULT_MAXIMUM_AREA      = 640.0*480.0;
    const double BlobFinderParam::THRESHOLD_MIN             = 0.0;
    const double BlobFinderParam::THRESHOLD_MAX             = 255.0;
    const double BlobFinderParam::THRESHOLD_MAXVAL_MIN      = 255.0;
    const double BlobFinderParam::THRESHOLD_MAXVAL_MAX      = 255.0;
    const double BlobFinderParam::MINIMUM_AREA_MIN          = 1.0;
    const double BlobFinderParam::MAXIMUM_AREA_MIN          = 1.0;
    
    
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


    RtnStatus BlobFinderParam::fromMap(QVariantMap paramMap)
    {
        RtnStatus rtnStatus; 

        if (paramMap.isEmpty())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter map is empty");
            return rtnStatus;
        }

        // Get threshold
        // -------------
        if (!paramMap.contains("threshold"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'threshold' not found in Blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["threshold"].canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'threshold' to double");
            return rtnStatus;
        }
        double thresholdTemp = paramMap["threshold"].toDouble();
        if (thresholdTemp < THRESHOLD_MIN) 
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'threshold' less than minimum"); 
            return rtnStatus;
        }
        if (thresholdTemp > THRESHOLD_MAX)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'threshold' greater than maximum");
            return rtnStatus;
        }
        threshold = thresholdTemp;

        // Get thresholdMaxVal
        // -------------------
        if (!paramMap.contains("thresholdMaxVal"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'thresholdMax' not found in blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["thresholdMaxVal"].canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'thresholdMaxVal' to double");
            return rtnStatus;
        }
        double thresholdMaxValTemp = paramMap["thresholdMaxVal"].toDouble();
        if (thresholdMaxValTemp < THRESHOLD_MAXVAL_MIN) 
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdMaxVal' less than minimum"); 
            return rtnStatus;
        }
        if (thresholdMaxValTemp > THRESHOLD_MAXVAL_MAX)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdMaxVal' greater than maximum");
            return rtnStatus;
        }
        thresholdMaxVal = thresholdMaxValTemp;

        // Get minimumArea
        if (!paramMap.contains("minimumArea"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'minimumArea' not found in blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["minimumArea"].canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'minimumArea' to double");
            return rtnStatus;
        }
        double minimumAreaTemp = paramMap["minimumArea"].toDouble();
        if (minimumAreaTemp < MINIMUM_AREA_MIN)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'minimumArea' less than minimum"); 
            return rtnStatus;
        }
        minimumArea = minimumAreaTemp;

        // Get maximumArea
        // ---------------
        if (!paramMap.contains("maximumArea"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'maximumArea' not found in blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["maximumArea"].canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'maximumArea' to double");
            return rtnStatus;
        }
        double maximumAreaTemp = paramMap["maximumArea"].toDouble();
        if (maximumAreaTemp < MAXIMUM_AREA_MIN)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'maximumArea' less than minimum"); 
            return rtnStatus;
        }
        if (maximumAreaTemp < minimumArea)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'maximumArea' less than 'minimumArea'"); 
            return rtnStatus;
        }
        minimumArea = minimumAreaTemp;
        maximumArea = maximumAreaTemp;

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
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


