#include "camera.hpp"
#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#endif
#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#endif

namespace bias {


    Camera::Camera() { 
        cameraDevicePtr_ = CameraDevicePtr(new CameraDevice);
    }

    Camera::Camera(Guid guid) 
    {
        switch ( guid.getCameraLib() )
        {
            case CAMERA_LIB_FC2:
                createCameraDevice_fc2(guid);
                break;

            case CAMERA_LIB_DC1394:
                createCameraDevice_dc1394(guid);

            case CAMERA_LIB_UNDEFINED:
                // TO DO ... some kind of error
                break;
            default:
                // TO DO ... some kind of error
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

// FlyCapture2 specific methods
// ------------------------------------------------------------------------
#ifdef WITH_FC2

    void Camera::createCameraDevice_fc2(Guid guid)
    { 
        cameraDevicePtr_ = CameraDevicePtr(new CameraDevice_fc2(guid));
    }

#else

    // Dummy methods for when the library isn't included - allows the ifdefs 
    // to be limit to two locations.

    void Camera::createCameraDevice_fc2(Guid guid)
    {
        // TO DO ... throw some kind or error
    }

#endif

// Libdc1394 specific methods
// ------------------------------------------------------------------------
#ifdef WITH_DC1394

    void Camera::createCameraDevice_dc1394(Guid guid)
    { 
        cameraDevicePtr_ = CameraDevicePtr(new CameraDevice_dc1394(guid));
    }

#else

    // Dummy methods for when the library isn't included - allows the ifdefs 
    // to  be limited to two locations.

    void Camera::createCameraDevice_dc1394(Guid guid)
    {
        // TO DO ... throw some kinde of error
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
