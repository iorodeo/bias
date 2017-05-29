#include <iostream>
#include <string>
#include "SpinnakerC.h"

#define MAX_BUFF_LEN 256

int main(int argc, char *argv[])
{
    std::cout << "running: " << __FILE__ << std::endl;

	spinError err = SPINNAKER_ERR_SUCCESS;
	spinSystem hSystem = NULL;

    // Get System instance
	err = spinSystemGetInstance(&hSystem);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to retrieve system instance: " <<  err << std::endl;
		return err;
	}

	//spinInterfaceList hInterfaceList = NULL;
	//size_t numInterfaces = 0;

	//// Create empty interface list 
	//err = spinInterfaceListCreateEmpty(&hInterfaceList);
	//if (err != SPINNAKER_ERR_SUCCESS)
	//{
    //    std::cout << "unable to create empty interface list: " <<  err << std::endl;
	//	return err;
	//}

    //// Retrieve interfaces from system
	//err = spinSystemGetInterfaces(hSystem, hInterfaceList);
	//if (err != SPINNAKER_ERR_SUCCESS)
	//{
	//    std::cout << "unable to retrieve interface list: " <<  err << std::endl;
	//	return err;
	//}

    //// Get number of interfaces
	//err = spinInterfaceListGetSize(hInterfaceList, &numInterfaces);
	//if (err != SPINNAKER_ERR_SUCCESS)
	//{
    //    std::cout << "unable to retrieve number of interfaces: " << err << std::endl;
	//	return err;
	//}
    //std::cout << "number of interfaces found: " << numInterfaces << std::endl;


    // Get camera list and number of cameras
	spinCameraList hCameraList = NULL;
	size_t numCameras = 0;

	// Create empty camera list
	err = spinCameraListCreateEmpty(&hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to create camera list: " << err << std::endl;
		return err;
	}

	// Retrieve cameras from system
	err = spinSystemGetCameras(hSystem, hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to retrieve camera list: " <<  err << std::endl;
		return err;
	}

	// Retrieve number of cameras
	err = spinCameraListGetSize(hCameraList, &numCameras);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to retrieve number of cameras: " << err << std::endl;
		return err;
	}
    std::cout << "number of cameras found: " << numCameras << std::endl;

    for (int i=0; i<numCameras; i++) 
    {
        std::cout << "testing camera[" << i << "]" << std::endl; 

        spinCamera hCam = NULL;

		err = spinCameraListGet(hCameraList, i, &hCam);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to retrieve camera[" << i << "]: " << err << std::endl;
			return err;
		}

		spinNodeMapHandle hNodeMapTLDevice = NULL;

		err = spinCameraGetTLDeviceNodeMap(hCam, &hNodeMapTLDevice);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to retrieve TL device nodemap: " << err << std::endl;
			return err;
		}

		spinNodeHandle hDeviceVendorName = NULL;
		bool8_t deviceVendorNameIsAvailable = False;
		bool8_t deviceVendorNameIsReadable = False;

		// Retrieve vendor name node handle 
		err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceVendorName", &hDeviceVendorName);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to retrieve device information (vendor name node): " << err << std::endl;
			return err;
		}
        std::cout << "got device vendor name handle" << std::endl;

		// Check vendor name availability
		err = spinNodeIsAvailable(hDeviceVendorName, &deviceVendorNameIsAvailable);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to check node availability (vendor name node): " <<  err << std::endl;
			return err;
		}
        std::cout << "vendor name is available" << std::endl;

		// Check vendor name readability
		err = spinNodeIsReadable(hDeviceVendorName, &deviceVendorNameIsReadable);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to check node readability (vendor name node): " << err << std::endl;
			return err;
		}
        std::cout << "vendor name is readable" << std::endl;


		spinNodeHandle hDeviceModelName = NULL;
		bool8_t deviceModelNameIsAvailable = False;
		bool8_t deviceModelNameIsReadable = False;

        // Retrieve mode name node handle
		err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceModelName", &hDeviceModelName);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to retrieve device information (model name node): " << err << std::endl;
			return err;
		}
        std::cout << "got device model name handle" << std::endl;

        // Check if model name is avialable
		err = spinNodeIsAvailable(hDeviceModelName, &deviceModelNameIsAvailable);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to check node availability (model name node): " << err << std::endl;
			return err;
		}
        std::cout << "model name available" << std::endl;

        // Check if model name is readable
		err = spinNodeIsReadable(hDeviceModelName, &deviceModelNameIsReadable);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to check node readability (model name node): " << err << std::endl;
			return err;
        }
        std::cout << "model name readable" << std::endl;



        std::string vendorNameStr;
        std::string modelNameStr;

		// Print device vendor name
		if (deviceVendorNameIsAvailable && deviceVendorNameIsReadable)
		{
		    char deviceVendorName[MAX_BUFF_LEN];
		    size_t lenDeviceVendorName = MAX_BUFF_LEN;
			err = spinStringGetValue(hDeviceVendorName, deviceVendorName, &lenDeviceVendorName);
			if (err != SPINNAKER_ERR_SUCCESS)
			{
                std::cout << "unable to retrieve device information (vendor name value):  " << err << std::endl;
				return err;
			}
            vendorNameStr = std::string(deviceVendorName);
		}
		else
		{
            vendorNameStr = std::string("not readable");
		}
        std::cout << "vendor: " << vendorNameStr << std::endl;

		// Print device model name
		if (deviceModelNameIsAvailable && deviceModelNameIsReadable)
		{
		    char deviceModelName[MAX_BUFF_LEN];
		    size_t lenDeviceModelName = MAX_BUFF_LEN;
			err = spinStringGetValue(hDeviceModelName, deviceModelName, &lenDeviceModelName);
			if (err != SPINNAKER_ERR_SUCCESS)
			{
				printf("Unable to retrieve device information (model name value). Aborting with error %d...\n\n", err);
				return err;
			}
            modelNameStr = std::string(deviceModelName);
		}
		else
		{
            modelNameStr = std::string("not readable");
		}
        std::cout << "model: " << modelNameStr << std::endl;



		err = spinCameraRelease(hCam);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
            std::cout << "unable to release camera[" << i << "]: " << err << std::endl;
			return err;
		}

    }

    
    // Clear and Destroy camera list
	err = spinCameraListClear(hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to clear camera list: " << err << std::endl;
		return err;
	}

	err = spinCameraListDestroy(hCameraList);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
        std::cout << "unable to destroy camera list: " << err << std::endl;
		return err;
	}


    //// Clear and destroy interface list
	//err = spinInterfaceListClear(hInterfaceList);
	//if (err != SPINNAKER_ERR_SUCCESS)
	//{
    //    std::cout << "unable to clear interface list: " << err << std::endl;
	//	return err;
	//}

	//err = spinInterfaceListDestroy(hInterfaceList);
	//if (err != SPINNAKER_ERR_SUCCESS)
	//{
    //    std::cout << "unable to destroy interface list: " << err << std::endl;
	//	return err;
	//}

    //Release System instance
	err = spinSystemReleaseInstance(hSystem);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
	    std::cout << "unable to release system instance: " <<  err << std::endl;
		return err;
	}


    return 0;
}
