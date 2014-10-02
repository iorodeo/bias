#include <iostream>
#include <vector>
#include <stdint.h>
#include <unistd.h>
#include "guid.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "camera.hpp"
#include "camera_finder.hpp"

using namespace std;
using namespace bias;

int main(int argc, char *argv[]) 
{
    int numImage = 100;
    cv::Mat image;
    cv::Mat imageScaled;
    CameraFinder cameraFinder;
    std::vector<std::string> windowNames;

    cout << endl;
    cout << "number of cameras: " << cameraFinder.numberOfCameras() << endl;

    // Get and print list of camera guids

    GuidList guidList = cameraFinder.getGuidList(); 

    if (guidList.size() == 0) 
    {
        cout << "No cameas found" << endl;
        return -1;
    }

    {
        cout << endl;
        cout << "printing guid list" << endl;
        int cnt;

        GuidList::iterator it;
        for (it=guidList.begin(), cnt=0; it!=guidList.end(); it++, cnt++)
        {
            Guid guid = *it;
            cout  << "  [" << cnt << "] guid " << guid << endl;
        }
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
            CameraPtr cameraPtr = std::make_shared<Camera>(guid);
            cameraPtr -> connect();
            cameraPtrList.push_back(cameraPtr);
        }
    }

    // Create opencv named windows
    // ------------------------------------------------------------------------
    windowNames = std::vector<std::string>(cameraPtrList.size());
    for (int cnt = 0; cnt < cameraPtrList.size(); cnt++)
    {
        std::stringstream ssName;
        ssName << "Camera: " <<  cnt;
        windowNames[cnt] = ssName.str();
        cv::namedWindow(
                windowNames[cnt],
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_NORMAL
                );
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
        for (int i=0; i<numImage; i++) 
        {
            cout << "  image: " << i << "/" << numImage << endl;
            int cnt;
            CameraPtrList::iterator it;
            for (it=cameraPtrList.begin(), cnt=0; it!=cameraPtrList.end(); it++, cnt++)
            {
                cout << "    camera: " << cnt << endl;
                CameraPtr cameraPtr = *it;
                image = cameraPtr -> grabImage();
                if (i%2 == 0) {
                    cv::resize(image, imageScaled, cv::Size(0,0), 0.8, 0.8);
                    cv::imshow(windowNames[cnt], imageScaled);
                    cv::waitKey(10);
                }
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
