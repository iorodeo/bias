#ifndef BLOB_FINDER_HPP
#define BLOB_FINDER_HPP
#include "camera_facade_fwd.hpp"
#include "blob_finder_param.hpp"

using namespace bias;

class BlobFinder
{
    public:

        BlobFinder();
        BlobFinder(BlobFinderParam param);

        bool setupCamera();
        bool cleanUp();
        bool run();

    private:

        BlobFinderParam param_;
        CameraPtr cameraPtr_;

        bool startCapture();
        bool stopCapture();
        bool sendData();
};

#endif // #ifndef BLOB_FINDER_HPP


