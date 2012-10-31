#ifdef WITH_FC2

#include <sstream>
#include <iostream>
#include "guid_device_fc2.hpp"

namespace bias {

    GuidDevice_fc2::GuidDevice_fc2(fc2PGRGuid guid) 
    { 
        value_ = guid; 
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

    bool GuidDevice_fc2::isEqual(GuidDevice_base &guid)
    {
        bool rval = false;
        GuidDevice_fc2 *guidPtr;

        fc2PGRGuid guid_fc2; 

        if ( guid.getCameraLib() == CAMERA_LIB_FC2) 
        { 
            rval = true;
            GuidDevice_fc2 *guidPtr = (GuidDevice_fc2*) &guid;
            guid_fc2 = guidPtr -> getValue(); 

            for ( int i=0; i<4; i++ ) {
                if ( value_.value[i] != guid_fc2.value[i] ) {
                    rval = false;
                }
            }
        }
        return rval;
    }
}
    
#endif // #ifdef WITH_FC2
