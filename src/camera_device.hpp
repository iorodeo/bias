#ifndef BIAS_CAMERA_DEVICE_HPP
#define BIAS_CAMERA_DEVICE_HPP

#include <string>
#include <memory>
#include "basic_types.hpp"
#include "guid.hpp"

namespace bias {

    // Base class for camera device objects.

    class CameraDevice
    {
        public:
            CameraDevice() 
            {
                connected_ = false;
                capturing_ = false;
            };
            explicit CameraDevice(Guid guid) {guid_ = guid;};
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
            virtual void grabImage() {};


        protected:
            Guid guid_;
            bool connected_;
            bool capturing_;
    };

    typedef std::shared_ptr<CameraDevice> CameraDevicePtr;
}


#endif // #ifndef BIAS_CAMERA_DEVICE_HPP
