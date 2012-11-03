#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
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

    // Create and connect cameras
    {
        GuidPtrList::iterator it;
        for (it=guidPtrList.begin(); it!=guidPtrList.end(); it++) 
        {
            Guid guid = **it;
            CameraPtr camPtr(new Camera(guid));
            camPtr -> connect();
            cameraPtrList.push_back(camPtr);
        }
    }

    // Print camera information
    {
        CameraPtrList::iterator it;
        for( it=cameraPtrList.begin(); it!=cameraPtrList.end(); it++) 
        {
            Camera camera = **it;
            camera.printInfo();
        }
    }


	return 0;
}

