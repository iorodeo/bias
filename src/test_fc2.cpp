#if defined(WIN32) || defined(WIN64)
#define _CRT_SECURE_NO_WARNINGS		
#endif
#include <windows.h>
#include <iostream>
#include <cstdint>
#include "C/FlyCapture2_C.h"
#include "guid.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"

#include <memory>

using namespace std;
using namespace bias;


int main(int argc, char** argv)
{        
    //fc2Error error;
    //fc2Context queryContext;
    //fc2Context camContext;
    //fc2PGRGuid guid;

    //unsigned int numCameras = 0;    
    //unsigned int serialNumber = 0;

    //Guid guidObj;
    //Camera camera;

    //cout << endl;

    //// Get camera context and query for number of attached cameras
    //error = fc2CreateContext( &queryContext );
    //if ( error != FC2_ERROR_OK ) {
    //    cout <<  "Error in fc2CreateContext: " << error << endl;
    //    return 0;
    //}        
    //error = fc2GetNumOfCameras( queryContext, &numCameras );
    //if ( error != FC2_ERROR_OK ) {
    //    cout << "Error in fc2GetNumOfCameras: " << error << endl;
    //    return 0;
    //}

    //// Print number of cameras found and their guids
    //cout << "numCameras: " << numCameras << endl;
    //for ( int i=0; i< numCameras; i++ ) {
    //    error = fc2GetCameraFromIndex( queryContext, i, &guid );
    //    cout << " camera[" << i << "]:  guid = ";
    //    if ( error == FC2_ERROR_OK ) {
    //        for (int j=0; j<4; j++) {
    //            cout << hex << guid.value[j] << dec;
    //            if (j < 3) {
    //                cout << " ";
    //            }
    //        }
    //    }
    //    else {
    //        cout << "error";
    //    }
    //    cout << ", serial# = ";
    //    error = fc2GetCameraSerialNumberFromIndex( queryContext, i, &serialNumber);
    //    if ( error == FC2_ERROR_OK ) {
    //        cout << hex << serialNumber << dec;
    //    }
    //    else {
    //        cout << "error";
    //    }
    //    cout << endl;
    //}
    //if ( numCameras == 0 ) {
    //    cout << "No cameras detected" << endl;
    //    return 0;
    //}

    //// Get guid of first camera.
    //error = fc2GetCameraFromIndex( queryContext, 0, &guid );
    //if ( error != FC2_ERROR_OK ) {
    //    cout << "Error in fc2GetCameraFromIndex: " << error << endl;
    //    return 0;
    //}    



    //guidObj = Guid(guid);
    //cout << "guidObj: " << guidObj << endl;
    //camera = Camera(guid);
    //camera.printGuid();
    //camera.connect();
    //camera.printInfo();

    CameraFinder cameraFinder;

    for (int i=0; i<20; i++) {
        cout << endl;
        cout << "i = " << i << endl;
        cameraFinder.update();
        cout << endl;
        Sleep(1000);
    }

	return 0;
}

