#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#include <sstream>
#include <iostream>
#include "utils_fc2.hpp"

namespace bias {

    CameraDevice_fc2::CameraDevice_fc2() 
    {
        rawImageCreated_ = false;
    }

    CameraDevice_fc2::CameraDevice_fc2(Guid guid) 
        : CameraDevice(guid)
    {
        fc2Error error= fc2CreateContext(&context_);
        if (error != FC2_ERROR_OK) 
        {
            std::cout << "Error: fc2CreateContext" << std::endl;
            // TO DO ... throw some kind of error
            return;
        }
        rawImageCreated_ = false;
    }

    CameraDevice_fc2::~CameraDevice_fc2() 
    {
        if ( capturing_ ) { stopCapture(); }
        
        if ( rawImageCreated_ ) { destroyRawImage(); }

        if ( connected_ ) { disconnect(); }

        fc2Error error = fc2DestroyContext(context_);
        if ( error != FC2_ERROR_OK ) 
        {
            std::cout << "Error: fc2DestroyContext" << std::endl;
            // TO DO ... throw some kind of error;
        }

    }

    void CameraDevice_fc2::connect() 
    {
        if ( !connected_ ) 
        { 
            fc2PGRGuid guid = getGuid_fc2();
            fc2Error error = fc2Connect(context_, &guid);
            if (error != FC2_ERROR_OK) 
            {
                std::cout << "Error: fc2Connect" << std::endl;
                // TO DO ... throw some kind of error
                return;
            }
            connected_ = true;
        }
    }

    void CameraDevice_fc2::disconnect()
    {
        if ( connected_ ) 
        {
            fc2Error error = fc2Disconnect(context_);
            if (error != FC2_ERROR_OK) 
            {
                std::cout << "Error: fc2Disconnect" << std::endl;
                // TO DO ... throw some kind of error
                return;
            }
            connected_ = false;
        }
    }

    void CameraDevice_fc2::startCapture()
    {
        if ( connected_ && !capturing_ ) 
        {
            createRawImage(); // destroys any existing raw image

            // Start image capture
            fc2Error error = fc2StartCapture(context_);
            if (error != FC2_ERROR_OK) 
            {
                std::cout << "Error: fc2StartCapture" << std::endl;
                // TO DO ... throw some kind of error
                return;
            }
            capturing_ = true;
        }
    }

    void CameraDevice_fc2::stopCapture()
    {
        if ( capturing_ ) 
        {
            // Stop capture
            fc2Error error = fc2StopCapture(context_);
            if (error != FC2_ERROR_OK) 
            { 
                std::cout << "Error: fc2StopCapture" << std::endl;
                // TO DO ... throw some kind of error
            }
            capturing_ = false;
        }
    }

    void CameraDevice_fc2::grabImage()
    {
        fc2Error error = fc2RetrieveBuffer(context_, &rawImage_);
        if ( error != FC2_ERROR_OK ) 
        {
            std::cout << "Error: grabbing image w/ fc2RetrieveBuffer";
            std::cout << std::endl;
            // TO DO ... throw some kind of error
        }

    }

    CameraLib CameraDevice_fc2::getCameraLib()
    {
        return guid_.getCameraLib();
    }

    std::string CameraDevice_fc2::toString()
    {
        std::stringstream ss; 
        fc2Error error;
        fc2CameraInfo camInfo;
        error = fc2GetCameraInfo( context_, &camInfo );
        if ( error != FC2_ERROR_OK ) 
        {
            ss << "Error: unable to get camera info";
        }
        else
        {
            ss << std::endl;
            ss << " ------------------ " << std::endl;
            ss << " CAMERA INFORMATION " << std::endl;
            ss << " ------------------ " << std::endl;
            ss << std::endl;

            ss << " Guid:           " << guid_ << std::endl;
            ss << " Serial number:  " << camInfo.serialNumber << std::endl;
            ss << " Camera model:   " << camInfo.modelName << std::endl;
            ss << " Camera vendor:  " << camInfo.vendorName << std::endl;
            ss << " Sensor          " << std::endl;
            ss << "   Type:         " << camInfo.sensorInfo << std::endl;
            ss << "   Resolution:   " << camInfo.sensorResolution << std::endl;
            ss << " Firmware        " << std::endl;     
            ss << "   Version:      " << camInfo.firmwareVersion << std::endl;
            ss << "   Build time:   " << camInfo.firmwareBuildTime << std::endl;
            ss << " Color camera:   " << std::boolalpha << (bool) camInfo.isColorCamera << std::endl;
            ss << " Interface:      " << getInterfaceTypeString_fc2(camInfo.interfaceType) << std::endl;
            ss << std::endl;
        }
        return ss.str();
    };

    void CameraDevice_fc2::printInfo()
    {
        std::cout << toString();
    }

    void CameraDevice_fc2::printGuid() 
    { 
        guid_.printValue(); 
    }
       
    // Private methods
    // ---------------------------------------------------------------------------

    fc2PGRGuid CameraDevice_fc2::getGuid_fc2()
    {
        return guid_.getValue_fc2();
    }

    void CameraDevice_fc2::createRawImage()
    { 
        // Destroy any pre-existing images
        if ( rawImageCreated_ ) { destroyRawImage(); }

        // Create new rawImage
        fc2Error error = fc2CreateImage(&rawImage_);
        if (error != FC2_ERROR_OK) 
        {
            std::cout << "Error creating image" << std::endl;
            // TO DO ... throw some kind of error 
            return;

        }
        rawImageCreated_ = true;
    }

    void CameraDevice_fc2::destroyRawImage()
    {
        if ( rawImageCreated_ ) 
        {
            fc2Error error = fc2DestroyImage(&rawImage_);
            if (error != FC2_ERROR_OK) 
            {
                std::cout << "Error destroying image" << std::endl;
                // TO DO ... throw some kinde of error
                return;
            }
            rawImageCreated_ = false;
        }
    }

}
#endif

