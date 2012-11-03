#ifndef CAMERA_DEVICE_HPP
#define CAMERA_DEVICE_HPP

#include <string>
#include <memory>
#include "basic_types.hpp"
#include "guid.hpp"

namespace bias {

    // Base class for camera device objects.

    class CameraDevice
    {
        public:
            CameraDevice() {};
            CameraDevice(Guid guid) {guid_ = guid;};
            Guid getGuid() {return guid_;};
            virtual ~CameraDevice() {};
            virtual CameraLib getCameraLib() {};
            virtual void connect() {};
            virtual void disconnect() {};
            virtual void printInfo() {};
            virtual void printGuid() {};
            virtual std::string toString() {return std::string("camera not defined");}
            virtual void startCapture() {};
            virtual void stopCapture() {};


        protected:
            Guid guid_;
    };

    typedef std::shared_ptr<CameraDevice> CameraDevicePtr;
}


#endif
