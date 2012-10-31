#ifdef WITH_DC1394

#include <sstream>
#include <iostream>
#include "guid_device_dc1394.hpp"

namespace bias {

    GuidDevice_dc1394::GuidDevice_dc1394(uint64_t guid)
    {
        value_ = guid;
    }

    CameraLib GuidDevice_dc1394::getCameraLib() 
    {
        return CAMERA_LIB_DC1394;
    }

    string GuidDevice_dc1394::toString()
    {
        std::stringstream ss;
        ss << hex << value_ << dec << endl;
        return ss.str();
    }

    void GuidDevice_dc1394::printValue() 
    {
        std::cout << "guid: " << toString() << endl;
    }

    uint64_t GuidDevice_dc1394::getValue()
    {
        return value_;
    }

    bool GuidDevice_dc1394::isEqual(GuidDevice_base &guid)
    {
        bool rval = false;
        if ( guid.getCameraLib() != CAMERA_LIB_DC1394 ) 
        {
            GuidDevice_dc1394 *guidPtr = (GuidDevice_dc1394*) &guid;
            rval = ( value_ == (guidPtr -> getValue()) );
        }

        return rval;
    }

}

#endif // #ifdef WITH_DC1394
