#ifdef WITH_FC2
#ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#define BIAS_CAMERA_DEVICE_FC2_HPP

#include <string>
#include "utils.hpp"
#include "camera_device.hpp"
#include "property.hpp"
#include "C/FlyCapture2_C.h"

namespace bias {

    struct fc2Format7Configuration 
    {
        fc2Format7ImageSettings imageSettings;
        unsigned int packetSize;
        float percentage; 
    };

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
            virtual bool isSupported(VideoMode videoMode, FrameRate frameRate);
            virtual bool isSupported(ImageMode imageMode);

            virtual VideoMode getVideoMode();
            virtual FrameRate getFrameRate();
            virtual ImageMode getImageMode();
            virtual VideoModeList getAllowedVideoModes();
            virtual FrameRateList getAllowedFrameRates(VideoMode videoMode);
            virtual ImageModeList getAllowedImageModes();
            virtual Property getProperty(PropertyType propertyType);

            virtual void setVideoMode(VideoMode videoMode);
            virtual void setFrameRate(FrameRate frameRate);
            virtual void setImageMode(ImageMode imageMode);
            
            virtual std::string toString();
            virtual void printGuid();
            virtual void printInfo();

        private:

            fc2Context context_;
            fc2CameraInfo cameraInfo_;
            fc2Image rawImage_;
            fc2Image convertedImage_;

            bool rawImageCreated_;
            bool convertedImageCreated_;

            fc2PGRGuid getGuid_fc2();

            void createRawImage();
            void destroyRawImage();

            void createConvertedImage();
            void destroyConvertedImage();

            void createPropertyMap();

            void getVideoModeAndFrameRate(fc2VideoMode &videoMode, fc2FrameRate &frameRate);
            fc2PropertyInfo getPropertyInfo_fc2(fc2PropertyType propType);
            fc2Property getProperty_fc2(fc2PropertyType propType);
            fc2Format7Configuration getFormat7Configuration();

            // Temporary 
            // --------------------------------------------
            void setVideoMode_Format7Mode0();

    };

    typedef std::shared_ptr<CameraDevice_fc2> CameraDevicePtr_fc2;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#endif // #ifdef WITH_FC2
