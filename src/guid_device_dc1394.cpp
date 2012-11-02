#ifdef WITH_DC1394

#include <sstream>
#include <iostream>
#include "guid_device_dc1394.hpp"

namespace bias {

    GuidDevice_dc1394::GuidDevice_dc1394(uint64_t guid_dc1394)
    {
        value_ = guid_dc1394;
    }

    CameraLib GuidDevice_dc1394::getCameraLib() 
    {
        return CAMERA_LIB_DC1394;
    }

    std::string GuidDevice_dc1394::toString()
    {
        std::stringstream ss;
        ss << std::hex << value_ << std::dec << std::endl;
        return ss.str();
    }

    void GuidDevice_dc1394::printValue() 
    {
        std::cout << "guid: " << toString() << std::endl;
    }

    uint64_t GuidDevice_dc1394::getValue()
    {
        return value_;
    }

    bool GuidDevice_dc1394::isEqual(GuidDevice &guid)
    {
        bool rval = false;
        if ( guid.getCameraLib() != getCameraLib()) 
        {
            GuidDevice_dc1394 *guidPtr = (GuidDevice_dc1394*) &guid;
            rval = (value_ == (guidPtr -> getValue()));
        }

        return rval;
    }

    bool GuidDevice_dc1394::lessThan(GuidDevice &guid) 
    {
        if (guid.getCameraLib() == getCameraLib())
        {
            GuidDevice_dc1394 *guidPtr = (GuidDevice_dc1394*) &guid;
            return (value_ < (guidPtr -> getValue()));
        }
        else {
            return (getCameraLib() < guid.getCameraLib());
        }
    }

    bool GuidDevice_dc1394::lessThanEqual(GuidDevice &guid)
    {
        if (isEqual(guid)) 
        {
            return true;
        }
        else
        {
            return lessThan(guid);
        }
    }

}

#endif // #ifdef WITH_DC1394
