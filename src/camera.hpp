#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <memory>
#include "guid.hpp"
#include "basic_types.hpp"
#include "camera_device_base.hpp"
//#ifdef WITH_FC2
//#include "camera_device_fc2.hpp"
//#endif
//#ifdef WITH_DC1394
//#include "camera_device_dc1394.hpp"
//#endif

using namespace std;

namespace bias {

    class Camera {

        // --------------------------------------------------------------------
        // Facade wrapper for cameras. Provices a unified interface for cameras 
        // which are accessed via different underlying libraries e.g. libdc1394, 
        // flycapture2, etc.
        // --------------------------------------------------------------------

        public:
            Camera();
            Camera(Guid guid);
            ~Camera();
            void connect();
            void printInfo();
            void printGuid();

        private:
            //CameraDevice_base *cameraDevicePtr_;
            shared_ptr<CameraDevice_base> cameraDevicePtr_;
            void createCameraDevice_fc2(Guid guid);
            void createCameraDevice_dc1394(Guid guid);
//#ifdef WITH_FC2
//        private:
//            CameraDevice_fc2 cameraDevice_fc2_;
//#endif
//
//#ifdef WITH_DC1394
//        private:
//            CameraDevice_dc1394 cameraDevice_dc1394_;
//#endif
    };
}


#endif
