#ifdef WITH_DC1394
#ifndef BIAS_CAMERA_DEVICE_DC1394_HPP 
#define BIAS_CAMERA_DEVICE_DC1394_HPP

#include "camera_device.hpp"
#include "guid.hpp"
#include "property.hpp"
#include "basic_types.hpp"
#include <opencv2/core/core.hpp>
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

            virtual void connect();     // Needs fixing ...
            virtual void disconnect(); 

            virtual void startCapture();  
            virtual void stopCapture();  
            virtual cv::Mat grabImage();  
            virtual void grabImage(cv::Mat &image);

            virtual bool isColor(); 
            virtual bool isSupported(ImageMode imgMode); 
            virtual bool isSupported(VideoMode vidMode, FrameRate frmRate);

            virtual VideoMode getVideoMode(); 
            virtual FrameRate getFrameRate();
            virtual ImageMode getImageMode();

            virtual VideoModeList getAllowedVideoModes();  
            virtual FrameRateList getAllowedFrameRates(VideoMode vidMode);  
            virtual ImageModeList getAllowedImageModes();  

            virtual Property getProperty(PropertyType propType); // TO DO //
            virtual PropertyInfo getPropertyInfo(PropertyType propType) {}; // TO DO //
            virtual ImageInfo getImageInfo() { return ImageInfo(); };

            virtual void setProperty(Property prop) {}; // TO DO //
            virtual void setVideoModeAndFrameRate(VideoMode vidMode, FrameRate frameRate) {}; // TO DO //
            virtual void setFormat7ImageMode(ImageMode imgMode) {}; // TO DO //

            virtual Format7Settings getFormat7Settings(); // TO DO // 
            virtual Format7Info getFormat7Info(ImageMode imgMode); // TO DO //

            virtual bool validateFormat7Settings(Format7Settings settings); // TO DO //
            virtual void setFormat7Configuration(Format7Settings settings, float percentSpeed); // TO DO //

            virtual PixelFormatList getListOfSupportedPixelFormats(ImageMode imgMode); // TO DO //

            virtual void setTriggerInternal() {}; // TO DO //
            virtual void setTriggerExternal() {}; // TO DO //
            virtual TriggerType getTriggerType() {}; // TO DO //

            virtual std::string getVendorName(); 
            virtual std::string getModelName(); 

            virtual TimeStamp getImageTimeStamp();

            virtual std::string toString();
            virtual void printGuid();
            virtual void printInfo();  

        private:
            unsigned int numDMABuffer_;
            dc1394_t *context_dc1394_;
            dc1394camera_t *camera_dc1394_;
            dc1394video_frame_t *frame_dc1394_;

            TimeStamp timeStamp_;
            uint64_t startTime_;
            uint64_t timerFreq_;
            bool isFirst_;

            void updateTimeStamp();


    };

    typedef std::shared_ptr<CameraDevice_dc1394> CameraDevicePtr_dc1394;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_DC1394_HPP
#endif // #ifdef WITH_DC1394
