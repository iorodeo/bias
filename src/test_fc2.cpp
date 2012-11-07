#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
#include <memory>
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

    {
        cout << "Creating and connecting to cameras" << endl;
        GuidPtrList::iterator it;
        for (it=guidPtrList.begin(); it!=guidPtrList.end(); it++) 
        {
            Guid guid = **it;
            CameraPtr camPtr = std::make_shared<Camera>(guid);
            camPtr -> connect();
            cout << "isColor: " << boolalpha << (camPtr -> isColor()) << noboolalpha << endl;
            cameraPtrList.push_back(camPtr);
        }
    }

    {
        cout << "Printing camera information" << endl;
        int cnt;
        CameraPtrList::iterator it;
        for(it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++) 
        {
            cout << endl << "Camera " << cnt << endl;
            Camera camera = **it;
            camera.printInfo();
        }
    }

    {
        cout << "Starting capture on cameras: ";
        int cnt;
        CameraPtrList::iterator it;
        for(it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++) 
        {
            cout << cnt << " ";
            Camera camera = **it;
            camera.startCapture();
        }
        cout << "done" << endl;
        cout << endl;
    }

    {
        int camCount;
        int numImage = 3;
        CameraPtrList::iterator it;
        cout << "Grabing images" << endl;
        cout << endl;
        for (int i=0; i<numImage; i++) 
        {
            for ( it=cameraPtrList.begin(), camCount=0; it!=cameraPtrList.end(); it++, camCount++)
            {
                Camera camera = **it;
                cout << "  image: " << (i+1) << "/" << numImage << " camera: " << camCount << endl;
                camera.grabImage();
            } 
            cout << endl;
        }
    }

    {
        cout << "Stopping capture on cameras: ";
        int cnt;
        CameraPtrList::iterator it;
        for( it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++) 
        {
            cout << cnt << " "; 
            Camera camera = **it;
            camera.stopCapture();
        }
        cout << "done" << endl;
    }

	return 0;
}

