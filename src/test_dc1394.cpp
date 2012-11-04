#include <iostream>
#include <stdint.h>
#include "guid.hpp"
#include "camera.hpp"
#include "camera_finder.hpp"

using namespace std;
using namespace bias;

int main(int argc, char *argv[]) 
{
    CameraFinder cameraFinder;
    cameraFinder.update();

    cout << "number of cameras: " << cameraFinder.numberOfCameras() << endl;

    GuidPtrList guidPtrList = cameraFinder.getGuidPtrList();

    {
        cout << "guid list" << endl;
        int cnt;
        GuidPtrList::iterator it;
        for (it=guidPtrList.begin(), cnt=0; it!=guidPtrList.end(); it++, cnt++)
        {
            cout  << "[" << cnt << "] " << **it << endl;
        }
    }
    return 0;
}
