#ifndef CAMERA_DEVICE_BASE_HPP
#define CAMERA_DEVICE_BASE_HPP

#include <iostream>
using namespace std;

namespace bias {

    // Base class for camera device objects.

    class CameraDevice_base
    {
        public:
            CameraDevice_base() {};
            CameraDevice_base(Guid guid) 
            { 
                cout << __PRETTY_FUNCTION__ << endl;
                guid_ = guid;
            };
            virtual ~CameraDevice_base() {};
            virtual CameraLib getCameraLib() {};
            virtual void connect() {};
            virtual void printInfo() {};
            virtual void printGuid() {};

        protected:
            Guid guid_;
    };
}


#endif
