#include "camera_device.hpp"
#include "exception.hpp"

namespace bias 
{
    CameraDevice::CameraDevice() 
    { 
        connected_ = false; 
        capturing_ = false; 
    }


    CameraDevice::CameraDevice(Guid guid)  
    {
        guid_ = guid;
        connected_ = false;
        capturing_ = false;
    }


    Guid CameraDevice::getGuid() 
    {
        return guid_;
    }


    cv::Mat CameraDevice::grabImage() 
    {   
        return cv::Mat(); 
    }


    bool CameraDevice::isConnected() 
    { 
        return connected_; 
    }


    bool CameraDevice::isCapturing() 
    { 
        return capturing_; 
    }


    bool CameraDevice::isColor() 
    { 
        return false; 
    }


    bool CameraDevice::isSupported(VideoMode vidMode, FrameRate frmRate)
    {
        return false;
    }


    bool CameraDevice::isSupported(ImageMode imgMode)
    {
        return false;
    }

    unsigned int CameraDevice::getNumberOfImageMode()
    {
        return 0;
    }


    VideoMode CameraDevice::getVideoMode() 
    { 
        return VIDEOMODE_UNSPECIFIED; 
    }


    FrameRate CameraDevice::getFrameRate() 
    { 
        return FRAMERATE_UNSPECIFIED; 
    }


    ImageMode CameraDevice::getImageMode() 
    { 
        return IMAGEMODE_UNSPECIFIED; 
    }


    VideoModeList CameraDevice::getAllowedVideoModes()
    {
        return VideoModeList();
    }


    FrameRateList CameraDevice::getAllowedFrameRates(VideoMode vidMode) 
    { 
        return FrameRateList(); 
    } 


    ImageModeList CameraDevice::getAllowedImageModes()
    {
        return ImageModeList();
    }


    Property CameraDevice::getProperty(PropertyType propType)
    {
        return Property();
    }


    PropertyInfo CameraDevice::getPropertyInfo(PropertyType propType)
    {
        return PropertyInfo();
    }


    ImageInfo CameraDevice::getImageInfo()
    {
        return ImageInfo();
    }

    
    Format7Settings CameraDevice::getFormat7Settings()
    {
        return Format7Settings();
    }


    Format7Info CameraDevice::getFormat7Info(ImageMode imgMode)
    {
        return Format7Info(imgMode);
    }


    bool CameraDevice::validateFormat7Settings(Format7Settings settings)
    {
        return false;
    }


    void CameraDevice::setFormat7Configuration(Format7Settings settings, float percentSpeed)
    {
        return;
    }


    PixelFormatList CameraDevice::getListOfSupportedPixelFormats(ImageMode imgMode)
    {
        return PixelFormatList();
    }


    TriggerType CameraDevice::getTriggerType()
    {
        return TRIGGER_INTERNAL;
    }

    TimeStamp CameraDevice::getImageTimeStamp()
    {
        TimeStamp ts;
        ts.seconds = 0;
        ts.microSeconds = 0;
    }


    std::string CameraDevice::toString() 
    {
        return std::string("camera not defined");
    }


    std::string CameraDevice::getVendorName()
    {
        return std::string("vendor name not defined");
    }


    std::string CameraDevice::getModelName()
    {
        return std::string("model name not defined");
    }

}
