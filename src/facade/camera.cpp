#include "camera.hpp"
#include "property.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include "guid.hpp"
#include "format7.hpp"
#include <sstream>
#include <opencv2/core/core.hpp>
#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#endif
#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#endif

namespace bias {


    Camera::Camera() { 
        cameraDevicePtr_ = std::make_shared<CameraDevice>();
    }


    Camera::Camera(Guid guid) 
    {
        std::stringstream ssError;
        switch ( guid.getCameraLib() )
        {
            case CAMERA_LIB_FC2:
                createCameraDevice_fc2(guid);
                break;

            case CAMERA_LIB_DC1394:
                createCameraDevice_dc1394(guid);
                break;

            case CAMERA_LIB_UNDEFINED:
                ssError << __PRETTY_FUNCTION__;
                ssError << ": camera library is not defined";
                throw RuntimeError(ERROR_CAMERA_LIB_UNDEFINED, ssError.str()); 
                break;

            default:
                ssError << __PRETTY_FUNCTION__;
                ssError << ": camera library unknown";
                throw RuntimeError(ERROR_CAMERA_LIB_UNKNOWN, ssError.str());
                break;
        }
    }
    

    Camera::~Camera() { }


    CameraLib Camera::getCameraLib() 
    { 
        return cameraDevicePtr_ -> getCameraLib(); 
    }


    Guid Camera::getGuid()
    {
        return cameraDevicePtr_ -> getGuid();
    }


    void Camera::connect() 
    { 
        cameraDevicePtr_ -> connect(); 
    }


    void Camera::disconnect() 
    {
        cameraDevicePtr_ -> disconnect();
    }


    void Camera::startCapture()
    {
        cameraDevicePtr_ -> startCapture();
    }


    void Camera::stopCapture()
    {
        cameraDevicePtr_ -> stopCapture();
    }


    cv::Mat Camera::grabImage()
    {
        return cameraDevicePtr_ -> grabImage();
    }


    void Camera::grabImage(cv::Mat &image)
    {
        cameraDevicePtr_ -> grabImage(image);
    }


    TimeStamp Camera::getImageTimeStamp()
    {
        return cameraDevicePtr_ -> getImageTimeStamp();
    }


    bool Camera::isConnected()
    {
        return cameraDevicePtr_ -> isConnected();
    }


    bool Camera::isCapturing() 
    { 
        return cameraDevicePtr_ -> isCapturing();
    }


    bool Camera::isColor()
    {
        return cameraDevicePtr_ -> isColor();
    }


    VideoMode Camera::getVideoMode()
    {
        return cameraDevicePtr_ -> getVideoMode();
    }


    FrameRate Camera::getFrameRate()
    {
        return cameraDevicePtr_ -> getFrameRate();
    }


    ImageMode Camera::getImageMode()
    {
        return cameraDevicePtr_ -> getImageMode();
    }


    VideoModeList Camera::getAllowedVideoModes()
    {
        return cameraDevicePtr_ -> getAllowedVideoModes();
    }


    FrameRateList Camera::getAllowedFrameRates(VideoMode vidMode)
    {
        return cameraDevicePtr_ -> getAllowedFrameRates(vidMode);
    }


    ImageModeList Camera::getAllowedImageModes()
    {
        return cameraDevicePtr_ -> getAllowedImageModes();
    }


    ImageInfo Camera::getImageInfo()
    {
        return cameraDevicePtr_ -> getImageInfo();
    }


    // ---
    bool Camera::isAllowedVideoMode(VideoMode videoMode)
    {
        bool allowed = false;
        VideoModeList videoModeList = getAllowedVideoModes();
        VideoModeList::iterator it; 
        for (it=videoModeList.begin(); it!=videoModeList.end(); it++)
        {
            VideoMode allowedVideoMode = *it;
            if (videoMode == allowedVideoMode)
            {
                allowed = true;
                break;
            }
        }
        return allowed;
    }


    bool Camera::isAllowedFrameRate(FrameRate frameRate, VideoMode videoMode)
    {
        bool allowed = false;
        FrameRateList frameRateList = getAllowedFrameRates(videoMode);
        FrameRateList::iterator it;
        for (it=frameRateList.begin(); it!=frameRateList.end(); it++)
        {
            FrameRate allowedFrameRate = *it;
            if (frameRate == allowedFrameRate)
            {
                allowed = true;
                break;
            }
        }
        return allowed;
    }


