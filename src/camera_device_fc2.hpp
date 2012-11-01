#ifdef WITH_FC2
#ifndef CAMERADEV_FC2_HPP
#define CAMERADEV_FC2_HPP

#include <string>
#include "guid.hpp"
#include "basic_types.hpp"
#include "camera_device.hpp"
#include "C/FlyCapture2_C.h"

namespace bias {

    class CameraDevice_fc2 : public CameraDevice
    {
        public:
            CameraDevice_fc2() {};
            CameraDevice_fc2(Guid guid);
            virtual ~CameraDevice_fc2() {};
            virtual CameraLib getCameraLib();
            virtual void connect();
            virtual void printInfo();
            virtual void printGuid();
            virtual std::string toString();

        private:
            fc2Context context_;
            fc2PGRGuid getGuid_fc2();
    };

}

#endif
#endif
