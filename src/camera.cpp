#include "camera.hpp"
#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#endif
#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#endif

namespace bias {


    Camera::Camera() { 
        std::shared_ptr<CameraDevice_base> tempPtr( new CameraDevice_base );
        cameraDevicePtr_ = tempPtr; 
    }

    Camera::Camera(Guid guid) 
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        switch ( guid.getCameraLib() )
        {
            case CAMERA_LIB_FC2:
                std::cout << "  case CAMERA_LIB_FC2" << std::endl;
                createCameraDevice_fc2(guid);
                break;

            case CAMERA_LIB_DC1394:
                std::cout << "  case CAMERA_LIB_DC1394" << std::endl;
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

    void Camera::connect() { cameraDevicePtr_ -> connect(); }

    void Camera::printInfo() { cameraDevicePtr_ -> printInfo(); }

    void Camera::printGuid() { cameraDevicePtr_ -> printGuid(); }

// FlyCapture2 specific methods
// ------------------------------------------------------------------------
#ifdef WITH_FC2

    void Camera::createCameraDevice_fc2(Guid guid)
    { 
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
        std::shared_ptr<CameraDevice_fc2> tempPtr( new CameraDevice_fc2(guid) );
        cameraDevicePtr_ = tempPtr;
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
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::shared_ptr<CameraDevice_dc1394> tempPtr( new CameraDevice_dc1394(guid) );
        cameraDevicePtr_ = tempPtr;
    }

#else

    // Dummy methods for when the library isn't included - allows the ifdefs 
    // to  be limited to two locations.

    void Camera::createCameraDevice_dc1394(Guid guid)
    {
        // TO DO ... throw some kinde of error
    }

#endif

} // namespace bias
