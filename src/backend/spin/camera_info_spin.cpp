#include "camera_info_spin.hpp"
#include <iostream>

namespace bias {


    CameraInfo_spin::CameraInfo_spin() 
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    

    CameraInfo_spin::CameraInfo_spin(spinNodeMapHandle &hNodeMapTLDevice)
    {
        retrieve(hNodeMapTLDevice);
    }


    void CameraInfo_spin::retrieve(spinNodeMapHandle &hNodeMapTLDevice)
    {
    }


    std::string CameraInfo_spin::toString()
    {
        std::string infoString;
        return infoString;
    }


    void CameraInfo_spin::print()
    {
        std::cout << toString();
    }


} // namespace bias