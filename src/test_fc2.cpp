#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
#include <memory>
#include "utils.hpp"
#include "guid.hpp"
#include "property.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"

using namespace std;
using namespace bias;


int main(int argc, char** argv)
{        
    CameraFinder cameraFinder;
    CameraPtrList cameraPtrList;

    // new version
    // ---------------------------
    //GuidList guidList;
    
    // old version
    // --------------------------
    GuidPtrList guidPtrList; 
    //---------------------------

    cameraFinder.update();
    // new version
    // -------------------------------------------
    //guidList = cameraFinder.getGuidList();
    // --------------------------------------------

    // old version
    // ------------------------------------------- 
    guidPtrList = cameraFinder.getGuidPtrList();
    // -------------------------------------------
    
    // Create cameras and connect
    // ------------------------------------------------------------------------
    {
        cout << "Creating and connecting to cameras" << endl;
       
        // new version
        // -------------------------------------------------------------
        //GuidList::iterator it;
        //for (it=guidList.begin(); it!=guidList.end(); it++) 
        //{
        //    Guid guid = *it;
        //    CameraPtr cameraPtr = std::make_shared<Camera>(guid);
        //    cameraPtr -> connect();
        //    cameraPtrList.push_back(cameraPtr);
        //}
        // -------------------------------------------------------------

        // old version
        // -------------------------------------------------------------
        GuidPtrList::iterator it;
        for (it=guidPtrList.begin(); it!=guidPtrList.end(); it++) 
        {
            GuidPtr guidPtr = *it;
            CameraPtr cameraPtr = std::make_shared<Camera>(*guidPtr);
            cameraPtr -> connect();
            cameraPtrList.push_back(cameraPtr);
        }
        // --------------------------------------------------------------
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
            cout << "Allowed VideoModes and FrameRates" << endl << endl;
            for (vit=vmList.begin(); vit!=vmList.end(); vit++) 
            {
                cout <<  " " << getVideoModeString(*vit) <<  endl; 
                FrameRateList frList = cameraPtr -> getAllowedFrameRates(*vit);
                FrameRateList::iterator fit;
                for (fit=frList.begin(); fit!=frList.end(); fit++)
                {
                    cout << "   " << getFrameRateString(*fit) << endl;
                }
                cout << endl;
            }

            PropertyTypeList propTypeList = getListOfPropertyTypes();
            PropertyTypeList::iterator pit;
            for (pit=propTypeList.begin(); pit!=propTypeList.end(); pit++)
            {
                PropertyType propType = *pit;
                Property property = cameraPtr -> getProperty(propType);
                PropertyInfo propertyInfo = cameraPtr -> getPropertyInfo(propType);
                cout << "-----------------------------------------" << endl;
                property.print(); 
                cout << endl;
                propertyInfo.print();
                cout << "-----------------------------------------" << endl;
            }

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

    // Print property types.
    // ------------------------------------------------------------------------
    {
        PropertyTypeList propTypeList = getListOfPropertyTypes();
        PropertyTypeList::iterator it;

        cout << "Property Types" << endl << endl;
        for (it=propTypeList.begin(); it!=propTypeList.end(); it++)
        {
            cout << getPropertyTypeString(*it) << endl;
        }
        cout << endl;
    }


    // Get property and propertyInfo maps
    if (1)
    {
        CameraPtr cameraPtr = cameraPtrList.front();
        PropertyMap propMap = cameraPtr -> getMapOfProperties();

        for (PropertyMap::iterator it=propMap.begin(); it!=propMap.end(); it++)
        {
            PropertyType propType = (*it).first;
            Property prop = (*it).second;

            cout << getPropertyTypeString(propType) << endl;
            cout << prop.toString() << endl;
            cout << endl;
        }
        cout << endl;
    }

    if (1)
    {

        CameraPtr cameraPtr = cameraPtrList.front();
        PropertyInfoMap propInfoMap = cameraPtr -> getMapOfPropertyInfos();

        cout << "size: " << propInfoMap.size() << endl;

        for (PropertyInfoMap::iterator it=propInfoMap.begin(); it!=propInfoMap.end(); it++)
        {
            PropertyType propType = (*it).first;
            PropertyInfo propInfo = (*it).second;

            cout << getPropertyTypeString(propType) << endl;
            cout << propInfo.toString() << endl;
            cout << endl;
        }
    }

	return 0;
}

