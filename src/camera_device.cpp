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
    bool CameraDevice::isSupported(VideoMode videoMode, FrameRate frameRate)
    {
        return false;
    }

    bool CameraDevice::isSupported(ImageMode imageMode)
    {
        return false;
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

    FrameRateList CameraDevice::getAllowedFrameRates(VideoMode videoMode) 
    { 
        return FrameRateList(); 
    } 

    ImageModeList CameraDevice::getAllowedImageModes()
    {
        return ImageModeList();
    }

    std::string CameraDevice::toString() 
    {
        return std::string("camera not defined");
    }
}
