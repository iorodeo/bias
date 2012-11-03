#ifdef WITH_DC1394
#ifndef CAMERA_DEVICE_DC1394_HPP 
#define CAMERA_DEVICE_DC1394_HPP

#include "camera_device.hpp"
#include "guid.hpp"
#include "basic_types.hpp"

namespace bias {

    class CameraDevice_dc1394 : public CameraDevice
    {
        public:
            CameraDevice_dc1394() {};
            CameraDevice_dc1394(Guid guid);
            virtual ~CameraDevice_dc1394() {};
            virtual CameraLib getCameraLib();
            virtual void connect() {};
            virtual void printInfo() {};
            virtual void printGuid() {};
            virtual std::string toString();

    };

    typedef std::shared_ptr<CameraDevice_dc1394> CameraDevicePtr_dc1394;

}

#endif
#endif
