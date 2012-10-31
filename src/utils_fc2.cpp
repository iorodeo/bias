#ifdef WITH_FC2
#include <iostream>
#include "utils_fc2.hpp"

namespace bias {

    std::string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType)
    {
    
        switch (ifaceType) { 
    
            case FC2_INTERFACE_IEEE1394:
                return std::string( "FC2_INTERFACE_IEEE1394" );
                break;
    
            case FC2_INTERFACE_USB_2:
                return std::string( "FC2_INTERFACE_USB_2" );
                break;
    
            case FC2_INTERFACE_USB_3:
                return std::string( "FC2_INTERFACE_USB_3" );
                break;
                
            case FC2_INTERFACE_GIGE:
                return std::string( "FC2_INTERFACE_GIGE" );
                break;
    
            case FC2_INTERFACE_UNKNOWN:
                return std::string( "FC2_INTERFACE_UNKNOWN" );
                break;
                
            default:
                return std::string( "INTERFACE UNEXPECTED VALUE" );
                break;
        }
    }
}
#endif
