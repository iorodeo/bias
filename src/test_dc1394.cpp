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

}
