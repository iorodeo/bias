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
        vendorName_ = retrieveVendorName(hNodeMapTLDevice);
        numberOfNodes_ = retrieveNumberOfNodes(hNodeMapTLDevice);
        nodeNameToValueMap_ = retrieveNodeNameToValueMap(hNodeMapTLDevice);

        printNameToValueMap();

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

        //ss << " guid:           " << guid() << std::endl;
        //ss << " serial number:  " << serialNumber() << std::endl;
        ss << " camera vendor:  " << vendorName() << std::endl;
        //ss << " camera model:   " << modelName() << std::endl;
        //ss << " sensor          " << std::endl;
        //ss << "   type:         " << sensorInfo() << std::endl;
        //ss << "   resolution:   " << sensorResolution() << std::endl;
        //ss << " firmware        " << std::endl;     
        //ss << "   version:      " << firmwareVersion() << std::endl;
        //ss << "   build time:   " << firmwareBuildTime() << std::endl;
        //ss << " color camera:   " << std::boolalpha << (bool) isColorCamera() << std::endl;
        //ss << " interface:      " << getInterfaceTypeString_spin(interfaceType) << std::endl;
        ss << std::endl;

        ss << "------------------ " << std::endl;
        ss << "Debug              " << std::endl;
        ss << "-------------------" << std::endl;
        ss << std::endl;

        ss << " numNodes:       " << numberOfNodes_ << std::endl;
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


    std::string CameraInfo_spin::serialNumber() 
    {
        return serialNumber_;
    }


    std::string CameraInfo_spin::vendorId() 
    {
        return vendorId_;
    }


    std::string CameraInfo_spin::modelId() 
    {
        return modelId_;
    }


    std::string CameraInfo_spin::modelName() 
    {
        return modelName_;
    }


    std::string CameraInfo_spin::vendorName() 
    {
        return vendorName_;
    }


    // protected methods
    // --------------------------------------------------------------------------------------------

    std::string CameraInfo_spin::retrieveVendorName(spinNodeMapHandle &hNodeMapTLDevice)
    {
        spinError err = SPINNAKER_ERR_SUCCESS;
        spinNodeHandle hDeviceVendorName = nullptr;
        bool8_t deviceVendorNameIsAvailable = False;
        bool8_t deviceVendorNameIsReadable = False;

        // Retrieve vendor name node handle 
        err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceVendorName", &hDeviceVendorName);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            haveInfo_ = false;
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to retrieve vendor name node handle, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
        }

        // Check vendor name availability
        err = spinNodeIsAvailable(hDeviceVendorName, &deviceVendorNameIsAvailable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            haveInfo_ = false;
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get vendor name availability, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
        }

        // Check vendor name readability
        err = spinNodeIsReadable(hDeviceVendorName, &deviceVendorNameIsReadable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            haveInfo_ = false;
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get vendor name readability, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
        }

        // Get vendor name 
        std::string vendorName;
        if (deviceVendorNameIsAvailable && deviceVendorNameIsReadable)
        {
            char deviceVendorName[MAX_BUF_LEN];
            size_t lenDeviceVendorName = MAX_BUF_LEN;
            err = spinStringGetValue(hDeviceVendorName, deviceVendorName, &lenDeviceVendorName);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                haveInfo_ = false;
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to get vendor name readability, error=" << err;
                throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
            }
            vendorName = std::string(deviceVendorName);
        }
        else
        {
            vendorName = std::string("not readable");
        }
        return vendorName;
    } 
    
    std::string CameraInfo_spin::retrieveGuid(spinNodeMapHandle &hNodeMapTLDevice)
    {
        std::string guid;
        return guid;
    }

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


} // namespace bias
