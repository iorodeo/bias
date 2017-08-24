#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "camera_facade.hpp"
#include "camera_device_spin.hpp"


int main(int argc, char *argv[]) 
{

    bias::CameraFinder camFinder;

    bias::GuidList guidList = camFinder.getGuidList();

    std::cout << std::endl;

    for (auto guid : guidList) 
    {
        std::cout << "guid:       " << guid.toString() << std::endl;
        bias::CameraDevice_spin camDev(guid);

        bias::CameraLib camLib = camDev.getCameraLib();
        std::cout << "camLib:     "  << camLib << std::endl;

        camDev.connect();
        std::cout << "modelName:  "  << camDev.getModelName() << std::endl;
        std::cout << "vendorName: "  << camDev.getVendorName() << std::endl;


        bool isColor = camDev.isColor();
        std::cout << "isColor: " << isColor << std::endl;
        //camDev.startCapture();
        //cv::namedWindow("preview", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED); 

        //int cnt = 0;
        //while (cnt < 10)
        //{

        //    cv::Mat image = camDev.grabImage();
        //    if (image.empty())
        //    {
        //        continue;
        //    }
        //    else
        //    {
        //        cv::imshow("preview", image);
        //        cv::waitKey(1);
    
        //        cnt++;
        //        std::cout << "cnt:  " << cnt << std::endl;
        //        std::cout << "cols: " << image.cols << std::endl;
        //        std::cout << "rows: " << image.rows << std::endl;
        //        std::cout << std::endl;
        //    }
        //}


        //camDev.stopCapture();


        camDev.disconnect();

        std::cout << std::endl;
    };



    return 0;
}


//#include "SpinnakerC.h"

