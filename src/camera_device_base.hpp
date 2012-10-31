#ifndef CAMERA_DEVICE_BASE_HPP
#define CAMERA_DEVICE_BASE_HPP

#include <string>

namespace bias {

    // Base class for camera device objects.

    class CameraDevice_base
    {
        public:
            CameraDevice_base() {};
            CameraDevice_base(Guid guid) {guid_ = guid;};
            virtual ~CameraDevice_base() {};
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
