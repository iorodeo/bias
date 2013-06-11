#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif

#include <iostream>
#include "camera_facade.hpp"

using namespace bias;

int main(int argc, char** argv)
{        

    const float frameRate = 10.0;
    const unsigned int numberOfFrames = 100;

    std::cout << std::endl;
    std::cout << "---------" << std::endl;
    std::cout << "BIAS Demo" << std::endl;
    std::cout << "---------" << std::endl;


    // Find attached cameras
    // -----------------------------------------------------------------------------
    std::cout << "searching for cameras ... ";

    CameraFinder camFinder;
    CameraPtrList camPtrList = camFinder.createCameraPtrList();
    if (camPtrList.empty())
    {
        std::cout << "no cameras found." << std::endl << std::endl;
        return 0;
    }

    std::cout << camPtrList.size() << " cameras" << std::endl;
    for (CameraPtrList::iterator it=camPtrList.begin(); it!=camPtrList.end(); it++)
    {
        CameraPtr camPtr = *it;
        std::cout << "guid: " << camPtr -> getGuid().toString() << std::endl;
    }


    // Connect to first camera found
    // -----------------------------------------------------------------------------
    std::cout << "connecting to first camera ... ";

    CameraPtr camPtr = camPtrList.front();
    try
    {
        camPtr -> connect();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return 0;
    }
    std::cout << "done" << std::endl;

    // Print camera info
    std::cout << "model:  " << camPtr -> getModelName() << std::endl;
    std::cout << "vendor: " << camPtr -> getVendorName() << std::endl;

    // Set camera videomode, trigger type, and framerate
    // -----------------------------------------------------------------------------
    std::cout << "setting videoMode to Format7 and trigger to internal ... ";
    try
    {
        camPtr -> setVideoMode(VIDEOMODE_FORMAT7);
        camPtr -> setTriggerInternal();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return 0;
    }
    std::cout << "done" << std::endl;

    std::cout << "setting framerate to " << frameRate << " fps ... ";
    try
    {
        PropertyInfo  frameRateInfo = camPtr -> getPropertyInfo(PROPERTY_TYPE_FRAME_RATE);
        Property frameRateProp = camPtr -> getProperty(PROPERTY_TYPE_FRAME_RATE);
        frameRateProp.absoluteControl = true;
        frameRateProp.absoluteValue = frameRate;
        frameRateProp.autoActive = false;
        camPtr -> setProperty(frameRateProp);
        frameRateProp = camPtr -> getProperty(PROPERTY_TYPE_FRAME_RATE);
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return 0;
    }
    std::cout << "done" << std::endl;

    // Start image capture
    // -----------------------------------------------------------------------------
    std::cout << "starting image capture ... ";
    try
    {
        camPtr -> startCapture();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return 0;
    }
    std::cout << "done" << std::endl;

    // Grab images 
    // -----------------------------------------------------------------------------
    std::cout << "grabbing images" << std::endl;
    for (unsigned int i=0; i< numberOfFrames; i++)
    {
        std::cout << " frame " << i << " ... ";
        cv::Mat image = camPtr -> grabImage();
        std::cout << "done" << std::endl;
    }

    // Stop image capture
    // -----------------------------------------------------------------------------
    std::cout << "stopping image capture ... ";
    try
    {
        camPtr -> stopCapture();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return 0;
    }
    std::cout << "done" << std::endl;


    // Disconnect from camera
    // -----------------------------------------------------------------------------
    std::cout << "disconnecting camera ... ";
    try
    {
        camPtr -> disconnect();
    }
    catch (RuntimeError &runtimeError)
    {
        std::cout << "error: " << runtimeError.what() << std::endl << std::endl;
        return 0;
    }
    std::cout << "done" << std::endl;
    std::cout << std::endl;

	return 0;
}

