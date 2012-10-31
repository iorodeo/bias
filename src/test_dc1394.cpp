#include <iostream>
#include "guid.hpp"
#include "camera.hpp"

using namespace std;
using namespace bias;

int main(int argc, char *argv[]) 
{
    uint64_t guidRaw = 12345678910111213ULL;
    Guid guid;
    Camera camera;

    cout << "guid: " << guid << endl;
    guid = Guid(guidRaw);
    cout << "guid: " << guid << endl;

    camera = Camera(guid);
    cout << "guid: " << guid << endl;


    return 0; 
}
