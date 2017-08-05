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
        //std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    

    CameraInfo_spin::CameraInfo_spin(spinNodeMapHandle &hNodeMapTLDevice)
    {
        retrieve(hNodeMapTLDevice);
    }


    void CameraInfo_spin::retrieve(spinNodeMapHandle &hNodeMapTLDevice)
    {
        nodeNameToValueMap_ = retrieveNodeNameToValueMap(hNodeMapTLDevice);
        //printNameToValueMap();
        haveInfo_ = true;
    }

    bool CameraInfo_spin::haveInfo()
    {
        return haveInfo_;
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
        std::string guid("no information");
        if (nodeNameToValueMap_.count("DeviceID") != 0)
        {
            guid = nodeNameToValueMap_["DeviceID"];
        }
        else
        {
            guid = std::string("not available");
        }
        return guid;
    }


    std::string CameraInfo_spin::serialNumber() 
    {
        std::string serialNumber("no information");
        if (nodeNameToValueMap_.count("DeviceSerialNumber") != 0)
        {
            serialNumber = nodeNameToValueMap_["DeviceSerialNumber"];
        }
        else
        {
            serialNumber = std::string("not available");
        }
        return serialNumber;
    }

    std::string CameraInfo_spin::modelName() 
    {
        std::string modelName("no information");
        if (nodeNameToValueMap_.count("DeviceModelName") != 0)
        {
            modelName = nodeNameToValueMap_["DeviceModelName"];
        }
        else
        {
            modelName = std::string("not available");
        }
        return modelName;
    }


    std::string CameraInfo_spin::vendorName() 
    {
        std::string vendorName("no information");
        if (nodeNameToValueMap_.count("DeviceVendorName") != 0)
        {
            vendorName = nodeNameToValueMap_["DeviceVendorName"];
        }
        else
        {
            vendorName = std::string("not available");
        }
        return vendorName;
    }


    void CameraInfo_spin::printNameToValueMap() 
    {
        std::cout << std::endl;
        std::cout << "------------------ " << std::endl;
        std::cout << "nodeNameToValueMap " << std::endl;
        std::cout << "------------------ " << std::endl;
        std::cout << std::endl;

        for (auto &kv : nodeNameToValueMap_)
        {
            //std::cout << " " << kv.first << " " << kv.second << std::endl;
            std::cout << " " << kv.first << " " << kv.second << " " << nodeNameToValueMap_.count("DeviceVendorName") << std::endl;
        }

        std::cout << std::endl;
    }


    // protected methods
    // --------------------------------------------------------------------------------------------

    std::map<std::string, std::string> CameraInfo_spin::retrieveNodeNameToValueMap(spinNodeMapHandle &hNodeMapTLDevice)
    {
        return getNodeNameToStringValueMap(hNodeMapTLDevice);
    }

    size_t CameraInfo_spin::retrieveNumberOfNodes(spinNodeMapHandle &hNodeMapTLDevice)
    {
        return getNumberOfNodes(hNodeMapTLDevice);
    }

} // namespace bias
#endif
