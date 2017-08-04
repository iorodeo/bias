#include "camera_info_spin.hpp"
#include <iostream>
#include <sstream>
#include "exception.hpp"

namespace bias {

    const size_t MAX_BUF_LEN = 256;

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
        nodeNameToValueMap_ = retrieveNodeNameToValueMap(hNodeMapTLDevice);
        haveInfo_ = true;

        printNameToValueMap();
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
        std::string guid("not available");
        if (nodeNameToValueMap_.count("DeviceID") != 0)
        {
            guid = nodeNameToValueMap_["DeviceID"];
        }
        return guid;
    }


    std::string CameraInfo_spin::serialNumber() 
    {
        std::string serialNumber("not available");
        if (nodeNameToValueMap_.count("DeviceSerialNumber") != 0)
        {
            serialNumber = nodeNameToValueMap_["DeviceSerialNumber"];
        }
        return serialNumber;
    }

    std::string CameraInfo_spin::modelName() 
    {
        std::string modelName("not available");
        if (nodeNameToValueMap_.count("DeviceModelName") != 0)
        {
            modelName = nodeNameToValueMap_["DeviceModelName"];
        }
        return modelName;
    }


    std::string CameraInfo_spin::vendorName() 
    {
        std::string vendorName("not available");
        if (nodeNameToValueMap_.count("DeviceVendorName") != 0)
        {
            vendorName = nodeNameToValueMap_["DeviceVendorName"];
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
            std::cout << " " << kv.first << " " << kv.second << std::endl;
        }

        std::cout << std::endl;
    }




    // protected methods
    // --------------------------------------------------------------------------------------------

    std::map<std::string, std::string> CameraInfo_spin::retrieveNodeNameToValueMap(spinNodeMapHandle &hNodeMapTLDevice)
    {
        std::map<std::string, std::string> nodeNameToValueMap;
        spinError err = SPINNAKER_ERR_SUCCESS;
        size_t numberOfNodes = retrieveNumberOfNodes(hNodeMapTLDevice);

        for (size_t i=0; i<numberOfNodes; i++) 
        { 
            // Try to get the node handle
            spinNodeHandle hNode = nullptr;
            err = spinNodeMapGetNodeByIndex(hNodeMapTLDevice,i,&hNode);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get this node handle - skip it. 
                continue;
            }

            char nodeNameBuf[MAX_BUF_LEN];
            size_t lenNodeName = MAX_BUF_LEN;
            err = spinNodeGetName(hNode, nodeNameBuf, &lenNodeName);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node name - skip it. 
                continue;
            }
            std::string nodeName = std::string(nodeNameBuf);

            std::cout << nodeName << std::endl;

            bool8_t isAvailable = False;
            err = spinNodeIsAvailable(hNode, &isAvailable);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node availability - skip it. 
                continue;
            }

            bool8_t isReadable = False;
            err = spinNodeIsReadable(hNode, &isReadable);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node readability - skip it. 
                continue;
            }

            char nodeValueBuf[MAX_BUF_LEN];
            size_t lenValueBuf = MAX_BUF_LEN;
            if (isAvailable && isReadable)
            {
                err = spinStringGetValue(hNode, nodeValueBuf, &lenValueBuf);
                if (err != SPINNAKER_ERR_SUCCESS)
                {
                    // Can't get value - skip it. 
                    continue;
                }
            }
            std::string nodeValue = std::string(nodeValueBuf);
            nodeNameToValueMap[nodeName] = nodeValue;
        }

        return nodeNameToValueMap;
    }

    size_t CameraInfo_spin::retrieveNumberOfNodes(spinNodeMapHandle &hNodeMapTLDevice)
    {
        size_t numNodes = 0;
        spinError err = SPINNAKER_ERR_SUCCESS;
        err = spinNodeMapGetNumNodes(hNodeMapTLDevice,&numNodes);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get vendor name readability, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
        }
        return numNodes;
    }

} // namespace bias
