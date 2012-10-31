#include "guid.hpp"

using namespace std;

namespace bias {

    // friend functions
    // ------------------------------------------------------------------------
    ostream& operator<< (ostream &out, Guid &guid)
    {
        out << guid.toString();
        return out;
    }

    Guid::Guid() 
    { 
        shared_ptr<GuidDevice_base> tempPtr( new GuidDevice_base() );
        guidDevicePtr_ = tempPtr; 
    }

    CameraLib Guid::getCameraLib() 
    {
        return guidDevicePtr_ -> getCameraLib();
    }

    string Guid::toString()
    {
        return guidDevicePtr_ -> toString();
    }

    void Guid::printValue()
    {
        guidDevicePtr_ -> printValue();
        
    }

#ifdef WITH_FC2

    // FlyCapture2 specific features
    // ------------------------------------------------------------------------

    Guid::Guid(fc2PGRGuid guid)
    {
        shared_ptr<GuidDevice_fc2> tempPtr( new GuidDevice_fc2(guid) );
        guidDevicePtr_ = tempPtr;
    }

    fc2PGRGuid Guid::getValue_fc2()
    {
        fc2PGRGuid rval;
        if ( getCameraLib() == CAMERA_LIB_FC2 ) 
        {
            shared_ptr<GuidDevice_fc2> tempPtr; 
            tempPtr = dyanamic_pointer_cast<GuidDevice_fc2>(guidDevicePtr_);
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

    // Libdc1394 specific features
    // ------------------------------------------------------------------------

    Guid::Guid(uint64_t guid)
    {
        shared_ptr<GuidDevice_dc1394> tempPtr( new GuidDevice_dc1394(guid) );
        guidDevicePtr_ =  tempPtr;
    }

    uint64_t Guid::getValue_dc1394()
    {
        uint64_t rval;
        if ( getCameraLib() == CAMERA_LIB_DC1394 )
        { 
            shared_ptr<GuidDevice_dc1394> tempPtr; 
            tempPtr = dynamic_pointer_cast<GuidDevice_dc1394>(guidDevicePtr_);
            rval = tempPtr -> getValue();
        }
        else 
            rval = 0;
        {
        }
        return rval;
    }

#endif

} // namespace bias






