#ifdef WITH_SPIN

#include "guid_device_spin.hpp"
#include <sstream>
#include <iostream>

namespace bias {

    GuidDevice_spin::GuidDevice_spin() 
    {
        value_ = std::string("-1");
    }

    GuidDevice_spin::GuidDevice_spin(std::string guid_spin) 
    { 
        value_ = guid_spin; 
    }

    CameraLib GuidDevice_spin::getCameraLib() 
    { 
        return CAMERA_LIB_SPIN; 
    }

    std::string GuidDevice_spin::toString() 
    {
        return value_;
    }

    void GuidDevice_spin::printValue() 
    {
        std::cout << "guid: " << toString() << std::endl;
    }
    
    std::string GuidDevice_spin::getValue() 
    { 
        return value_; 
    }

    bool GuidDevice_spin::isEqual(GuidDevice &guid)
    {
        bool rval = false;
        if (value_.compare(guid.toString()) == 0)
        {
            rval = true;
        }
        return rval;
    }

    bool GuidDevice_spin::lessThan(GuidDevice &guid) 
    {
        bool rval = false;
        if (value_.compare(guid.toString()) < 0)
        {
            rval = true;
        }
        return rval;
    }

    bool GuidDevice_spin::lessThanEqual(GuidDevice &guid)
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
    
#endif // #ifdef WITH_SPIN
