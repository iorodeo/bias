#include "guid.hpp"

namespace bias {


    Guid::Guid() 
    { 
        guidDevicePtr_ = GuidDevicePtr(new GuidDevice()); 
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

    bool operator== (const Guid &guid0, const Guid &guid1) 
    {
        return *(guid0.guidDevicePtr_) == *(guid1.guidDevicePtr_);
    }

    bool operator!= (const Guid &guid0, const Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) != *(guid1.guidDevicePtr_);
    }

    bool operator<  (const Guid &guid0, const Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) < *(guid1.guidDevicePtr_);
    }

    bool operator<= (const Guid &guid0, const Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) <= *(guid1.guidDevicePtr_);
    }

    bool operator>  (const Guid &guid0, const Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) > *(guid1.guidDevicePtr_);
    }

    bool operator>= (const Guid &guid0, const Guid &guid1)
    {
        return *(guid0.guidDevicePtr_) >= *(guid1.guidDevicePtr_);
    }

#ifdef WITH_FC2

    // FlyCapture2 specific methods 
    // ------------------------------------------------------------------------

    Guid::Guid(fc2PGRGuid guid)
    {
        guidDevicePtr_ = std::make_shared<GuidDevice_fc2>(guid);
    }

    fc2PGRGuid Guid::getValue_fc2()
    {
        fc2PGRGuid rval;
        if ( getCameraLib() == CAMERA_LIB_FC2 ) 
        {
            GuidDevicePtr_fc2 tempPtr; 
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

    Guid::Guid(uint64_t guidInt)
    {
        guidDevicePtr_ = std::make_shared<GuidDevice_dc1394>(guidInt);
    }

    uint64_t Guid::getValue_dc1394()
    {
        uint64_t rval;
        if ( getCameraLib() == CAMERA_LIB_DC1394 )
        { 
            GuidDevicePtr_dc1394 tempPtr; 
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

#ifdef WITH_SPIN

    // Spinnaker specific methods
    // ------------------------------------------------------------------------
    Guid::Guid(std::string guidStr)
    {
        guidDevicePtr_ = std::make_shared<GuidDevice_spin>(guidStr);
    };

    std::string Guid::getValue_spin()
    {
        std::string rval;
        return rval;
    };

#endif
    
    // Guid comparison operator
    // ------------------------------------------------------------------------
    bool GuidCmp::operator() (const Guid &guid0, const Guid &guid1)
    {
        if (guid0 == guid1) {
            return false;
        }
        else 
        {
            return (guid0 < guid1);
        }
    }

    // Shared pointer comparison operator - for use in sets, maps, etc.
    //-------------------------------------------------------------------------

    bool GuidPtrCmp::operator() (const GuidPtr &guidPtr0, const GuidPtr &guidPtr1) 
    {
        if (*guidPtr0 == *guidPtr1) 
        {
            return false;
        }
        else 
        {
            return (*guidPtr0 < *guidPtr1);
        }
    };

} // namespace bias






