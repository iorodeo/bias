#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include "guid.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"

using namespace std;
using namespace bias;

int main(int argc, char *argv[]) 
{
    CameraFinder cameraFinder;

    cameraFinder.update();
    cout << endl;
    cout << "number of cameras: " << cameraFinder.numberOfCameras() << endl;

    // Get and print list of camera guids

    // new version
    // -------------------------------------------------------
    //GuidList = cameraFinder.getGuidList(); 
    // --------------------------------------------------------
    
    // old version
    // -------------------------------------------------------
    GuidPtrList guidPtrList = cameraFinder.getGuidPtrList();
    //--------------------------------------------------------
    {
        cout << endl;
        cout << "printing guid list" << endl;
        int cnt;

        // new version
        // ----------------------------------------------------------------------
        //GuidList::iterator it;
        //for (it=guidList.begin(), cnt=0; it!=guidList.end(); it++, cnt++)
        //{
        //    Guid guid = *it;
        //    cout  << "  [" << cnt << "] guid " << guid << endl;
        //}
        // -----------------------------------------------------------------------

        // old version
        // ----------------------------------------------------------------------
        //GuidPtrList::iterator it;
        //for (it=guidPtrList.begin(), cnt=0; it!=guidPtrList.end(); it++, cnt++)
        //{
        //    cout  << "  [" << cnt << "] guid " << **it << endl;
        //}
        // -----------------------------------------------------------------------
    }

    // Create, connect and add cameras to camera list
    CameraPtrList cameraPtrList;
    {
        cout << endl;
        cout << "creating & connecting cameras" << endl;
        int cnt;
        GuidList::iterator it;
        for (it=guidList.begin(), cnt=0; it!=guidList.end(); it++, cnt++)
        {
            Guid guid = *it;
            cout  << "  [" << cnt << "] guid " << guid << endl;
            CameraPtr cameraPtr = std::make_shared<Camera>(**it);
            cameraPtr -> connect();
            cameraPtrList.push_back(cameraPtr);
        }


        // old version
        // ---------------------------------------------------------------------
        //GuidPtrList::iterator it;
        //for (it=guidPtrList.begin(), cnt=0; it!=guidPtrList.end(); it++, cnt++)
        //{
        //    cout  << "  [" << cnt << "] guid " << **it << endl;
        //    CameraPtr cameraPtr = std::make_shared<Camera>(**it);
        //    cameraPtr -> connect();
        //    cameraPtrList.push_back(cameraPtr);
        //}
        // --------------------------------------------------------------------
    }

    // Print camera information
    {
        cout << endl;
        cout << "printing camera info" << endl;
        int cnt;
        CameraPtrList::iterator it;
        for (it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++)
        {
            cout << "camera: " << cnt << endl;
            CameraPtr cameraPtr = *it;
            cameraPtr -> printInfo();
        }
    }

    // Start capture
    {
        cout << endl;
        cout << "starting video capture" << endl;
        int cnt;
        CameraPtrList::iterator it;
        for (it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++)
        {
            cout << "  camera: " << cnt << endl;
            CameraPtr cameraPtr = *it;
            cameraPtr -> startCapture();
        }
    }

    // Grab some frames
    {
        cout << "grabbing some images" << endl;
        int numImage = 100;
        for (int i=0; i<numImage; i++) 
        {
            cout << "  image: " << i << "/" << numImage << endl;
            int cnt;
            CameraPtrList::iterator it;
            for (it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++)
            {
                cout << "    camera: " << cnt << endl;
                CameraPtr cameraPtr = *it;
                cameraPtr -> grabImage();
            }
        }
    }

    // Stop capture
    {
        cout << endl;
        cout << "stoping video capture" << endl;
        int cnt;
        CameraPtrList::iterator it;
        for (it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++)
        {
            cout << "  camera: " << cnt << endl;
            CameraPtr cameraPtr = *it;
            cameraPtr -> stopCapture();
        }
    }
    

    return 0;
}