//#define MAX_BUFF_LEN 256
//const int NumFramesToAcquire = 50;
//
//spinError runSpinCamera(spinCamera hCam);
//spinError printSpinCameraInfo(spinCamera hCam);
//
//int main(int argc, char *argv[])
//{
//    std::cout << "running: " << __FILE__ << std::endl;
//
//	spinError err = SPINNAKER_ERR_SUCCESS;
//	spinSystem hSystem = NULL;
//
//    // Get System instance
//	err = spinSystemGetInstance(&hSystem);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to retrieve system instance: " <<  err << std::endl;
//		return err;
//	}
//
//    // Get camera list and number of cameras
//	spinCameraList hCameraList = NULL;
//	size_t numCameras = 0;
//
//	// Create empty camera list
//	err = spinCameraListCreateEmpty(&hCameraList);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to create camera list: " << err << std::endl;
//		return err;
//	}
//
//	// Retrieve cameras from system
//	err = spinSystemGetCameras(hSystem, hCameraList);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to retrieve camera list: " <<  err << std::endl;
//		return err;
//	}
//
//	// Retrieve number of cameras
//	err = spinCameraListGetSize(hCameraList, &numCameras);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to retrieve number of cameras: " << err << std::endl;
//		return err;
//	}
//    std::cout << "number of cameras found: " << numCameras << std::endl;
//
//    for (int i=0; i<numCameras; i++) 
//    {
//        std::cout << "testing camera[" << i << "]" << std::endl; 
//
//        spinCamera hCam = NULL;
//
//		err = spinCameraListGet(hCameraList, i, &hCam);
//		if (err != SPINNAKER_ERR_SUCCESS)
//		{
//            std::cout << "unable to retrieve camera[" << i << "]: " << err << std::endl;
//			return err;
//		}
//
//        err = printSpinCameraInfo(hCam);
//        if (err != SPINNAKER_ERR_SUCCESS)
//        {
//            std::cout << "unable to print camera info: " << err << std::cout;
//            return err;
//        }
//
//        err = runSpinCamera(hCam);
//        if (err != SPINNAKER_ERR_SUCCESS) 
//        {
//            std::cout << "unable to run camera: " << err << std::cout;
//            return err;
//        }
//
//
//		err = spinCameraRelease(hCam);
//		if (err != SPINNAKER_ERR_SUCCESS)
//		{
//            std::cout << "unable to release camera[" << i << "]: " << err << std::endl;
//			return err;
//		}
//    } 
//
//    
//    // Clear and Destroy camera list
//	err = spinCameraListClear(hCameraList);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to clear camera list: " << err << std::endl;
//		return err;
//	}
//
//	err = spinCameraListDestroy(hCameraList);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to destroy camera list: " << err << std::endl;
//		return err;
//	}
//
//
//    //Release System instance
//	err = spinSystemReleaseInstance(hSystem);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//	    std::cout << "unable to release system instance: " <<  err << std::endl;
//		return err;
//	}
//
//
//    return 0;
//}
//
//spinError runSpinCamera(spinCamera hCam)
//{
//	spinError err = SPINNAKER_ERR_SUCCESS;
//	spinNodeMapHandle hNodeMapTLDevice = NULL;
//
//	// Initialize camera
//	err = spinCameraInit(hCam);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to initialize camera: " << err << std::endl;
//		return err;
//	}
//
//	// Retrieve GenICam nodemap
//	spinNodeMapHandle hNodeMap = NULL;
//	err = spinCameraGetNodeMap(hCam, &hNodeMap);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to retrieve GenICam nodemap: " << err << std::endl;
//		return err;
//	}
//
//	spinNodeHandle hAcquisitionMode = NULL;
//	spinNodeHandle hAcquisitionModeContinuous = NULL;
//	int64_t acquisitionModeContinuous = 0;
//
//	// Retrieve enumeration node from nodemap
//	err = spinNodeMapGetNode(hNodeMap, "AcquisitionMode", &hAcquisitionMode);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//	    std::cout << "unable to set acquisition mode to continuous (node retrieval): " << err << std::endl;
//		return err;
//	}
//
//	// Retrieve entry node from enumeration node
//	err = spinEnumerationGetEntryByName(hAcquisitionMode, "Continuous", &hAcquisitionModeContinuous);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to set acquisition mode to continuous (entry 'continuous' retrieval): " << err << std::endl;
//		return err;
//	}
//
//	// Retrieve integer from entry node
//	err = spinEnumerationEntryGetIntValue(hAcquisitionModeContinuous, &acquisitionModeContinuous);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to set acquisition mode to continuous (entry int value retrieval): " << err << std::endl;
//		return err;
//	}
//
//	// Set integer as new value of enumeration node
//	err = spinEnumerationSetIntValue(hAcquisitionMode, acquisitionModeContinuous);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to set acquisition mode to continuous (entry int value setting): " << err << std::endl;
//		return err;
//	}
//    std::cout << "acquisition mode is set to continuous" << std::endl;
//
//    // Begin acquiring images
//	err = spinCameraBeginAcquisition(hCam);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to begin image acquisition: " << err << std::endl;
//		return err;
//	}
//    std::cout << "acquiring images" << std::endl;
//
//    int frameCnt = 0;
//    while (frameCnt < NumFramesToAcquire)
//    {
//		spinImage hResultImage = NULL;
//
//        // Get image from camera
//		err = spinCameraGetNextImageEx(hCam, 0, &hResultImage);
//		if (err != SPINNAKER_ERR_SUCCESS)
//		{
//		    std::cout << "unable to get next image: " << err << std::endl;
//			continue;
//		}
//        std::cout << "frame count: " << frameCnt << std::endl;
//        frameCnt++;
//
//        // Check image
//		bool8_t isIncomplete = False;
//		bool haveImage = true;
//
//		err = spinImageIsIncomplete(hResultImage, &isIncomplete);
//		if (err != SPINNAKER_ERR_SUCCESS)
//		{
//            std::cout << "unable to determine image completion: " << err << std::endl; 
//            haveImage = false;
//		}
//
//		// Check image for completion
//		if (isIncomplete)
//		{
//			spinImageStatus imageStatus = IMAGE_NO_ERROR;
//			err = spinImageGetStatus(hResultImage, &imageStatus);
//			if (err != SPINNAKER_ERR_SUCCESS)
//			{
//                std::cout << "unable to retrieve image status: " << err << std::endl; 
//			}
//			else
//			{
//                std::cout << "image incomplete with image status: " << imageStatus << std::endl;
//			}
//            haveImage = false;
//		}
//
//        if (haveImage) 
//        {
//            std::cout << "image OK" << std::endl;
//
//		    // Retrieve image width
//		    size_t width = 0;
//		    err = spinImageGetWidth(hResultImage, &width);
//		    if (err != SPINNAKER_ERR_SUCCESS)
//		    {
//                std::cout << "error getting image width: " << err << std::endl;
//		    }
//		    else
//		    {
//                std::cout << "width: " << width << std::endl;
//		    }
//
//		    // Retrieve image height
//		    size_t height = 0; 
//		    err = spinImageGetHeight(hResultImage, &height);
//		    if (err != SPINNAKER_ERR_SUCCESS)
//		    {
//                std::cout << "error getting image height: " << err << std::endl;
//		    }
//		    else
//		    {
//                std::cout << "height: " << height << std::endl;
//		    }
//        }
//
//
//        // Release image
//		err = spinImageRelease(hResultImage);
//		if (err != SPINNAKER_ERR_SUCCESS)
//		{
//            std::cout << "unable to release image: " << err << std::endl;
//		}
//    }
//
//    // End image acquisition
//	err = spinCameraEndAcquisition(hCam);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        std::cout << "unable to end acquisition: " <<  err << std::endl;
//	}
//    std::cout << "image acquisition ended" << std::endl;
//
//	// Deinitialize camera
//	err = spinCameraDeInit(hCam);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//	    std::cout << "unable to deinitialize camera: " << err << std::endl;
//		return err;
//	}
//
//    return SPINNAKER_ERR_SUCCESS;
//}
//
//
//spinError printSpinCameraInfo(spinCamera hCam)
//{
//	spinError err = SPINNAKER_ERR_SUCCESS;
//    spinNodeMapHandle hNodeMapTLDevice = NULL;
//
//    err = spinCameraGetTLDeviceNodeMap(hCam, &hNodeMapTLDevice);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to retrieve TL device nodemap: " << err << std::endl;
//        return err;
//    }
//
//    spinNodeHandle hDeviceVendorName = NULL;
//    bool8_t deviceVendorNameIsAvailable = False;
//    bool8_t deviceVendorNameIsReadable = False;
//
//    // Retrieve vendor name node handle 
//    err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceVendorName", &hDeviceVendorName);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to retrieve device information (vendor name node): " << err << std::endl;
//        return err;
//    }
//
//    // Check vendor name availability
//    err = spinNodeIsAvailable(hDeviceVendorName, &deviceVendorNameIsAvailable);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to check node availability (vendor name node): " <<  err << std::endl;
//        return err;
//    }
//
//    // Check vendor name readability
//    err = spinNodeIsReadable(hDeviceVendorName, &deviceVendorNameIsReadable);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to check node readability (vendor name node): " << err << std::endl;
//        return err;
//    }
//
//
//    spinNodeHandle hDeviceModelName = NULL;
//    bool8_t deviceModelNameIsAvailable = False;
//    bool8_t deviceModelNameIsReadable = False;
//
//    // Retrieve mode name node handle
//    err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceModelName", &hDeviceModelName);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to retrieve device information (model name node): " << err << std::endl;
//        return err;
//    }
//
//    // Check if model name is avialable
//    err = spinNodeIsAvailable(hDeviceModelName, &deviceModelNameIsAvailable);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to check node availability (model name node): " << err << std::endl;
//        return err;
//    }
//
//    // Check if model name is readable
//    err = spinNodeIsReadable(hDeviceModelName, &deviceModelNameIsReadable);
//    if (err != SPINNAKER_ERR_SUCCESS)
//    {
//        std::cout << "unable to check node readability (model name node): " << err << std::endl;
//        return err;
//    }
//
//    // Get vendor name 
//    std::string vendorNameStr;
//    if (deviceVendorNameIsAvailable && deviceVendorNameIsReadable)
//    {
//        char deviceVendorName[MAX_BUFF_LEN];
//        size_t lenDeviceVendorName = MAX_BUFF_LEN;
//        err = spinStringGetValue(hDeviceVendorName, deviceVendorName, &lenDeviceVendorName);
//        if (err != SPINNAKER_ERR_SUCCESS)
//        {
//            std::cout << "unable to retrieve device information (vendor name value):  " << err << std::endl;
//            return err;
//        }
//        vendorNameStr = std::string(deviceVendorName);
//    }
//    else
//    {
//        vendorNameStr = std::string("not readable");
//    }
//
//    // Get model name
//    std::string modelNameStr;
//    if (deviceModelNameIsAvailable && deviceModelNameIsReadable)
//    {
//        char deviceModelName[MAX_BUFF_LEN];
//        size_t lenDeviceModelName = MAX_BUFF_LEN;
//        err = spinStringGetValue(hDeviceModelName, deviceModelName, &lenDeviceModelName);
//        if (err != SPINNAKER_ERR_SUCCESS)
//        {
//            std::cout << "unable to retrieve device information (model name value): " << err << std::endl;
//            return err;
//        }
//        modelNameStr = std::string(deviceModelName);
//    }
//    else
//    {
//        modelNameStr = std::string("not readable");
//    }
//
//    // Get serial number
//    std::string serialNumberStr;
//	spinNodeHandle hDeviceSerialNumber = NULL;
//	err = spinNodeMapGetNode(hNodeMapTLDevice, "DeviceSerialNumber", &hDeviceSerialNumber);
//	if (err != SPINNAKER_ERR_SUCCESS)
//	{
//        serialNumberStr = std::string("no serial number node");
//	}
//	else
//	{
//	    char deviceSerialNumber[MAX_BUFF_LEN];
//	    size_t lenDeviceSerialNumber = MAX_BUFF_LEN;
//		err = spinStringGetValue(hDeviceSerialNumber, deviceSerialNumber, &lenDeviceSerialNumber);
//		if (err != SPINNAKER_ERR_SUCCESS)
//		{
//            serialNumberStr = std::string("unable to get serial number");
//		}
//        serialNumberStr = std::string(deviceSerialNumber);
//	}
//
//    std::cout << "vendor: " << vendorNameStr << std::endl;
//    std::cout << "model:  " << modelNameStr << std::endl;
//    std::cout << "serial# " << serialNumberStr << std::endl;
//
//    return SPINNAKER_ERR_SUCCESS;
//}
//
