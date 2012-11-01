#ifdef WITH_DC1394
#include <sstream>
#include "camera_device_dc1394.hpp"

namespace bias {

    CameraDevice_dc1394::CameraDevice_dc1394(Guid guid) 
        : CameraDevice(guid)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

    }

    CameraLib CameraDevice_dc1394::getCameraLib()
    {
        return guid_.getCameraLib();
    }

    std::string CameraDevice_dc1394::toString()
    {
        std::stringstream ss;
        ss << "camera string for libdc1394 camera" << std::endl;
        return ss.str();
    }

}
#endif 
