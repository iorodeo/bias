#ifdef WITH_FC2
#ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#define BIAS_CAMERA_DEVICE_FC2_HPP

#include <string>
#include <opencv2/core/core.hpp>
#include "utils.hpp"
#include "camera_device.hpp"
#include "property.hpp"
#include "FlyCapture2_C.h"

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
            virtual cv::Mat grabImage();
            virtual void grabImage(cv::Mat &image);

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
            virtual ImageInfo getImageInfo();

            virtual void setProperty(Property prop);
            virtual void setVideoMode(VideoMode vidMode, FrameRate frmRate);
            virtual void setFormat7ImageMode(ImageMode imgMode); // TO DO //

            virtual Format7Settings getFormat7Settings();
            virtual Format7Info getFormat7Info(ImageMode imgMode);

            virtual bool validateFormat7Settings(Format7Settings settings);
            virtual void setFormat7Configuration(Format7Settings settings, float percentSpeed);

            virtual PixelFormatList getListOfSupportedPixelFormats(ImageMode imgMode);

            virtual void setTriggerInternal();
            virtual void setTriggerExternal();
            virtual TriggerType getTriggerType();

            virtual std::string getVendorName();
            virtual std::string getModelName();

            virtual TimeStamp getImageTimeStamp();
            
            virtual std::string toString();
            virtual void printGuid();
            virtual void printInfo();

        private:

            fc2Context context_ {nullptr};
            fc2CameraInfo cameraInfo_;
            fc2Image rawImage_;
            fc2Image convertedImage_;
            bool useConverted_;

            TimeStamp timeStamp_;
            unsigned int cycleSecondsLast_;   // Used with embedded timestamp only

            bool isFirst_;
            bool rawImageCreated_;
            bool convertedImageCreated_;
            bool haveEmbeddedTimeStamp_;

            void initialize();
            void createRawImage();
            void destroyRawImage();
            //void grabImageCommon();
            bool grabImageCommon(std::string &errMsg);

            void createConvertedImage();
            void destroyConvertedImage();

            //void createPropertyMap();

            void setupTimeStamping();
            void updateTimeStamp();

            // fc2 get methods
            // ---------------
            fc2PGRGuid getGuid_fc2();
            void getVideoModeAndFrameRate(fc2VideoMode &vidMode, fc2FrameRate &frmRate);
            fc2PropertyInfo getPropertyInfo_fc2(fc2PropertyType propType);
            fc2Property getProperty_fc2(fc2PropertyType propType);
            fc2Format7Configuration getFormat7Configuration();
            fc2TriggerMode getTriggerMode_fc2();
            fc2TriggerModeInfo getTriggerModeInfo_fc2();
            fc2Config getConfiguration_fc2();

            // fc2 set methods
            // ---------------
            void setProperty(fc2Property prop);
            void setTriggerMode(fc2TriggerMode trigMode);
            void setConfiguration_fc2(fc2Config &config);

            // Temporary 
            // --------------------------------------------
            void setVideoModeToFormat7(ImageMode mode);
            void setVideoModeToFormat7(fc2Mode mode);
            void printFormat7Configuration();

    };


    typedef std::shared_ptr<CameraDevice_fc2> CameraDevicePtr_fc2;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_FC2_HPP
#endif // #ifdef WITH_FC2
