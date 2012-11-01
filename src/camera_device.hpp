#ifndef CAMERA_DEVICE_HPP
#define CAMERA_DEVICE_HPP

#include <string>

namespace bias {

    // Base class for camera device objects.

    class CameraDevice
    {
        public:
            CameraDevice() {};
            CameraDevice(Guid guid) {guid_ = guid;};
            virtual ~CameraDevice() {};
            virtual CameraLib getCameraLib() {};
            virtual void connect() {};
            virtual void printInfo() {};
            virtual void printGuid() {};
            virtual std::string toString() {return std::string("camera not defined");}

        protected:
            Guid guid_;
    };
}


#endif
