#ifndef BLOB_FINDER_HPP
#define BLOB_FINDER_HPP
#include "camera_facade_fwd.hpp"

using namespace bias;

class BlobFinder
{
    public:

        static const float DEFAULT_FRAMERATE;
        static const float DEFAULT_DISPLAY_SCALE;
        static const double DEFAULT_THRESHOLD;
        static const double DEFAULT_MINIMUM_AREA;
        static const double DEFAULT_MAXIMUM_AREA;
        static const double THRESHOLD_MAXVAL;

        BlobFinder();
        BlobFinder(float frameRate);

        bool setupCamera();
        bool cleanUp();
        bool run();

    private:

        float frameRate_;
        float displayScale_;
        double threshold_;
        double minimumArea_; 
        double maximumArea_;
        CameraPtr cameraPtr_;

        bool startCapture();
        bool stopCapture();

};

#endif // #ifndef BLOB_FINDER_HPP


