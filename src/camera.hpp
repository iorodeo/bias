#ifndef BIAS_CAMERA_HPP
#define BIAS_CAMERA_HPP

#include <set>
#include <list>
#include <memory>
#include <iostream>
#include "guid.hpp"
#include "basic_types.hpp"
#include "property.hpp"
#include "camera_device.hpp"

namespace bias {

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
            void grabImage();

            bool isConnected();
            bool isCapturing();
            bool isColor();

            VideoMode getVideoMode();
            FrameRate getFrameRate();
            ImageMode getImageMode();
            VideoModeList getAllowedVideoModes();
            FrameRateList getAllowedFrameRates(VideoMode videoMode);
            ImageModeList getAllowedImageModes();

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

            std::string getUnits(PropertyType propType);
            std::string getUnitsAbbr(PropertyType propType);

            void setProperty(Property property);
            void setValue(PropertyType propType, unsigned int value);
            void setAbsoluteValue(PropertyType propType, float absValue);
            
            void setVideoMode(VideoMode videoMode) {}; // TO DO //
            void setFrameRate(FrameRate frameRate) {}; // TO DO //
            void setImageMode(ImageMode imageMode) {}; // TO DO //
                
            void printInfo();
            void printGuid();

            friend std::ostream& operator<< (std::ostream &out, Camera &camera);

        private:
            CameraDevicePtr cameraDevicePtr_;
            void createCameraDevice_fc2(Guid guid);
            void createCameraDevice_dc1394(Guid guid);

    };

    typedef std::shared_ptr<Camera> CameraPtr;

    class CameraPtrCmp : public std::binary_function<CameraPtr, CameraPtr, bool>
    {
        // Comparison object for shared_ptrs to Guid objects  
        public:
            bool operator() (const CameraPtr &camPtr0, const CameraPtr &camPtr1);
    };

    typedef std::set<CameraPtr> CameraPtrSet;
    typedef std::list<CameraPtr> CameraPtrList;
}


#endif // #ifndef BIAS_CAMERA_HPP
