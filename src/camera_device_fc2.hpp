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
            explicit CameraDevice_fc2(Guid guid);
            virtual ~CameraDevice_fc2();
            virtual CameraLib getCameraLib();

            virtual void connect();
            virtual void disconnect();
            
            virtual void startCapture();
            virtual void stopCapture();
            virtual void grabImage();

            virtual bool isColor();
            
            virtual std::string toString();
            virtual void printGuid();
            virtual void printInfo();

        private:
            fc2Context context_;
            fc2CameraInfo cameraInfo_;
            fc2Image rawImage_;
            fc2Image convertedImage_;
            bool workingImagesCreated_;

            fc2PGRGuid getGuid_fc2();
            void createWorkingImages();
            void destroyWorkingImages();

            // Temporary 
            void setVideoMode_Format7Mode0();
    };

    typedef std::shared_ptr<CameraDevice_fc2> CameraDevicePtr_fc2;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#endif // #ifdef WITH_FC2
