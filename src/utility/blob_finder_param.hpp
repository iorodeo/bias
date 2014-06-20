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
            static const double DEFAULT_THRESHOLD_LAX;
            static const double DEFAULT_THRESHOLD_STRICT;
            static const double DEFAULT_THRESHOLD_MAXVAL;
            static const unsigned int DEFAULT_NUM_PAD;
            static const double MINIMUM_AREA_MIN;
            static const double MAXIMUM_AREA_MIN;
            static const double THRESHOLD_MIN;
            static const double THRESHOLD_MAX;
            static const double THRESHOLD_MAXVAL_MIN;
            static const double THRESHOLD_MAXVAL_MAX;
            static const unsigned int NUM_PAD_MIN;
            static const unsigned int NUM_PAD_MAX;

            double minimumArea;
            double maximumArea;
            double thresholdLax;
            double thresholdStrict;
            double thresholdMaxVal;
            unsigned int numPad;

            BlobFinderParam();
            QVariantMap toMap();
            RtnStatus fromMap(QVariantMap paramMap);
            std::string toStdString();
            void print();
    };

}

#endif // #ifndef BLOB_FINDER_PARAMS_HPP
