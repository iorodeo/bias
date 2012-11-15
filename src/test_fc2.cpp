#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "utils.hpp"
#include "guid.hpp"
#include "property.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"

using namespace std;
using namespace bias;

int main(int argc, char** argv)
{        
    int cnt;
    int numGrab = 2000;
    CameraFinder camFinder;
    cv::Mat image;
    cv::Mat imageScaled;
    std::vector<std::string> windowNames;

    // Create and Connect cameras
    // ------------------------------------------------------------------------
    cnt = 0;
    CameraPtrList camPtrList = camFinder.createCameraPtrList();
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        )
    {
        CameraPtr camPtr = *it;
        camPtr -> connect();
    }

    // Create opencv named windows
    // ------------------------------------------------------------------------
    windowNames = std::vector<std::string>(camPtrList.size());
    for ( cnt = 0; cnt < camPtrList.size(); cnt++)
    {
        std::stringstream ssName;
        ssName << "Camera: " <<  cnt;
        windowNames[cnt] = ssName.str();
        cv::namedWindow(
                windowNames[cnt],
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED
                );
    }

    // Print camera information
    // ------------------------------------------------------------------------
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        ) 
    {
        cout << endl << "Camera " << cnt << endl;
        CameraPtr camPtr = *it;
        camPtr -> printInfo();
        cnt++;
    }

    // Print Property information for all cameras
    // ------------------------------------------------------------------------
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin();
            it!=camPtrList.end();
            it++
        )
    {
        cout << endl << "Camera " << cnt << " Property Information" << endl;
        CameraPtr camPtr = *it;
        camPtr -> printAllPropertyInfos();
        cnt++;
    }

    // Print property values for all cameras
    // ------------------------------------------------------------------------
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin();
            it!=camPtrList.end();
            it++
        )
    {
        cout << endl << "Camera " << cnt << " Property Values" << endl;
        CameraPtr camPtr = *it;
        camPtr -> printAllProperties();
        cnt++;
    }

    // Start capture on cameras
    // ------------------------------------------------------------------------
    cout << "Starting capture on cameras: ";
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        ) 
    {
        cout << cnt << " ";
        CameraPtr cameraPtr = *it;
        cameraPtr -> startCapture();
        cnt++;
    }
    cout << "done" << endl;
    cout << endl;
   

    // Grab images
    // ------------------------------------------------------------------------
    cout << "Grabing images" << endl;
    cout << endl;
    for (int i=0; i<numGrab; i++) 
    {
        cout << "  image: " << (i+1) << "/" << numGrab << " camera: ";
        cnt = 0;
        for (
                CameraPtrList::iterator it=camPtrList.begin();
                it!=camPtrList.end(); 
                it++
            )
        {
            CameraPtr cameraPtr = *it;
            cameraPtr -> grabImage(image);

            if (cnt%10 == 0) {
                cv::resize(image,imageScaled, cv::Size(0,0), 0.5, 0.5);
                cv::imshow(windowNames[cnt], imageScaled);
                cv::waitKey(1);
            }

            cout << cnt << " ";
            cnt++;
        } 
        cout << endl;
    }


    // Stop capture on cameras
    // ------------------------------------------------------------------------
    cout << endl;
    cout << "Stopping capture on cameras: ";
    cnt = 0;
    for (
            CameraPtrList::iterator it=camPtrList.begin(); 
            it!=camPtrList.end(); 
            it++
        ) 
    {
        cout << cnt << " "; 
        CameraPtr cameraPtr = *it;
        cameraPtr -> stopCapture();
        cnt++;
    }
    cout << "done" << endl << endl;
    

	return 0;
}

