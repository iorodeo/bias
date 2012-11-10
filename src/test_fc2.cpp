#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
#include <memory>
#include "utils.hpp"
#include "guid.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"

using namespace std;
using namespace bias;


int main(int argc, char** argv)
{        
    CameraFinder cameraFinder;
    GuidPtrList guidPtrList; 
    CameraPtrList cameraPtrList;

    cameraFinder.update();
    guidPtrList = cameraFinder.getGuidPtrList();
    
    // Create cameras and connect
    // ------------------------------------------------------------------------
    {
        cout << "Creating and connecting to cameras" << endl;
        GuidPtrList::iterator it;
        for (it=guidPtrList.begin(); it!=guidPtrList.end(); it++) 
        {
            GuidPtr guidPtr = *it;
            CameraPtr cameraPtr = std::make_shared<Camera>(*guidPtr);
            cameraPtr -> connect();
            cameraPtrList.push_back(cameraPtr);
        }
    }


    // Print camera information
    // ------------------------------------------------------------------------
    {
        cout << "Printing camera information" << endl;
        int cnt;
        CameraPtrList::iterator it;
        for(it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++) 
        {
            cout << endl << "Camera " << cnt << endl;
            CameraPtr cameraPtr = *it;
            cameraPtr -> printInfo();

            VideoMode videoMode = cameraPtr -> getVideoMode();
            FrameRate frameRate = cameraPtr -> getFrameRate();
            ImageMode imagingMode = cameraPtr -> getImageMode();
            cout << "VideoMode: " << videoMode << endl;
            cout << "FrameRate: " << frameRate << endl;
            cout << "ImageMode: " << imagingMode << endl;
            cout << endl;

            VideoModeList vmList = cameraPtr -> getAllowedVideoModes();
            VideoModeList::iterator vit;
            cout << endl;
            cout << "Allowed Video Modes" << endl;
            for (vit=vmList.begin(); vit!=vmList.end(); vit++) 
            {
                cout << " " << *vit << " " << getVideoModeString(*vit) << endl; 
            }
            cout << endl;
        }
    }

    // Start capture on cameras
    // ------------------------------------------------------------------------
    {
        cout << "Starting capture on cameras: ";
        int cnt;
        CameraPtrList::iterator it;
        for(it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++) 
        {
            cout << cnt << " ";
            CameraPtr cameraPtr = *it;
            cameraPtr -> startCapture();
        }
        cout << "done" << endl;
        cout << endl;
    }


    // Grab images
    // ------------------------------------------------------------------------
    {
        int camCount;
        int numImage = 10;
        CameraPtrList::iterator it;
        cout << "Grabing images" << endl;
        cout << endl;
        for (int i=0; i<numImage; i++) 
        {
            cout << "  image: " << (i+1) << "/" << numImage << " camera: ";
            for (it=cameraPtrList.begin(), camCount=0; it!=cameraPtrList.end(); it++, camCount++)
            {
                CameraPtr cameraPtr = *it;
                cameraPtr -> grabImage();
                cout << camCount << " ";
            } 
            cout << endl;
        }
    }


    // Stop capture on cameras
    // ------------------------------------------------------------------------
    {
        cout << endl;
        cout << "Stopping capture on cameras: ";
        int cnt;
        CameraPtrList::iterator it;
        for(it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++) 
        {
            cout << cnt << " "; 
            CameraPtr cameraPtr = *it;
            cameraPtr -> stopCapture();
        }
        cout << "done" << endl << endl;
    }

	return 0;
}

