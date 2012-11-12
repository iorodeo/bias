#ifdef WITH_DC1394
#ifndef BIAS_CAMERA_DEVICE_DC1394_HPP 
#define BIAS_CAMERA_DEVICE_DC1394_HPP

#include "camera_device.hpp"
#include "guid.hpp"
#include "property.hpp"
#include "basic_types.hpp"
#include <dc1394/dc1394.h>

namespace bias {

    class CameraDevice_dc1394 : public CameraDevice
    {
        static const unsigned int DEFAULT_NUM_DMA_BUFFER=5;

        public:
            CameraDevice_dc1394();
            explicit CameraDevice_dc1394(Guid guid);
            virtual ~CameraDevice_dc1394();
            virtual CameraLib getCameraLib();

            virtual void connect();    
            virtual void disconnect(); 

            virtual void startCapture();  
            virtual void stopCapture();  
            virtual void grabImage();  

            virtual bool isColor() {return false;} // TO DO //

            virtual VideoMode getVideoMode() {}; // TO DO //
            virtual FrameRate getFrameRate() {}; // TO DO //
            virtual ImageMode getImageMode() {}; // TO DO //
            virtual VideoModeList getAllowedVideoModes();  // TO DO //
            virtual FrameRateList getAllowedFrameRates(VideoMode vidMode);  // TO DO //
            virtual ImageModeList getAllowedImageModes();  // TO DO //
            virtual Property getProperty(PropertyType propType) {}; // TO DO //
            virtual PropertyInfo getPropertyInfo(PropertyType propType) {}; // TO DO //

            virtual void setProperty(Property prop) {}; // TO DO //
            virtual void setVideoMode(VideoMode vidMode) {}; // TO DO //
            virtual void setFrameRate(FrameRate frmRate) {}; // TO DO //
            virtual void setImageMode(ImageMode imgMode) {}; // TO DO //

            virtual std::string toString();
            virtual void printGuid();
            virtual void printInfo();  

        private:
            unsigned int numDMABuffer_;
            dc1394_t *context_dc1394_;
            dc1394camera_t *camera_dc1394_;
            dc1394video_frame_t *frame_dc1394_;

    };

    typedef std::shared_ptr<CameraDevice_dc1394> CameraDevicePtr_dc1394;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_DC1394_HPP
#endif // #ifdef WITH_DC1394
