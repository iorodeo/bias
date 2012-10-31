#include <iostream>
#include "camera.hpp"
#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#endif
#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#endif

using namespace std;

namespace bias {


    Camera::Camera() { 
        ///cameraDevicePtr_ = NULL;
        shared_ptr<CameraDevice_base> tempPtr( new CameraDevice_base );
        cameraDevicePtr_ = tempPtr; 
    }

    Camera::Camera(Guid guid) 
    {
        cout << __PRETTY_FUNCTION__ << endl;

        switch ( guid.getCameraLib() )
        {
            case CAMERA_LIB_FC2:
                cout << "  case CAMERA_LIB_FC2" << endl;
                createCameraDevice_fc2(guid);
                break;

            case CAMERA_LIB_DC1394:
                cout << "  case CAMERA_LIB_DC1394" << endl;
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
        cout << __PRETTY_FUNCTION__ << endl;
        //cameraDevice_fc2_ = CameraDevice_fc2(guid); 
        //cameraDevicePtr_ = (CameraDevice_base*) &cameraDevice_fc2_;
        shared_ptr<CameraDevice_fc2> tempPtr( new CameraDevice_fc2(guid) );
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
        cout << __PRETTY_FUNCTION__ << endl;
        //cameraDevice_dc1394_ = CameraDevice_dc1394(guid); 
        //cameraDevicePtr_ = (CameraDevice_base*) &cameraDevice_dc1394_;
        shared_ptr<CameraDevice_dc1394> tempPtr( new CameraDevice_dc1394(guid) );
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
