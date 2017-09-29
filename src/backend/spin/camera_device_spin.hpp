#ifdef WITH_SPIN
#ifndef BIAS_CAMERA_DEVICE_SPIN_HPP
#define BIAS_CAMERA_DEVICE_SPIN_HPP

#include <map>
#include <string>
#include <functional>
#include <opencv2/core/core.hpp>

#include "utils.hpp"
#include "camera_device.hpp"
#include "camera_info_spin.hpp"
#include "property.hpp"
#include "SpinnakerC.h"
#include "node_map_spin.hpp"

namespace bias {



    class CameraDevice_spin : public CameraDevice
    {
        public:

            CameraDevice_spin(); 
            explicit CameraDevice_spin(Guid guid);
            virtual ~CameraDevice_spin();

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
            virtual unsigned int getNumberOfImageMode();

            virtual VideoMode getVideoMode();
            virtual FrameRate getFrameRate();
            virtual ImageMode getImageMode();

            virtual VideoModeList getAllowedVideoModes();
            virtual FrameRateList getAllowedFrameRates(VideoMode vidMode);
            virtual ImageModeList getAllowedImageModes();

            virtual PropertyInfo getPropertyInfo(PropertyType propType);
            virtual Property getProperty(PropertyType propType);
            virtual void setProperty(Property prop);
            
            //virtual void setVideoMode(VideoMode vidMode, FrameRate frmRate);
            //virtual void setFormat7ImageMode(ImageMode imgMode); // TO DO //

            //virtual Format7Settings getFormat7Settings();
            //virtual Format7Info getFormat7Info(ImageMode imgMode);

            //virtual bool validateFormat7Settings(Format7Settings settings);
            //virtual void setFormat7Configuration(Format7Settings settings, float percentSpeed);

            //virtual PixelFormatList getListOfSupportedPixelFormats(ImageMode imgMode);

            //virtual void setTriggerInternal();
            //virtual void setTriggerExternal();
            //virtual TriggerType getTriggerType();

            virtual std::string getVendorName();
            virtual std::string getModelName();

            virtual TimeStamp getImageTimeStamp();
            
            virtual std::string toString();

            virtual void printGuid();
            
            virtual void printInfo();

            void develExpProps();


        private:


            spinSystem hSystem_ = nullptr;
            spinCamera hCamera_ = nullptr;

            spinNodeMapHandle hNodeMapCamera_ = nullptr;
            spinNodeMapHandle hNodeMapTLDevice_ = nullptr;

            NodeMapCamera_spin nodeMapCamera_;
            NodeMapTLDevice_spin nodeMapTLDevice_;
            
            CameraInfo_spin cameraInfo_;

            TimeStamp timeStamp_ = {0,0};
            int64_t timeStamp_ns_ = 0;

            bool imageOK_ = false;
            spinImage hSpinImage_ = nullptr;

            //bool isFirst_;
            //void initialize();

            bool grabImageCommon(std::string &errMsg);
            bool releaseSpinImage(spinImage &hImage);
            bool destroySpinImage(spinImage &hImage);

            ////void createPropertyMap();

            void setupTimeStamping();
            void updateTimeStamp();


            // Get Property Info methods
            static std::map<PropertyType, std::function<PropertyInfo(CameraDevice_spin*)>> getPropertyInfoDispatchMap_; 

            PropertyInfo getPropertyInfoBrightness();
            PropertyInfo getPropertyInfoGamma();
            PropertyInfo getPropertyInfoShutter();
            PropertyInfo getPropertyInfoGain();
            PropertyInfo getPropertyInfoTriggerDelay();
            PropertyInfo getPropertyInfoFrameRate();
            PropertyInfo getPropertyInfoTemperature();
            PropertyInfo getPropertyInfoTriggerMode();

            // Get property methods
            static std::map<PropertyType, std::function<Property(CameraDevice_spin*)>> getPropertyDispatchMap_; 

            Property getPropertyBrightness();
            Property getPropertyGamma();
            Property getPropertyShutter();
            Property getPropertyGain();
            Property getPropertyTriggerDelay();
            Property getPropertyFrameRate();
            Property getPropertyTemperature();
            Property getPropertyTriggerMode();

            // Set Property methods
            bool isPropertySettable(PropertyType propType, std::string &msg);
            static std::map<PropertyType, std::function<void(CameraDevice_spin*,Property)>> setPropertyDispatchMap_; 

            void setPropertyBrightness(Property prop);
            void setPropertyGamma(Property prop);
            void setPropertyShutter(Property prop);
            void setPropertyGain(Property prop);
            void setPropertyTriggerDelay(Property prop);
            void setPropertyFrameRate(Property prop);
            void setPropertyTemperature(Property prop);
            void setPropertyTriggerMode(Property prop);


            // spin get methods
            // ---------------
            std::vector<spinPixelFormatEnums> getSupportedPixelFormats_spin();

    };


    typedef std::shared_ptr<CameraDevice_spin> CameraDevicePtr_spin;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_SPIN_HPP
#endif // #ifdef WITH_SPIN
