#ifdef WITH_SPIN
#ifndef BIAS_CAMERA_DEVICE_SPIN_HPP
#define BIAS_CAMERA_DEVICE_SPIN_HPP

#include <string>
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

            //virtual Property getProperty(PropertyType propType);
            //virtual PropertyInfo getPropertyInfo(PropertyType propType);

            //virtual void setProperty(Property prop);
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

            TimeStamp timeStamp_;
            int64_t timeStamp_ns_;

            //bool isFirst_;

            bool imageOK_ = false;
            spinImage hSpinImage_ = nullptr;

            //void initialize();

            bool grabImageCommon(std::string &errMsg);
            bool releaseSpinImage(spinImage &hImage);
            bool destroySpinImage(spinImage &hImage);

            ////void createPropertyMap();

            void setupTimeStamping();
            void updateTimeStamp();

            // spin get methods
            // ---------------

            std::vector<spinPixelFormatEnums> getSupportedPixelFormats_spin();

            //spinPGRGuid getGuid_spin();
            //void getVideoModeAndFrameRate(spinVideoMode &vidMode, spinFrameRate &frmRate);
            //spinPropertyInfo getPropertyInfo_spin(spinPropertyType propType);
            //spinProperty getProperty_spin(spinPropertyType propType);
            //spinFormat7Configuration getFormat7Configuration();
            //spinTriggerMode getTriggerMode_spin();
            //spinTriggerModeInfo getTriggerModeInfo_spin();
            //spinConfig getConfiguration_spin();

            //// spin set methods
            //// ---------------
            //void setProperty(spinProperty prop);
            //void setTriggerMode(spinTriggerMode trigMode);
            //void setConfiguration_spin(spinConfig &config);

            //// Temporary 
            //// --------------------------------------------
            //void setVideoModeToFormat7(ImageMode mode);
            //void setVideoModeToFormat7(spinMode mode);
            //void printFormat7Configuration();

    };


    typedef std::shared_ptr<CameraDevice_spin> CameraDevicePtr_spin;

}

#endif // #ifndef BIAS_CAMERA_DEVICE_SPIN_HPP
#endif // #ifdef WITH_SPIN
