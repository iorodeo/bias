#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <memory>
#include <iostream>
#include "guid.hpp"
#include "basic_types.hpp"
#include "camera_device.hpp"

namespace bias {

    class Camera {

        // --------------------------------------------------------------------
        // Facade wrapper for cameras. Provides a unified interface for cameras 
        // which are accessed via different underlying libraries e.g. libdc1394, 
        // flycapture2, etc.
        // --------------------------------------------------------------------
       
        friend std::ostream& operator<< (std::ostream &out, Camera &camera);

        public:
            Camera();
            Camera(Guid guid);
            ~Camera();
            void connect();
            void printInfo();
            void printGuid();

        private:
            std::shared_ptr<CameraDevice> cameraDevicePtr_;
            void createCameraDevice_fc2(Guid guid);
            void createCameraDevice_dc1394(Guid guid);

    };
}


#endif
