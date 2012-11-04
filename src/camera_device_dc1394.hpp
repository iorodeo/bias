#ifdef WITH_DC1394
#ifndef BIAS_CAMERA_DEVICE_DC1394_HPP 
#define BIAS_CAMERA_DEVICE_DC1394_HPP

#include "camera_device.hpp"
#include "guid.hpp"
#include "basic_types.hpp"
#include <dc1394/dc1394.h>

namespace bias {

    class CameraDevice_dc1394 : public CameraDevice
    {
        public:
            CameraDevice_dc1394();
            explicit CameraDevice_dc1394(Guid guid);
            virtual ~CameraDevice_dc1394();
            virtual CameraLib getCameraLib();
            virtual void printInfo() {};    // TO DO //
            virtual void printGuid() {};    // TO DO //
            virtual std::string toString(); // TO DO //

            virtual void connect() {};      // TO DO //
            virtual void disconnect() {};   // TO DO //

            virtual void startCapture() {}; // TO DO // 
            virtual void stopCapture() {};  // TO DO //
            virtual void grabImage() {};    // TO DO //

        private:
            dc1394_t *context_;

    };

    typedef std::shared_ptr<CameraDevice_dc1394> CameraDevicePtr_dc1394;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_DC1394_HPP
#endif // #ifdef WITH_DC1394
