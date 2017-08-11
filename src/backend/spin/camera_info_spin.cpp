#ifdef WITH_SPIN
#include "camera_info_spin.hpp"
#include <iostream>
#include <sstream>
#include "exception.hpp"
#include "node_map_utils.hpp"

namespace bias {

    const size_t MAX_BUF_LEN = 256;

    CameraInfo_spin::CameraInfo_spin() 
    {

    }
    
    std::string CameraInfo_spin::toString()
    {
        std::stringstream ss; 

        ss << std::endl;
        ss << "------------------ " << std::endl;
        ss << "Camera Information " << std::endl;
        ss << "------------------ " << std::endl;
        ss << std::endl;

        ss << " guid:           " << guid() << std::endl;
        ss << " serial number:  " << serialNumber() << std::endl;
        ss << " camera vendor:  " << vendorName() << std::endl;
        ss << " camera model:   " << modelName() << std::endl;
        ss << std::endl;
            
        return ss.str();
    }


    void CameraInfo_spin::print()
    {
        std::cout << toString();
    }



    std::string CameraInfo_spin::guid() 
    {
        return guid_;
    }


    void CameraInfo_spin::setGuid(std::string guid)
    {
        guid_ = guid;
    }


    std::string CameraInfo_spin::serialNumber() 
    {
        return serialNumber_;
    }

   
    void CameraInfo_spin::setSerialNumber(std::string serialNumber)
    {
        serialNumber_ = serialNumber;
    } 


    std::string CameraInfo_spin::vendorId()
    {
        return vendorId_;
    }


    void CameraInfo_spin::setVendorId(std::string vendorId)
    {
        vendorId_ = vendorId;
    }


    std::string CameraInfo_spin::modelId()
    {
        return modelId_;
    }


    void CameraInfo_spin::setModelId(std::string modelId)
    {
        modelId_ = modelId;
    }


    std::string CameraInfo_spin::modelName() 
    {
        return modelName_;
    }


    void CameraInfo_spin::setModelName(std::string modelName)
    {
        modelName_ = modelName;
    }


    std::string CameraInfo_spin::vendorName() 
    {
        return vendorName_;
    }


    void CameraInfo_spin::setVendorName(std::string vendorName)
    {
        vendorName_ = vendorName;
    }



} // namespace bias
#endif
