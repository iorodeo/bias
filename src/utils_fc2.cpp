#ifdef WITH_FC2
#include <iostream>
#include "utils_fc2.hpp"

namespace bias {

    string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType)
    {
    
        switch (ifaceType) { 
    
            case FC2_INTERFACE_IEEE1394:
                return string( "FC2_INTERFACE_IEEE1394" );
                break;
    
            case FC2_INTERFACE_USB_2:
                return string( "FC2_INTERFACE_USB_2" );
                break;
    
            case FC2_INTERFACE_USB_3:
                return string( "FC2_INTERFACE_USB_3" );
                break;
                
            case FC2_INTERFACE_GIGE:
                return string( "FC2_INTERFACE_GIGE" );
                break;
    
            case FC2_INTERFACE_UNKNOWN:
                return string( "FC2_INTERFACE_UNKNOWN" );
                break;
                
            default:
                return string( "INTERFACE UNEXPECTED VALUE" );
                break;
        }
    }
}
#endif
