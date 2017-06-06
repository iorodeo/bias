#ifdef WITH_SPIN

#include "guid_device_spin.hpp"
#include <sstream>
#include <iostream>

namespace bias {

    GuidDevice_spin::GuidDevice_spin() 
    {
        //for (int i=0; i<4; i++) {
        //    value_.value[i] = 0;
        //}
    }

    //GuidDevice_spin::GuidDevice_spin(spinPGRGuid guid_spin) 
    //{ 
    //    value_ = guid_spin; 
    //}

    //CameraLib GuidDevice_spin::getCameraLib() 
    //{ 
    //    return CAMERA_LIB_SPIN; 
    //}

    //std::string GuidDevice_spin::toString() 
    //{
    //    std::stringstream ss;
    //    for ( int i=0; i<4; i++ ) {
    //        ss << std::hex << value_.value[i];
    //    }
    //    ss << std::dec;
    //    return ss.str();
    //}

    //void GuidDevice_spin::printValue() 
    //{
    //    std::cout << "guid: " << toString() << std::endl;
    //}
    //
    //spinPGRGuid GuidDevice_spin::getValue() { return value_; }

    //bool GuidDevice_spin::isEqual(GuidDevice &guid)
    //{
    //    bool rval = false;
    //    if (getCameraLib() == guid.getCameraLib()) 
    //    { 
    //        rval = true;
    //        GuidDevice_spin *guidPtr = (GuidDevice_spin*) &guid;
    //        spinPGRGuid guid_spin = guidPtr -> getValue(); 

    //        for ( int i=0; i<4; i++ ) {
    //            if ( value_.value[i] != guid_spin.value[i] ) {
    //                rval = false;
    //            }
    //        }
    //    }
    //    return rval;
    //}

    //bool GuidDevice_spin::lessThan(GuidDevice &guid) 
    //{
    //    if (getCameraLib() == guid.getCameraLib())
    //    {
    //        // CameraLibs are the same use guid values
    //        bool rval = false;
    //        GuidDevice_spin *guidPtr = (GuidDevice_spin*) &guid;
    //        spinPGRGuid guid_spin = guidPtr -> getValue();
    //        for (int i=0; i<4; i++) 
    //        {
    //            if (value_.value[i] < guid_spin.value[i]) 
    //            {
    //                rval = true;
    //                break;
    //            }
    //            else if (value_.value[i] > guid_spin.value[i]) 
    //            {
    //                break;

    //            }
    //        }
    //        return rval;
    //    }
    //    else {
    //        // Not the same library  - order by CameraLib Id value
    //        return (getCameraLib() < guid.getCameraLib());
    //    }
    //}

    //bool GuidDevice_spin::lessThanEqual(GuidDevice &guid)
    //{
    //    if (isEqual(guid)) 
    //    {
    //        return true;
    //    }
    //    else
    //    {
    //        return lessThan(guid);
    //    }
    //}
}
    
#endif // #ifdef WITH_SPIN
