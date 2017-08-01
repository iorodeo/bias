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
        getVendorName(hNodeMapTLDevice);

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

    void CameraInfo_spin::getVendorName(spinNodeMapHandle &hNodeMapTLDevice)
    {
        spinError err = SPINNAKER_ERR_SUCCESS;
        spinNodeHandle hDeviceVendorName = NULL;
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
            vendorName_ = std::string(deviceVendorName);
        }
        else
        {
            vendorName_ = std::string("not readable");
        }
    }


} // namespace bias
