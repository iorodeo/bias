#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#include "exception.hpp"
#include <sstream>

namespace bias {

    CameraDevice_dc1394::CameraDevice_dc1394() : CameraDevice()
    {
        context_ = NULL;
    }

    CameraDevice_dc1394::CameraDevice_dc1394(Guid guid) : CameraDevice(guid)
    {
        context_ = NULL;
        context_ = dc1394_new();
        if ( !context_ ) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error creating dc1394_t" << std::endl;
            throw RuntimeError(ERROR_DC1394_CREATE_CONTEXT, ssError.str());
        }

    }

    CameraDevice_dc1394::~CameraDevice_dc1394()
    {
        dc1394_free(context_);
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