    bool Camera::isAllowedImageMode(ImageMode imageMode)
    {
        bool allowed = false;
        ImageModeList imageModeList = getAllowedImageModes();
        ImageModeList::iterator it;
        for (it=imageModeList.begin(); it!=imageModeList.end(); it++)
        {
            ImageMode allowedImageMode = *it;
            if (imageMode == allowedImageMode)
            {
                allowed = true;
                break;
            }
        }
        return allowed;
    }
    // ---


    Property Camera::getProperty(PropertyType propType)
    {
        return cameraDevicePtr_ -> getProperty(propType);
    }


    PropertyMap Camera::getMapOfProperties()
    {
        PropertyMap propMap;
        PropertyTypeList propTypeList = getListOfPropertyTypes();

        for (
                PropertyTypeList::iterator it=propTypeList.begin(); 
                it!=propTypeList.end(); 
                it++
            )
        {
            PropertyType propType = *it;
            Property prop = getProperty(propType);
            propMap[propType] = prop;
        }
        return propMap;
    }


    PropertyList Camera::getListOfProperties()
    {
        PropertyList  propList;
        PropertyTypeList propTypeList = getListOfPropertyTypes();

        for (
                PropertyTypeList::iterator it=propTypeList.begin(); 
                it!=propTypeList.end(); 
                it++
            )
        {
            PropertyType propType = *it;
            Property prop = getProperty(propType);
            propList.push_back(prop);
        }
        return propList;
    }


    PropertyInfo Camera::getPropertyInfo(PropertyType propType)
    {
        return cameraDevicePtr_ -> getPropertyInfo(propType);
    }


    PropertyInfoMap Camera::getMapOfPropertyInfos()
    {
        PropertyInfoMap  propInfoMap;
        PropertyTypeList propTypeList = getListOfPropertyTypes();

        for (
                PropertyTypeList::iterator it=propTypeList.begin(); 
                it!=propTypeList.end(); 
                it++
            )
        {
            PropertyType propType = *it;
            PropertyInfo propInfo = getPropertyInfo(propType);
            propInfoMap[propType] = propInfo;
        }
        return propInfoMap;
    }


    PropertyInfoList Camera::getListOfPropertyInfos()
    {
        PropertyInfoList propInfoList;
        PropertyTypeList propTypeList = getListOfPropertyTypes();

        for (
                PropertyTypeList::iterator it=propTypeList.begin(); 
                it!=propTypeList.end(); 
                it++
            )
        {
            PropertyType propType = *it;
            PropertyInfo propInfo = getPropertyInfo(propType);
            propInfoList.push_back(propInfo);
        }
        return propInfoList;
    }


    unsigned int Camera::getValue(PropertyType propType)
    {
        Property property;
        property = getProperty(propType);
        return property.value;
    }


    unsigned int Camera::getMinValue(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo = getPropertyInfo(propType);
        return propInfo.minValue;
    }


    unsigned int Camera::getMaxValue(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo = getPropertyInfo(propType);
        return propInfo.maxValue;
    }


    float Camera::getAbsoluteValue(PropertyType propType)
    {
        Property property;
        property = getProperty(propType);
        return property.absoluteValue;
    }


    float Camera::getMinAbsoluteValue(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo = getPropertyInfo(propType);
        return propInfo.minAbsoluteValue;
    }


    float Camera::getMaxAbsoluteValue(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo = getPropertyInfo(propType);
        return propInfo.maxAbsoluteValue;
    } 


    void Camera::setProperty(Property property)
    {
        cameraDevicePtr_ -> setProperty(property);
    } 


