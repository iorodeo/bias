#include "camera.hpp"
#include "exception.hpp"
#include <sstream>
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

    void Camera::printInfo() 
    { 
        cameraDevicePtr_ -> printInfo(); 
    }

    void Camera::printGuid() 
    { 
        cameraDevicePtr_ -> printGuid(); 
    }

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

    void Camera::grabImage()
    {
        cameraDevicePtr_ -> grabImage();
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

    Property Camera::getProperty(PropertyType propertyType)
    {
        return cameraDevicePtr_ -> getProperty(propertyType);
    }

    PropertyInfo Camera::getPropertyInfo(PropertyType propertyType)
    {
        return cameraDevicePtr_ -> getPropertyInfo(propertyType);
    }

    void Camera::setProperty(Property property)
    {
        cameraDevicePtr_ -> setProperty(property);
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
