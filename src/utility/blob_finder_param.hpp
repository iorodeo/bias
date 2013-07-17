#ifndef BLOB_FINDER_PARAM_HPP
#define BLOB_FINDER_PARAM_HPP

#include <string>
#include <QVariantMap>
#include "rtn_status.hpp"

namespace bias
{

    class BlobFinderParam
    {
        public:
    
            static const double DEFAULT_MINIMUM_AREA;
            static const double DEFAULT_MAXIMUM_AREA;

            static const double DEFAULT_THRESHOLD_MAXVAL;
            static const double DEFAULT_THRESHOLD_UPPER;
            static const double DEFAULT_THRESHOLD_LOWER;
            
            // OLD 
            // --------------------------------------- 
            static const double DEFAULT_THRESHOLD;
            // ---------------------------------------

            static const double MINIMUM_AREA_MIN;
            static const double MAXIMUM_AREA_MIN;
            static const double THRESHOLD_MIN;
            static const double THRESHOLD_MAX;
            static const double THRESHOLD_MAXVAL_MIN;
            static const double THRESHOLD_MAXVAL_MAX;

            double minimumArea;
            double maximumArea;
            double thresholdUpper;
            double thresholdLower;
            double thresholdMaxVal;

            // OLD
            // ---------------------------------------
            double threshold;
            // ---------------------------------------
    
            BlobFinderParam();
            QVariantMap toMap();
            RtnStatus fromMap(QVariantMap paramMap);
            std::string toStdString();
            void print();
    };

}

#endif // #ifndef BLOB_FINDER_PARAMS_HPP
