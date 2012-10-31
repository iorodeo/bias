#ifdef WITH_FC2
#include <iostream>
#include "camera_device_fc2.hpp"
#include "utils_fc2.hpp"

using namespace std;

namespace bias {

    CameraDevice_fc2::CameraDevice_fc2(Guid guid) 
        : CameraDevice_base(guid)
    {
        cout << __PRETTY_FUNCTION__ << endl;
        fc2Error error= fc2CreateContext(&context_);
        if ( error != FC2_ERROR_OK) {
            cout << "Error: fc2CreateContext" << endl;
            // TO DO ... throw some kind of error
        }
    }


    void CameraDevice_fc2::connect() 
    {
        cout << "CameraDevice_fc2::connect" << endl;
        fc2PGRGuid guid = getGuid_fc2();

        fc2Error error = fc2Connect(context_, &guid);
        if ( error != FC2_ERROR_OK ) {
            cout << "Error: fc2Connect" << endl;
            // TO DO ... throw some kind of error
        }
    }

    CameraLib CameraDevice_fc2::getCameraLib()
    {
        return guid_.getCameraLib();
    }

    void CameraDevice_fc2::printInfo()
    {
        fc2Error error;
        fc2CameraInfo camInfo;
        error = fc2GetCameraInfo( context_, &camInfo );
        if ( error != FC2_ERROR_OK ) {
            printf( "Error: unable to get camera info\n" );
            // TO DO ... throw some kind of error
            return;
        }

        cout << endl;
        cout << " ------------------ " << endl;
        cout << " CAMERA INFORMATION " << endl;
        cout << " ------------------ " << endl;
        cout << endl;

        cout << " Guid:           " << guid_ << endl;
        cout << " Serial number:  " << camInfo.serialNumber << endl;
        cout << " Camera model:   " << camInfo.modelName << endl;
        cout << " Camera vendor:  " << camInfo.vendorName << endl;
        cout << " Sensor          " << endl;
        cout << "   Type:         " << camInfo.sensorInfo << endl;
        cout << "   Resolution:   " << camInfo.sensorResolution << endl;
        cout << " Firmware        " << endl;     
        cout << "   Version:      " << camInfo.firmwareVersion << endl;
        cout << "   Build time:   " << camInfo.firmwareBuildTime << endl;
        cout << " Color camera:   " << boolalpha << (bool) camInfo.isColorCamera << endl;
        cout << " Interface:      " << getInterfaceTypeString_fc2(camInfo.interfaceType) << endl;
        cout << endl;
    }

    void CameraDevice_fc2::printGuid() { guid_.printValue(); }

    // Private methods
    // ---------------------------------------------------------------------------

    fc2PGRGuid CameraDevice_fc2::getGuid_fc2()
    {
        return guid_.getValue_fc2();
    }

}
#endif

