#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <opencv2/core/core.hpp>

#include "utils.hpp"
#include "guid.hpp"
#include "property.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"
#include "basic_types.hpp"

using namespace std;
using namespace bias;

int main(int argc, char** argv)
{        
    int cnt;
    int numGrab = 5;
    string input;

    CameraFinder camFinder;
    cv::Mat image;

    cout << endl;

    // Create and Connect cameras - set videomode
    // ------------------------------------------------------------------------
    cnt = 0;
    cout << "Getting list of cameras" << endl;
    CameraPtrList camPtrList = camFinder.createCameraPtrList();
    if (camPtrList.empty()) 
    {
        cout << "No cmaeras found" << endl << endl;
        //cout << "Press enter to quit" << endl;
        //getline(cin,input);
        return 0;
    }
    cout << "Done" << endl << endl;

    cout << "Connecting" << endl;
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        )
    {
        CameraPtr camPtr = *it;
        Guid guid = camPtr -> getGuid();
        cout << "Camera: " << cnt << ", GUID: " << guid.toString() << endl;
        camPtr -> connect();
        camPtr -> setVideoMode(VIDEOMODE_FORMAT7);
        camPtr -> setTriggerInternal();
        cnt++;
    }
    cout << "Done" << endl << endl;


    // Start capture on cameras
    // ------------------------------------------------------------------------
    cout << "Starting capture" << endl;
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        ) 
    {
        CameraPtr camPtr = *it;
        cout << "Camera: " << cnt << endl;
        camPtr -> startCapture();
        cnt++;
    }
    cout << "Done" << endl << endl;
   

    // Grab images
    // ------------------------------------------------------------------------
    cout << "Grabing images" << endl;
    cout << endl;
    for (int i=0; i<numGrab; i++) 
    {
        cout << "Image: " << (i+1) << "/" << numGrab << endl;
        cnt = 0;
        for (
                CameraPtrList::iterator it=camPtrList.begin();
                it!=camPtrList.end(); 
                it++
            )
        {
            CameraPtr camPtr = *it;
            cout << "Camera: " << cnt << endl;
            camPtr -> grabImage(image);
            cnt++;
        } 
        cout << endl;
    }


    // Stop capture on cameras
    // ------------------------------------------------------------------------
    cout << endl;
    cout << "Stopping capture" << endl;
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        ) 
    {
        cout << "Camera: " << cnt << endl; 
        CameraPtr cameraPtr = *it;
        cameraPtr -> stopCapture();
        cnt++;
    }
    cout << "Done" << endl << endl;

    //cout << "Press Enter to Quit" << endl;
    //getline(cin,input);

	return 0;
}

