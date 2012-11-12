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
            virtual bool isSupported(VideoMode vidMode, FrameRate frmRate);
            virtual bool isSupported(ImageMode imgMode);

            virtual VideoMode getVideoMode(); 
            virtual FrameRate getFrameRate(); 
            virtual ImageMode getImageMode(); 

            virtual VideoModeList getAllowedVideoModes(); 
            virtual FrameRateList getAllowedFrameRates(VideoMode vidMode); 
            virtual ImageModeList getAllowedImageModes();

            virtual Property getProperty(PropertyType propType);
            virtual PropertyInfo getPropertyInfo(PropertyType propType);

            virtual void setProperty(Property prop) {};
            virtual void setVideoMode(VideoMode vidMode) {};
            virtual void setFrameRate(FrameRate frmRate) {};
            virtual void setImageMode(ImageMode imgMode) {};

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