    bool Camera::isPresent(PropertyType propType)
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.present;
    }


    bool Camera::isAutoCapable(PropertyType propType)
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.autoCapable;
    }


    bool Camera::isManualCapable(PropertyType propType)
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.manualCapable;
    }


    bool Camera::isAbsoluteCapable(PropertyType propType)
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.absoluteCapable;
    }


    bool Camera::isOnePushCapable(PropertyType propType) 
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.onePushCapable;
    }


    bool Camera::isOnOffCapable(PropertyType propType) 
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.onOffCapable;
    }


    bool Camera::isReadOutCapable(PropertyType propType) 
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.readOutCapable;
    }


    bool Camera::isAbsoluteControlEnabled(PropertyType propType) 
    {
        Property prop = getProperty(propType);
        return prop.absoluteControl;
    }


    bool Camera::isOnePushEnabled(PropertyType propType) 
    {
        Property prop = getProperty(propType);
        return prop.onePush;
    }


    bool Camera::isOn(PropertyType propType) 
    {
        Property prop = getProperty(propType);
        return prop.on;
    }


    bool Camera::isAutoActive(PropertyType propType) 
    {
        Property prop = getProperty(propType);
        return prop.autoActive;
    }


    std::string Camera::getUnits(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo = getPropertyInfo(propType);
        return propInfo.units;
    }
    

    std::string Camera::getUnitsAbbr(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo = getPropertyInfo(propType);
        return propInfo.unitsAbbr;
    }


    void Camera::setValue(PropertyType propType, unsigned int value)
    {
        Property property;
        property = getProperty(propType);
        property.value = value;
        setProperty(property);
    }


    void Camera::setAbsoluteValue(PropertyType propType, float absValue)
    {
        Property property;
        property = getProperty(propType);
        property.absoluteValue = absValue;
    } 


    void Camera::setVideoMode(VideoMode vidMode) 
    {
        FrameRate frmRate;

        if (vidMode == VIDEOMODE_FORMAT7) 
        {
            frmRate = FRAMERATE_FORMAT7;
        }
        else
        {
            // Get list of allowed frame rates for this video mode
            FrameRateList frmRateList = getAllowedFrameRates(vidMode);
            if (frmRateList.empty()) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set Video mode, no allowed FrameRate";
                throw RuntimeError(ERROR_NO_FRAMERATE_FOR_VIDEOMODE, ssError.str());
            }

            //find maximum frame rate possible for this video mode.
            frmRate= frmRateList.front();
            for (
                    FrameRateList::iterator it = frmRateList.begin();
                    it != frmRateList.end();
                    it++
                )
            {
                FrameRate itemFrmRate = *it;
                if (itemFrmRate == FRAMERATE_FORMAT7) { continue; }
                float itemValue = getFrameRateAsFloat(itemFrmRate);
                float currValue = getFrameRateAsFloat(frmRate);
                frmRate= itemValue >= currValue ? itemFrmRate : frmRate;
            }
        }
        setVideoMode(vidMode,frmRate);
    } 


    void Camera::setVideoMode(VideoMode vidMode, FrameRate frmRate) 
    {
        cameraDevicePtr_ -> setVideoMode(vidMode, frmRate);
    } 


    void Camera::setFrameRate(FrameRate frmRate)
    {
        VideoMode vidMode = getVideoMode();
        setVideoMode(vidMode, frmRate);
    }


    void Camera::setTriggerInternal()
    {
        cameraDevicePtr_ -> setTriggerInternal();
    }


    void Camera::setTriggerExternal()
    {
        cameraDevicePtr_ -> setTriggerExternal();
    }


    void Camera::setTriggerType(TriggerType triggerType)
    {
        if (triggerType == TRIGGER_EXTERNAL)
        {
            cameraDevicePtr_ -> setTriggerExternal();
        }
        else
        {
            cameraDevicePtr_ -> setTriggerInternal();
        };
    }


    TriggerType Camera::getTriggerType()
    {
        return cameraDevicePtr_ -> getTriggerType();
    }


    Format7Settings Camera::getFormat7Settings()
    {
        return cameraDevicePtr_ -> getFormat7Settings();
    }


    Format7Info Camera::getFormat7Info(ImageMode imgMode)
    {
        return cameraDevicePtr_ -> getFormat7Info(imgMode);
    }


    bool Camera::validateFormat7Settings(Format7Settings settings)
    {
        return cameraDevicePtr_ -> validateFormat7Settings(settings);
    }


    void Camera::setFormat7Configuration(
            Format7Settings settings, 
            float percentSpeed
            )
    {
        return cameraDevicePtr_ -> setFormat7Configuration(settings, percentSpeed);
    }


    ImageModeList Camera::getListOfSupportedImageModes()
    {
        ImageModeList modeList = getListOfImageModes();
        ImageModeList supportedModeList;

        ImageModeList::iterator it;
        for (it=modeList.begin(); it!=modeList.end(); it++)
        {
            ImageMode mode = *it;
            Format7Info info = getFormat7Info(mode);
            if (info.supported)
            {
                supportedModeList.push_back(mode);
            }
        }
        return supportedModeList;
    }


    PixelFormatList Camera::getListOfSupportedPixelFormats(ImageMode imgMode)
    {
        return cameraDevicePtr_ -> getListOfSupportedPixelFormats(imgMode);
    }

    std::string Camera::getPropertyString(PropertyType propType)
    {
        Property prop = getProperty(propType);
        return prop.toString();
    }


    std::string Camera::getPropertyInfoString(PropertyType propType) 
    {
        PropertyInfo propInfo = getPropertyInfo(propType);
        return propInfo.toString();
    }


    std::string Camera::getAllPropertiesString()
    {
        std::stringstream ss;
        PropertyTypeList propTypeList = getListOfPropertyTypes();
        for (
                PropertyTypeList::iterator it=propTypeList.begin();
                it != propTypeList.end();
                it++
            )
        {
            PropertyType propType = *it;
            ss << getPropertyString(propType); 
        }
        return ss.str();
    }


    std::string Camera::getVendorName()
    {
        return cameraDevicePtr_ -> getVendorName();
    }


    std::string Camera::getModelName()
    {
        return cameraDevicePtr_ -> getModelName();
    }


    void Camera::printInfo() 
    { 
        cameraDevicePtr_ -> printInfo(); 
    }


    void Camera::printGuid() 
    { 
        cameraDevicePtr_ -> printGuid(); 
    }


    void Camera::printProperty(PropertyType propType) 
    {
        std::cout << getPropertyString(propType) << std::endl;
    }


    void Camera::printPropertyInfo(PropertyType propType) 
    {
        std::cout << getPropertyInfoString(propType) << std::endl;
    }


    void Camera::printAllProperties()
    {
        std::string allPropString = getAllPropertiesString();
        std::cout << getAllPropertiesString();
        //PropertyTypeList propTypeList = getListOfPropertyTypes();
        //for (
        //        PropertyTypeList::iterator it=propTypeList.begin();
        //        it != propTypeList.end();
        //        it++
        //    )
        //{
        //    PropertyType propType = *it;
        //    printProperty(propType);
        //}
    }


    void Camera::printAllPropertyInfos() 
    {
        PropertyTypeList propTypeList = getListOfPropertyTypes();
        for (
                PropertyTypeList::iterator it=propTypeList.begin();
                it != propTypeList.end();
                it++
            )
        {
            PropertyType propType = *it;
            printPropertyInfo(propType);
        }
    }


    void Camera::printImageInfo()
    {
        ImageInfo imgInfo = getImageInfo();
        std::cout << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << "Camera ImageInfo " << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << std::endl;
        std::cout << getImageInfoString(imgInfo);
    }


    // FlyCapture2 specific methods
    // ------------------------------------------------------------------------
