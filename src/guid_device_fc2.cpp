#ifdef WITH_FC2

#include "guid_device_fc2.hpp"
#include <sstream>
#include <iostream>

namespace bias {

    GuidDevice_fc2::GuidDevice_fc2() 
    {
        for (int=0; i<4; i++) {
            value_.value[i] = 0;
        }
    }

    GuidDevice_fc2::GuidDevice_fc2(fc2PGRGuid guid_fc2) 
    { 
        value_ = guid_fc2; 
    }

    CameraLib GuidDevice_fc2::getCameraLib() 
    { 
        return CAMERA_LIB_FC2; 
    }

    std::string GuidDevice_fc2::toString() 
    {
        std::stringstream ss;
        for ( int i=0; i<4; i++ ) {
            ss << std::hex << value_.value[i];
        }
        ss << std::dec;
        return ss.str();
    }

    void GuidDevice_fc2::printValue() 
    {
        std::cout << "guid: " << toString() << std::endl;
    }
    
    fc2PGRGuid GuidDevice_fc2::getValue() { return value_; }

    bool GuidDevice_fc2::isEqual(GuidDevice &guid)
    {
        bool rval = false;
        if (getCameraLib() == guid.getCameraLib()) 
        { 
            rval = true;
            GuidDevice_fc2 *guidPtr = (GuidDevice_fc2*) &guid;
            fc2PGRGuid guid_fc2 = guidPtr -> getValue(); 

            for ( int i=0; i<4; i++ ) {
                if ( value_.value[i] != guid_fc2.value[i] ) {
                    rval = false;
                }
            }
        }
        return rval;
    }

    bool GuidDevice_fc2::lessThan(GuidDevice &guid) 
    {
        if (getCameraLib() == guid.getCameraLib())
        {
            // CameraLibs are the same use guid values
            bool rval = false;
            GuidDevice_fc2 *guidPtr = (GuidDevice_fc2*) &guid;
            fc2PGRGuid guid_fc2 = guidPtr -> getValue();
            for (int i=0; i<4; i++) 
            {
                if (value_.value[i] < guid_fc2.value[i]) 
                {
                    rval = true;
                    break;
                }
                else if (value_.value[i] > guid_fc2.value[i]) 
                {
                    break;

                }
            }
            return rval;
        }
        else {
            // Not the same library  - order by CameraLib Id value
            return (getCameraLib() < guid.getCameraLib());
        }
    }

    bool GuidDevice_fc2::lessThanEqual(GuidDevice &guid)
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
    
#endif // #ifdef WITH_FC2
