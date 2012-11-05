#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>


namespace bias {

    CameraDevice_fc2::CameraDevice_fc2() : CameraDevice()
    {
        rawImageCreated_ = false;
    }

    CameraDevice_fc2::CameraDevice_fc2(Guid guid) : CameraDevice(guid)
    {
        fc2Error error= fc2CreateContext(&context_);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 context"; 
            throw RuntimeError(ERROR_FC2_CREATE_CONTEXT, ssError.str());
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
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to destroy FlyCapture2 context";
            throw RuntimeError(ERROR_FC2_DESTROY_CONTEXT, ssError.str());
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
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unabled to connect ot FlyCapture2 device";
                throw RuntimeError(ERROR_FC2_CONNECT, ssError.str());
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
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to disconnect from FlyCapture2 device";
                throw RuntimeError(ERROR_FC2_DISCONNECT, ssError.str());
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
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to start FlyCapture2 capture";
                throw RuntimeError(ERROR_FC2_START_CAPTURE, ssError.str());
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
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError <<": unable to stop FlyCapture2 capture";
                throw RuntimeError(ERROR_FC2_STOP_CAPTURE, ssError.str());

            }
            capturing_ = false;
        }
    }

    void CameraDevice_fc2::grabImage()
    {
        if ( capturing_ ) {
            fc2Error error = fc2RetrieveBuffer(context_, &rawImage_);
            if ( error != FC2_ERROR_OK ) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to retrieve image from buffer";
                throw RuntimeError(ERROR_FC2_RETRIEVE_BUFFER, ssError.str());
            }
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
            ss << " Camera vendor:  " << camInfo.vendorName << std::endl;
            ss << " Camera model:   " << camInfo.modelName << std::endl;
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

    void CameraDevice_fc2::printGuid() 
    { 
        guid_.printValue(); 
    }

    void CameraDevice_fc2::printInfo()
    {
        std::cout << toString();
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
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 image";
            throw RuntimeError(ERROR_FC2_CREATE_IMAGE, ssError.str());
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
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to destroy FlyCapture2 image";
                throw RuntimeError(ERROR_FC2_DESTROY_IMAGE, ssError.str());
            }
            rawImageCreated_ = false;
        }
    }

}
#endif