#ifdef WITH_FC2

    void Camera::createCameraDevice_fc2(Guid guid)
    { 
        cameraDevicePtr_ = std::make_shared<CameraDevice_fc2>(guid);
    }

#else

    // Dummy methods for when the library isn't included - allows the ifdefs 
    // to be limit to two locations.

    void Camera::createCameraDevice_fc2(Guid guid)
    {
        throw_ERROR_NO_FC2(std::string(__PRETTY_FUNCTION__));
    }

#endif

    // Libdc1394 specific methods
    // ------------------------------------------------------------------------
#ifdef WITH_DC1394

    void Camera::createCameraDevice_dc1394(Guid guid)
    { 
        cameraDevicePtr_ = std::make_shared<CameraDevice_dc1394>(guid);
    }

#else

    // Dummy methods for when the library isn't included - allows the ifdefs 
    // to  be limited to two locations.

    void Camera::createCameraDevice_dc1394(Guid guid)
    {
        throw_ERROR_NO_DC1394(std::string(__PRETTY_FUNCTION__));
    }

#endif

    // Shared pointer comparison operator - for use in sets, maps, etc.
    //-------------------------------------------------------------------------

    bool CameraPtrCmp::operator() (const CameraPtr &camPtr0, const CameraPtr &camPtr1) 
    {
        // Order cameras based on guid order
        Guid guid0 = camPtr0 -> getGuid();
        Guid guid1 = camPtr1 -> getGuid();
        if (guid0 == guid1) {
            return false;
        }
        else {
            bool rval = (guid0 < guid1);
            return rval;
        }
    };

} // namespace bias
