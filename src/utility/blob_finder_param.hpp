#ifndef BLOB_FINDER_PARAM_HPP
#define BLOB_FINDER_PARAM_HPP

#include <string>

class BlobFinderParam
{
    public:

        static const float  DEFAULT_FRAMERATE;
        static const float  DEFAULT_DISPLAY_SCALE;
        static const double DEFAULT_THRESHOLD;
        static const double DEFAULT_MINIMUM_AREA;
        static const double DEFAULT_MAXIMUM_AREA;
        static const double DEFAULT_THRESHOLD_MAXVAL;
        float frameRate;
        float displayScale;
        double minimumArea;
        double maximumArea;
        double threshold;
        double thresholdMaxVal;
        BlobFinderParam();
        std::string toStdString();
        void print();
};

#endif // #ifndef BLOB_FINDER_PARAMS_HPP
