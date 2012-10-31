#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"

namespace bias {

    CameraDevice_dc1394::CameraDevice_dc1394(Guid guid) 
        : CameraDevice_base(guid)
    {
        cout << __PRETTY_FUNCTION__ << endl;

    }

    CameraLib CameraDevice_dc1394::getCameraLib()
    {
        return guid_.getCameraLib();
    }


}
#endif 
