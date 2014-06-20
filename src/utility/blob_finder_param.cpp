#include "blob_finder_param.hpp"
#include <sstream>
#include <iostream>

namespace bias 
{
    // OLD
    const double BlobFinderParam::DEFAULT_THRESHOLD_LAX     = 100.0;
    const double BlobFinderParam::DEFAULT_THRESHOLD_STRICT  = 80.0;
    const double BlobFinderParam::DEFAULT_THRESHOLD_MAXVAL  = 255.0;
    const double BlobFinderParam::DEFAULT_MINIMUM_AREA      = 100.0;
    const double BlobFinderParam::DEFAULT_MAXIMUM_AREA      = 640.0*480.0;
    const unsigned int BlobFinderParam::DEFAULT_NUM_PAD     = 50;
    const double BlobFinderParam::THRESHOLD_MIN             = 0.0;
    const double BlobFinderParam::THRESHOLD_MAX             = 255.0;
    const double BlobFinderParam::THRESHOLD_MAXVAL_MIN      = 255.0;
    const double BlobFinderParam::THRESHOLD_MAXVAL_MAX      = 255.0;
    const double BlobFinderParam::MINIMUM_AREA_MIN          = 1.0;
    const double BlobFinderParam::MAXIMUM_AREA_MIN          = 1.0;
    const unsigned int BlobFinderParam::NUM_PAD_MIN         = 1; 
    const unsigned int BlobFinderParam::NUM_PAD_MAX         = 200;
    
    
    BlobFinderParam::BlobFinderParam()
    {
        thresholdLax = DEFAULT_THRESHOLD_LAX;
        thresholdStrict = DEFAULT_THRESHOLD_STRICT;
        thresholdMaxVal = DEFAULT_THRESHOLD_MAXVAL;
        minimumArea = DEFAULT_MINIMUM_AREA;
        maximumArea = DEFAULT_MAXIMUM_AREA;
        numPad = DEFAULT_NUM_PAD;
    }


    QVariantMap BlobFinderParam::toMap()
    {
        QVariantMap paramMap;
        paramMap.insert("thresholdLax", thresholdLax);
        paramMap.insert("thresholdStrict", thresholdStrict);
        paramMap.insert("thresholdMaxVal", thresholdMaxVal);
        paramMap.insert("minimumArea", minimumArea);
        paramMap.insert("maximumArea", maximumArea);
        paramMap.insert("numPad", numPad);

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

        // Get thresholdStrict
        if (!paramMap.contains("thresholdStrict"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'thresholdStrict' not found in Blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["thresholdStrict"].canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'thresholdStrict' to double");
            return rtnStatus;
        }
        double thresholdStrictTemp = paramMap["thresholdStrict"].toDouble();
        if (thresholdStrictTemp < THRESHOLD_MIN) 
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdStrict' less than minimum"); 
            return rtnStatus;
        }
        if (thresholdStrictTemp > THRESHOLD_MAX)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdStrict' greater than maximum");
            return rtnStatus;
        }
        thresholdStrict = thresholdStrictTemp;

        // Get thresholdLaxUpper
        if (!paramMap.contains("thresholdLax"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'thresholdLax' not found in Blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["thresholdLax"].canConvert<double>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'thresholdLax' to double");
            return rtnStatus;
        }
        double thresholdLaxTemp = paramMap["thresholdLax"].toDouble();
        if (thresholdLaxTemp < THRESHOLD_MIN) 
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdLax' less than minimum"); 
            return rtnStatus;
        }
        if (thresholdLaxTemp > THRESHOLD_MAX)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdLax' greater than maximum");
            return rtnStatus;
        }
        if (thresholdLaxTemp < thresholdStrict)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'thresholdLax' less than 'thresholdStrict'");
            return rtnStatus;
        }
        thresholdLax = thresholdLaxTemp;


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

        // Get numPad
        // -------------
        if (!paramMap.contains("numPad"))
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("'numPad' not found in blob finder parameters");
            return rtnStatus;
        }
        if (!paramMap["numPad"].canConvert<unsigned int>())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Unable to convert blob finder parameter 'numPad' to unsigned int");
            return rtnStatus;
        }
        unsigned int numPadTemp = paramMap["numPad"].toUInt();
        if (numPadTemp < NUM_PAD_MIN) 
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'numPad' less than minimum allowed"); 
            return rtnStatus;
        }
        if (numPadTemp > NUM_PAD_MAX)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("Blob finder parameter 'numPad' greater than maximum allowed"); 
            return rtnStatus;
        }
        numPad = numPadTemp;

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }
    
    
    std::string BlobFinderParam::toStdString()
    {
        std::stringstream ss;
        ss << "thresholdLax:    " << thresholdLax     << std::endl;
        ss << "thresholdStrict: " << thresholdStrict  << std::endl;
        ss << "thresholdMaxVal: " << thresholdMaxVal  << std::endl;
        ss << "minimumArea:     " << minimumArea      << std::endl;
        ss << "maximumArea:     " << maximumArea      << std::endl;
        ss << "numPad:          " << numPad           << std::endl;
        return ss.str();
    }
    
    
    void BlobFinderParam::print()
    {
        std::cout << toStdString();
    }

}


