#ifndef BIAS_CAMERA_HPP
#define BIAS_CAMERA_HPP

#include "camera_fwd.hpp"
#include "property_fwd.hpp"
#include "basic_types.hpp"
#include <iostream>
#include "camera_device.hpp"

namespace cv
{
    class Mat;
}

namespace bias {

    // Forward declarations
    class Guid;

    class Camera {

        // --------------------------------------------------------------------
        // Facade wrapper for cameras. Provides a unified interface for cameras 
        // which are accessed via different underlying libraries e.g. libdc1394, 
        // flycapture2, etc.
        // --------------------------------------------------------------------
       
        public:
            Camera();
            explicit Camera(Guid guid);
            ~Camera();
            CameraLib getCameraLib();

            Guid getGuid();

            void connect();
            void disconnect();      

            void startCapture(); 
            void stopCapture();
            void grabImage(cv::Mat &image);
            cv::Mat grabImage();
            TimeStamp getImageTimeStamp();

            bool isConnected();
            bool isCapturing();

            // Get/set videomode, framerate and image info
            VideoMode getVideoMode();
            FrameRate getFrameRate();
            ImageMode getImageMode();
            ImageInfo getImageInfo();

            VideoModeList getAllowedVideoModes();
            FrameRateList getAllowedFrameRates(VideoMode videoMode);
            ImageModeList getAllowedImageModes();

            bool isAllowedVideoMode(VideoMode videoMode);
            bool isAllowedFrameRate(FrameRate frameRate, VideoMode videoMode);
            bool isAllowedImageMode(ImageMode imageMode);

            void setVideoMode(VideoMode vidMode);
            void setVideoMode(VideoMode vidMode, FrameRate frmRate);
            void setFrameRate(FrameRate frmRate); 
             
            // Get/set properties
            Property getProperty(PropertyType propType);
            PropertyMap getMapOfProperties();
            PropertyList getListOfProperties();

            PropertyInfo getPropertyInfo(PropertyType propType);
            PropertyInfoMap getMapOfPropertyInfos();
            PropertyInfoList getListOfPropertyInfos();

            unsigned int getValue(PropertyType propType);
            unsigned int getMinValue(PropertyType propType);
            unsigned int getMaxValue(PropertyType propType);

            float getAbsoluteValue(PropertyType propType);
            float getMinAbsoluteValue(PropertyType propType);
            float getMaxAbsoluteValue(PropertyType propType);

            bool isPresent(PropertyType propType);
            bool isAutoCapable(PropertyType propType);
            bool isManualCapable(PropertyType propType);
            bool isAbsoluteCapable(PropertyType propType);
            bool isOnePushCapable(PropertyType propType);
            bool isOnOffCapable(PropertyType propType);
            bool isReadOutCapable(PropertyType propType);
            bool haveUnits(PropertyType propType);

            bool isAbsoluteControlEnabled(PropertyType propType);
            bool isOnePushEnabled(PropertyType propType);
            bool isOn(PropertyType propType);
            bool isAutoActive(PropertyType propType);

            std::string getUnits(PropertyType propType);
            std::string getUnitsAbbr(PropertyType propType);

            void setProperty(Property property);
            void setValue(PropertyType propType, unsigned int value);
            void setAbsoluteValue(PropertyType propType, float absValue);

            std::string getPropertyString(PropertyType propType);
            std::string getPropertyInfoString(PropertyType propType);
            std::string getAllPropertiesString();

            // Get/set trigger
            void setTriggerInternal();
            void setTriggerExternal();
            TriggerType getTriggerType();

            // Format 7 settings
            Format7Settings getFormat7Settings();
            Format7Info getFormat7Info(ImageMode imgMode);

            bool validateFormat7Settings(Format7Settings settings);
            void setFormat7Configuration(Format7Settings settings, float percentSpeed);

            void setFormat7ImageMode(ImageMode imgMode) {};        // TO DO //
            void setFormat7PixelFormat(PixelFormat pixFormat) {};  // TO DO //

            ImageModeList getListOfSupportedImageModes();
            PixelFormatList getListOfSupportedPixelFormats(ImageMode imgMode);

            // Get basic camera information
            std::string getVendorName();
            std::string getModelName();
            bool isColor();
                
            // Print methods for displaying information
            void printInfo();
            void printGuid();
            void printProperty(PropertyType propType);
            void printPropertyInfo(PropertyType propType);
            void printAllProperties();
            void printAllPropertyInfos();
            void printImageInfo();

            friend std::ostream& operator<< (std::ostream &out, Camera &camera);

        private:
            CameraDevicePtr cameraDevicePtr_;
            void createCameraDevice_fc2(Guid guid);
            void createCameraDevice_dc1394(Guid guid);

    };

    class CameraPtrCmp : public std::binary_function<CameraPtr, CameraPtr, bool>
    {
        // Comparison object for shared_ptrs to Guid objects  
        public:
            bool operator() (const CameraPtr &camPtr0, const CameraPtr &camPtr1);
    };

}

#endif // #ifndef BIAS_CAMERA_HPP
