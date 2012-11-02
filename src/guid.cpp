#include "guid.hpp"

namespace bias {


    Guid::Guid() 
    { 
        std::shared_ptr<GuidDevice> tempPtr(new GuidDevice());
        guidDevicePtr_ = tempPtr; 
    }

    Guid::~Guid() {};

    CameraLib Guid::getCameraLib() 
    {
        return guidDevicePtr_ -> getCameraLib();
    }

    std::string Guid::toString()
    {
        return guidDevicePtr_ -> toString();
    }

    void Guid::printValue()
    {
        guidDevicePtr_ -> printValue();
        
    }

    // friend functions
    // ------------------------------------------------------------------------
    std::ostream& operator<< (std::ostream &out, Guid &guid)
    {
        out << guid.toString();
        return out;
    }

    bool operator== (Guid &guid0, Guid &guid1) 
    {
        return *(guid0.guidDevicePtr_) == *(guid1.guidDevicePtr_);
    }

    bool operator!= (Guid &guid0, Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) != *(guid1.guidDevicePtr_);
    }

    bool operator<  (Guid &guid0, Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) < *(guid1.guidDevicePtr_);
    }

    bool operator<= (Guid &guid0, Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) <= *(guid1.guidDevicePtr_);
    }

    bool operator>  (Guid &guid0, Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) > *(guid1.guidDevicePtr_);
    }

    bool operator>= (Guid &guid0, Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) >= *(guid1.guidDevicePtr_);
    }

#ifdef WITH_FC2

    // FlyCapture2 specific methods 
    // ------------------------------------------------------------------------

    Guid::Guid(fc2PGRGuid guid)
    {
        std::shared_ptr<GuidDevice_fc2> tempPtr(new GuidDevice_fc2(guid));
        guidDevicePtr_ = tempPtr;
    }

    fc2PGRGuid Guid::getValue_fc2()
    {
        fc2PGRGuid rval;
        if ( getCameraLib() == CAMERA_LIB_FC2 ) 
        {
            std::shared_ptr<GuidDevice_fc2> tempPtr; 
            tempPtr = std::dynamic_pointer_cast<GuidDevice_fc2>(guidDevicePtr_);
            rval = tempPtr -> getValue();
        }
        else 
        {
            for (int i=0; i<4; i++) {
                rval.value[i] = 0;
            }
        }
        return rval;
    }

#endif

#ifdef WITH_DC1394

    // Libdc1394 specific methods 
    // ------------------------------------------------------------------------

    Guid::Guid(uint64_t guid)
    {
        std::shared_ptr<GuidDevice_dc1394> tempPtr(new GuidDevice_dc1394(guid));
        guidDevicePtr_ =  tempPtr;
    }

    uint64_t Guid::getValue_dc1394()
    {
        uint64_t rval;
        if ( getCameraLib() == CAMERA_LIB_DC1394 )
        { 
            std::shared_ptr<GuidDevice_dc1394> tempPtr; 
            tempPtr = std::dynamic_pointer_cast<GuidDevice_dc1394>(guidDevicePtr_);
            rval = tempPtr -> getValue();
        }
        else 
            rval = 0;
        {
        }
        return rval;
    }

#endif

    // Shared pointer comparison operator - for use in sets, maps, etc.
    //-------------------------------------------------------------------------

    bool GuidPtrCmp::operator() (
            const std::shared_ptr<Guid> &guidPtr0, 
            const std::shared_ptr<Guid> &guidPtr1
            ) 
    {
        if (*guidPtr0 == *guidPtr1) {
            return false;
        }
        else {
            bool rval = (*guidPtr0 < *guidPtr1);
            return rval;
        }
    };

} // namespace bias






