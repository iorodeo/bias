#ifndef BIAS_CAMERA_DEVICE_HPP
#define BIAS_CAMERA_DEVICE_HPP

#include <string>
#include <memory>
#include "basic_types.hpp"
#include "property.hpp"
#include "guid.hpp"

namespace bias 
{
    // Base class for camera device objects.

    class CameraDevice
    {
        public:
            CameraDevice();
            explicit CameraDevice(Guid guid); 

            Guid getGuid(); 
            virtual ~CameraDevice() {};
            virtual CameraLib getCameraLib() {};

            virtual void connect() {};
            virtual void disconnect() {};

            virtual void startCapture() {};
            virtual void stopCapture() {};
            virtual void grabImage() {};

            virtual bool isConnected(); 
            virtual bool isCapturing();
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
            virtual PropertyInfo getPropertyInfo(PropertyType propertyType);

            virtual void setVideoMode(VideoMode videoMode) {};
            virtual void setFrameRate(FrameRate frameRate) {};
            virtual void setImageMode(ImageMode imageMode) {};

            virtual std::string toString(); 

            virtual void printInfo() {};
            virtual void printGuid() {};

        protected:
            Guid guid_;
            bool connected_;
            bool capturing_;
    };

    typedef std::shared_ptr<CameraDevice> CameraDevicePtr;
}


#endif // #ifndef BIAS_CAMERA_DEVICE_HPP
