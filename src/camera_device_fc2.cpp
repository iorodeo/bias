#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>


namespace bias {

    CameraDevice_fc2::CameraDevice_fc2() : CameraDevice()
    {
        workingImagesCreated_ = false;
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
        workingImagesCreated_ = false;
    }

    CameraDevice_fc2::~CameraDevice_fc2() 
    {
        if ( capturing_ ) { stopCapture(); }
        
        if ( workingImagesCreated_ ) { destroyWorkingImages(); }

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
        fc2Error error;

        if ( connected_ && !capturing_ ) 
        {
            createWorkingImages(); // destroys any existing raw image

            // Temporary - for now just pick a video mode and frame rate 
            // which works.
            setVideoMode_Format7Mode0();

            // Start image capture
            error = fc2StartCapture(context_);
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
        fc2Error error;
        if ( capturing_ ) {

            // ----------------------------------------------------------------------------
            //fc2VideoMode videoMode;
            //fc2FrameRate frameRate;
            //fc2GetVideoModeAndFrameRate(context_, &videoMode, &frameRate);
            //std::cout << "videoMode: " << getVideoModeString_fc2(videoMode) << std::endl;
            //std::cout << "frameRate: " << getFrameRateString_fc2(frameRate) << std::endl;
            //std::cout << std::endl;
            // -----------------------------------------------------------------------------

            // Retrieve image from buffer
            error = fc2RetrieveBuffer(context_, &rawImage_);
            if ( error != FC2_ERROR_OK ) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to retrieve image from buffer";
                throw RuntimeError(ERROR_FC2_RETRIEVE_BUFFER, ssError.str());
            }

            //printImageInfo_fc2(rawImage_);
            //std::cout << std::flush;

            // Temporary - convert image to mono8 format. Need to figure out 
            // how to do this automatically.  
            error = fc2ConvertImageTo(
                    FC2_PIXEL_FORMAT_MONO8, 
                    &rawImage_, 
                    &convertedImage_
                    );
            if ( error != FC2_ERROR_OK ) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to convert image";
                throw RuntimeError(ERROR_FC2_CONVERT_IMAGE, ssError.str());

            }
            //printImageInfo_fc2(convertedImage_);
            //std::cout << std::flush;
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

    void CameraDevice_fc2::createWorkingImages()
    { 
        fc2Error error;

        // Destroy any pre-existing images
        if ( workingImagesCreated_ ) { destroyWorkingImages(); }

        // Create new rawImage_
        error = fc2CreateImage(&rawImage_);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 image";
            throw RuntimeError(ERROR_FC2_CREATE_IMAGE, ssError.str());
        }

        // Create new convertedImage_
        error = fc2CreateImage(&convertedImage_);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 image";
            throw RuntimeError(ERROR_FC2_CREATE_IMAGE, ssError.str());
        }
        workingImagesCreated_ = true;
    }

    void CameraDevice_fc2::destroyWorkingImages()
    {
        fc2Error error;
        if ( workingImagesCreated_ ) 
        {
            error = fc2DestroyImage(&rawImage_);
            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to destroy FlyCapture2 image";
                throw RuntimeError(ERROR_FC2_DESTROY_IMAGE, ssError.str());
            }

            error = fc2DestroyImage(&convertedImage_);
            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to destroy FlyCapture2 image";
                throw RuntimeError(ERROR_FC2_DESTROY_IMAGE, ssError.str());
            }
            workingImagesCreated_ = false;
        }
    }

    // Temporary methods
    void CameraDevice_fc2::setVideoMode_Format7Mode0()
    {
        fc2Error error;
        fc2Format7Info format7Info;
        fc2Format7ImageSettings imageSettings;
        unsigned int packetSize;
        float percentage; 
        BOOL supported;

        // Get the format 7 info
        format7Info.mode = FC2_MODE_0;
        error = fc2GetFormat7Info(context_, &format7Info, &supported);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to get FlyCapture2 format7 information"; 
            throw RuntimeError(ERROR_FC2_GET_FORMAT7_INFO, ssError.str());
        }
        if ( !supported )
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unsupported FlyCapture2 video mode, "; 
            ssError << getModeString_fc2(format7Info.mode);
            throw RuntimeError(ERROR_FC2_UNSUPPORTED_VIDEO_MODE, ssError.str());
        }

        if (1) // Print format7 information for selected mode
        {
            std::cout << std::endl << std::endl;
            std::cout << "Supported: " << std::boolalpha << bool(supported); 
            std::cout << std::noboolalpha << std::endl << std::endl;
            printFormat7Info_fc2(format7Info);
        }

        // Get Current format7 settings
        error = fc2GetFormat7Configuration(
                context_, 
                &imageSettings,
                &packetSize,
                &percentage
                );
        if (error != FC2_ERROR_OK)
        { 
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to get FlyCapture2 format7 configuration"; 
            throw RuntimeError( ERROR_FC2_GET_FORMAT7_CONFIGURATION, ssError.str());
        }

        if (1) // Print current configuration settings
        {
            std::cout << std::endl;
            printFormat7Configuration_fc2(imageSettings,packetSize,percentage);
            std::cout << std::endl;
        }

        // Desired format7 configuration
        imageSettings.mode = format7Info.mode;
        imageSettings.offsetX = 0;
        imageSettings.offsetY = 0;
        imageSettings.width = format7Info.maxWidth;
        imageSettings.height = format7Info.maxHeight;
        imageSettings.pixelFormat = FC2_PIXEL_FORMAT_RAW8;

    }
}
#endif

