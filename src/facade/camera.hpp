#ifndef BIAS_CAMERA_HPP
#define BIAS_CAMERA_HPP

#include "camera_fwd.hpp"
#include "basic_types.hpp"
#include <iostream>
#include "property.hpp"
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
            cv::Mat grabImage();
            void grabImage(cv::Mat &image);

            bool isConnected();
            bool isCapturing();
            bool isColor();

            VideoMode getVideoMode();
            FrameRate getFrameRate();
            ImageMode getImageMode();
            VideoModeList getAllowedVideoModes();
            FrameRateList getAllowedFrameRates(VideoMode videoMode);
            ImageModeList getAllowedImageModes();

            ImageInfo getImageInfo();

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
            
            void setVideoMode(VideoMode vidMode);
            void setVideoMode(VideoMode vidMode, FrameRate frmRate);
            void setFrameRate(FrameRate frmRate); 

            void setTriggerInternal();
            void setTriggerExternal();
            TriggerType getTriggerType();

            void setFormat7ImageMode(ImageMode imgMode) {}; // TO DO //
            void setFormat7PixelFormat(PixelFormat pixFormat) {}; // TO DO //

            std::string getPropertyString(PropertyType propType);
            std::string getPropertyInfoString(PropertyType propType);
                
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
