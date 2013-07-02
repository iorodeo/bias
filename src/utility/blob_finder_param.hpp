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
    
            static const double DEFAULT_THRESHOLD;
            static const double DEFAULT_MINIMUM_AREA;
            static const double DEFAULT_MAXIMUM_AREA;
            static const double DEFAULT_THRESHOLD_MAXVAL;

            double minimumArea;
            double maximumArea;
            double threshold;
            double thresholdMaxVal;
    
            BlobFinderParam();
            QVariantMap toMap();
            RtnStatus fromMap(QVariantMap paramMap);
            std::string toStdString();
            void print();
    };

}

#endif // #ifndef BLOB_FINDER_PARAMS_HPP
