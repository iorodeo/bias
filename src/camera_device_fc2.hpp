#ifdef WITH_FC2
#ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#define BIAS_CAMERA_DEVICE_FC2_HPP

#include <string>
#include "camera_device.hpp"
#include "C/FlyCapture2_C.h"

namespace bias {

    class CameraDevice_fc2 : public CameraDevice
    {
        public:
            CameraDevice_fc2(); 
            CameraDevice_fc2(Guid guid);
            virtual ~CameraDevice_fc2();
            virtual CameraLib getCameraLib();
            virtual std::string toString();
            virtual void printInfo();
            virtual void printGuid();

            virtual void connect();
            virtual void disconnect();
            
            virtual void startCapture();
            virtual void stopCapture();
            virtual void grabImage();

        private:
            fc2Context context_;
            fc2Image rawImage_;
            bool rawImageCreated_;
            fc2PGRGuid getGuid_fc2();

            void createRawImage();
            void destroyRawImage();
    };


    typedef std::shared_ptr<CameraDevice_fc2> CameraDevicePtr_fc2;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#endif // #ifdef WITH_FC2
