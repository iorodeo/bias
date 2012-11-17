#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>


namespace bias {

    CameraDevice_fc2::CameraDevice_fc2() : CameraDevice()
    {
        rawImageCreated_ = false;
        convertedImageCreated_ = false;
    }

    CameraDevice_fc2::CameraDevice_fc2(Guid guid) : CameraDevice(guid)
    {
        rawImageCreated_ = false;
        convertedImageCreated_ = false;
        fc2Error error= fc2CreateContext(&context_);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 context"; 
            throw RuntimeError(ERROR_FC2_CREATE_CONTEXT, ssError.str());
        }
    }

    CameraDevice_fc2::~CameraDevice_fc2() 
    {
        if (capturing_) { stopCapture(); }

        if (convertedImageCreated_) { destroyConvertedImage(); }

        if (rawImageCreated_) { destroyRawImage(); }

        if (connected_) { disconnect(); }

        fc2Error error = fc2DestroyContext(context_);
        if ( error != FC2_ERROR_OK ) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to destroy FlyCapture2 context";
            throw RuntimeError(ERROR_FC2_DESTROY_CONTEXT, ssError.str());
        }
    }

    CameraLib CameraDevice_fc2::getCameraLib() 
    { 
        return guid_.getCameraLib(); 
    }

    void CameraDevice_fc2::connect() 
    {
        if (!connected_) 
        { 
            // Connect to camera
            fc2PGRGuid guid = getGuid_fc2();
            fc2Error error = fc2Connect(context_, &guid);
            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unabled to connect to FlyCapture2 device";
                throw RuntimeError(ERROR_FC2_CONNECT, ssError.str());
            }
            connected_ = true;

            // Get Camera information
            error = fc2GetCameraInfo( context_, &cameraInfo_ );
            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unabled to got FlyCapture2 camera info";
                throw RuntimeError(ERROR_FC2_GET_CAMERA_INFO, ssError.str());
            }
        }
    }

    void CameraDevice_fc2::disconnect()
    {
        if (capturing_) { stopCapture(); }
        if (connected_) 
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
        if (!connected_) 
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to start FlyCapture2 capture - not connected";
            throw RuntimeError(ERROR_FC2_START_CAPTURE, ssError.str());
        }

        if (!capturing_) 
        {
            createRawImage();
            createConvertedImage();

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
        if (capturing_) 
        {
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

    cv::Mat CameraDevice_fc2::grabImage()
    {
        cv::Mat image;  
        grabImage(image);
        return image;
    }

    void CameraDevice_fc2::grabImage(cv::Mat &image)
    {
        bool resize = false;

        grabImageCommon();

        // Only create new image if the size is incorrect - 
        // --------------------------------------------------------------------
        // TO DO ... currently only handles 8 bit mono images
        // --------------------------------------------------------------------
        if ((image.cols != rawImage_.cols) | (image.rows != rawImage_.rows))
        {
            resize = true;
        }
        
        // ---------------------------------------------------------------------
        // TO DO .. also test for image type.
        // ---------------------------------------------------------------------

        if (resize) {
            // -----------------------------------------------------------------
            // TO DO .. need to handle Pixel type conversions again currently
            // only supports 8 bit mono images
            // -----------------------------------------------------------------
            image = cv::Mat(rawImage_.rows, rawImage_.cols, CV_8UC1);
        }

        // Copy data -- TO DO might be able to do this without copying.
        // ---------------------------------------------------------------------
        unsigned char *pData0 = rawImage_.pData;
        unsigned char *pData1 = rawImage_.pData + rawImage_.dataSize - 1;
        std::copy(pData0,pData1,image.data);
    }


    bool CameraDevice_fc2::isColor()
    {
        return bool(cameraInfo_.isColorCamera);
    } 
    
    VideoMode CameraDevice_fc2::getVideoMode() 
    {
        fc2VideoMode vidMode_fc2;
        fc2FrameRate dummy;
        getVideoModeAndFrameRate(vidMode_fc2, dummy);
        return convertVideoMode_from_fc2(vidMode_fc2);
    } 

    FrameRate CameraDevice_fc2::getFrameRate() 
    {
        fc2VideoMode dummy;
        fc2FrameRate frmRate_fc2;
        getVideoModeAndFrameRate(dummy, frmRate_fc2);
        return convertFrameRate_from_fc2(frmRate_fc2);
    }

    ImageMode CameraDevice_fc2::getImageMode()
    {
        ImageMode mode = IMAGEMODE_UNSPECIFIED;
        VideoMode vidMode = getVideoMode();
        if (vidMode == VIDEOMODE_FORMAT7)
        {
            fc2Format7Configuration f7config; 
            fc2Mode mode_fc2;

            f7config = getFormat7Configuration();
            mode_fc2 = f7config.imageSettings.mode;
            mode = convertImageMode_from_fc2(mode_fc2);
        }
        return mode;
    } 

    VideoModeList CameraDevice_fc2::getAllowedVideoModes()
    {
        VideoModeList allowedVideoModes;
        bool supported;

        // Test for non-format7 vidModes
        // --------------------------------------------------------------------
        VideoModeList allVideoModes = getListOfVideoModes();
        VideoModeList::iterator vit;

        for (vit=allVideoModes.begin(); vit!=allVideoModes.end(); vit++)
        {
            VideoMode vidMode = *vit;
            if (vidMode == VIDEOMODE_FORMAT7) 
            { 
                // Skip format7 modes ... as we need a separate test for them.
                continue; 
            }

            FrameRateList allFrameRates = getListOfFrameRates();
            FrameRateList::iterator fit;
            supported = false;

            for (fit=allFrameRates.begin(); fit!=allFrameRates.end(); fit++)
            {
                FrameRate frmRate = *fit; 
                try
                {
                    supported |= isSupported(vidMode, frmRate);
                }
                catch (RuntimeError &runtimeError)
                {
                    // Device query can sometimes fail for some combinations of 
                    // vidMode and frmRate - handle failure gracefully by
                    // continuing to examine vidMode and frmRate combinations.                      
                    continue;
                }
            } 
            if (supported) 
            { 
                allowedVideoModes.push_back(vidMode); 
            }
        }

        // Test for format7 vidMode
        // --------------------------------------------------------------------
        ImageModeList allImageModes = getListOfImageModes();
        ImageModeList::iterator it;
        supported = false;

        for (it=allImageModes.begin(); it!=allImageModes.end(); it++)
        {
            ImageMode imgMode = *it;
            try
            {
                supported != isSupported(imgMode);
            }
            catch (RuntimeError &runtimeError)
            {
                // Fail gracefully in cases where the query for information
                // from the device fails.  
                continue;
            }
            if (supported) 
            { 
                // Only nee to validate support for on imageMode.
                allowedVideoModes.push_back(VIDEOMODE_FORMAT7);
                break; 
            }
        }
        return allowedVideoModes;
    }

    FrameRateList CameraDevice_fc2::getAllowedFrameRates(VideoMode vidMode)
    {
        FrameRateList allowedFramesRates;
        bool supported;

        if (vidMode == VIDEOMODE_FORMAT7) {
            allowedFramesRates.push_back(FRAMERATE_FORMAT7);
        } 
        else
        {
            FrameRateList allFrameRates = getListOfFrameRates();
            FrameRateList::iterator it;

            for (it=allFrameRates.begin(); it!=allFrameRates.end(); it++)  
            {
                FrameRate frmRate = *it; 
                try
                {
                    supported = isSupported(vidMode, frmRate);
                }
                catch (RuntimeError &runtimeError)
                {
                    // Continue checking even in case where device query fails
                    continue;
                }
                if (supported) 
                {
                    allowedFramesRates.push_back(frmRate);
                }
            }
        }
        return allowedFramesRates;
    }

    ImageModeList CameraDevice_fc2::getAllowedImageModes()
    {
        ImageModeList allowedImageModes;
        ImageModeList allImageModes = getListOfImageModes();
        ImageModeList::iterator it;
        bool supported;

        for (it=allImageModes.begin(); it!=allImageModes.end(); it++)
        {
            ImageMode imgMode = *it;
            try
            {
                supported = isSupported(imgMode);
            }
            catch (RuntimeError &runtimeError)
            {
                // Continue checking even is case where device query fails.
                continue;
            }
            if (supported)
            {
                allowedImageModes.push_back(imgMode);
            }
        }
        return allowedImageModes;
    }

    Property CameraDevice_fc2::getProperty(PropertyType propType)
    {
        fc2PropertyType propType_fc2;
        fc2Property prop_fc2;
        Property prop;

        propType_fc2 = convertPropertyType_to_fc2(propType);
        prop_fc2 = getProperty_fc2(propType_fc2);
        prop = convertProperty_from_fc2(prop_fc2);
        return prop;
    }

    PropertyInfo CameraDevice_fc2::getPropertyInfo(PropertyType propType)
    {
        fc2PropertyType propType_fc2;
        fc2PropertyInfo propInfo_fc2;
        PropertyInfo propInfo;

        propType_fc2 = convertPropertyType_to_fc2(propType);
        propInfo_fc2 = getPropertyInfo_fc2(propType_fc2);
        propInfo =  convertPropertyInfo_from_fc2(propInfo_fc2);
        return propInfo;
    }
   
    ImageInfo CameraDevice_fc2::getImageInfo()
    {
        // Note, this method grab a frame from the camera in order
        // to get the image information. 
        ImageInfo imgInfo;
        PixelFormat pixFormat;

        TriggerType savedTrigType = getTriggerType(); 

        if (savedTrigType == TRIGGER_EXTERNAL) 
        {
            // Temporarily set to internal trigger
            setTriggerInternal();
        }

        grabImageCommon();
        imgInfo.rows = rawImage_.rows;
        imgInfo.cols = rawImage_.cols;
        imgInfo.stride = rawImage_.stride;
        imgInfo.dataSize = rawImage_.dataSize;
        pixFormat = convertPixelFormat_from_fc2(rawImage_.format);
        imgInfo.pixelFormat = pixFormat; 

        if (savedTrigType == TRIGGER_EXTERNAL) 
        {
            // Return to external trigger
            setTriggerExternal();
        }

        return imgInfo;
    }

    void CameraDevice_fc2::setProperty(Property prop)
    {
        fc2Property prop_fc2;
        prop_fc2 = convertProperty_to_fc2(prop);
        setProperty(prop_fc2);
    }

    bool CameraDevice_fc2::isSupported(VideoMode vidMode, FrameRate frmRate)
    {
        fc2VideoMode vidMode_fc2;
        fc2FrameRate frmRate_fc2;
        BOOL supported;
        fc2Error error;

        try 
        {
            vidMode_fc2 = convertVideoMode_to_fc2(vidMode);
            frmRate_fc2 = convertFrameRate_to_fc2(frmRate);
        }
        catch (RuntimeError &runtimeError)
        {
            // Conversion failed - combination not supported
            return false;
        }

        error = fc2GetVideoModeAndFrameRateInfo(
                context_, 
                vidMode_fc2, 
                frmRate_fc2, 
                &supported
                ); 

        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 VideoMode and ";
            ssError << "FrameRate information";
            throw RuntimeError(ERROR_FC2_GET_VIDEOMODE_FRAMERATE_INFO, ssError.str());
            return false;
        }

        return (supported == TRUE ? true : false);
    }

    bool CameraDevice_fc2::isSupported(ImageMode imgMode)
    {
        fc2Error error;
        fc2Mode mode_fc2;
        fc2Format7Info info_fc2;
        BOOL supported;

        try 
        {
            mode_fc2 = convertImageMode_to_fc2(imgMode);
        }
        catch (RuntimeError &runtimeError)
        {
            // If conversion failed - mode not supported
            return false;
        }

        info_fc2.mode = mode_fc2;
        error = fc2GetFormat7Info(context_, &info_fc2, &supported);
        if (error != FC2_ERROR_OK)
        {   
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get format7 information for given mode";
            throw RuntimeError(ERROR_FC2_GET_FORMAT7_INFO, ssError.str());
        }
        return (supported==TRUE ? true : false);
    }

    void CameraDevice_fc2::setVideoMode(VideoMode vidMode, FrameRate frmRate) 
    {
        if (vidMode != VIDEOMODE_FORMAT7) 
        {
            // For all non-format7 video modes
            fc2VideoMode vidMode_fc2 = convertVideoMode_to_fc2(vidMode);
            fc2FrameRate frmRate_fc2 = convertFrameRate_to_fc2(frmRate);
            fc2Error error = fc2SetVideoModeAndFrameRate(
                    context_, 
                    vidMode_fc2, 
                    frmRate_fc2
                    );
            if (error != FC2_ERROR_OK)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set VideoMode and frame rate";
                throw RuntimeError(
                        ERROR_FC2_SET_VIDEOMODE_AND_FRAMERATE, 
                        ssError.str()
                        );
            }
        }
        else 
        {
            // For format 7 video modes ... 
            //
            // Temporary. TO DO uses setVideoModeFormat7 which needs to be 
            // worked on to make it more flexible.
            ImageModeList allowedImageModes = getAllowedImageModes();
            if (allowedImageModes.empty())
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set Format7 video mode not support imageModes";
                throw RuntimeError(ERROR_FC2_SET_VIDEOMODE_FORMAT7, ssError.str());
            }
            ImageMode mode = allowedImageModes.front();
            setVideoModeToFormat7(mode);
        }
    }


    void CameraDevice_fc2::setFormat7ImageMode(ImageMode imgMode) 
    {
        // -------------------------------------------------
        // TO DO ...
        // -------------------------------------------------
    }

    void CameraDevice_fc2::setTriggerInternal()
    {
        fc2TriggerMode trigMode = getTriggerMode_fc2();
        trigMode.onOff = FALSE;
        setTriggerMode(trigMode);
    }

    void CameraDevice_fc2::setTriggerExternal()
    {
        // ------------------------------------------------
        // TO DO ... not really finished yet
        // ------------------------------------------------
        //
        // Currently only sets to mode 0, doesn't check for
        // support, doesn't set polarity, etc.
        // ------------------------------------------------ 
        fc2TriggerMode trigMode = getTriggerMode_fc2();
        trigMode.onOff = TRUE;
        trigMode.mode = 0;
        setTriggerMode(trigMode);
    }

    TriggerType CameraDevice_fc2::getTriggerType()
    {
        fc2TriggerMode trigMode = getTriggerMode_fc2();
        if ((trigMode.onOff == TRUE) && (trigMode.mode != 3)) // Note, mode 3 is frame skip
        {
            return TRIGGER_EXTERNAL;
        }
        else
        {
            return TRIGGER_INTERNAL;
        }
    }

    std::string CameraDevice_fc2::toString()
    {
        fc2Error error;
        std::stringstream ss; 

        ss << std::endl;
        ss << "------------------ " << std::endl;
        ss << "Camera Information " << std::endl;
        ss << "------------------ " << std::endl;
        ss << std::endl;

        if ( connected_ ) { 
            ss << " guid:           " << guid_ << std::endl;
            ss << " serial number:  " << cameraInfo_.serialNumber << std::endl;
            ss << " camera vendor:  " << cameraInfo_.vendorName << std::endl;
            ss << " camera model:   " << cameraInfo_.modelName << std::endl;
            ss << " sensor          " << std::endl;
            ss << "   type:         " << cameraInfo_.sensorInfo << std::endl;
            ss << "   resolution:   " << cameraInfo_.sensorResolution << std::endl;
            ss << " firmware        " << std::endl;     
            ss << "   version:      " << cameraInfo_.firmwareVersion << std::endl;
            ss << "   build time:   " << cameraInfo_.firmwareBuildTime << std::endl;
            ss << " color camera:   " << std::boolalpha << (bool) cameraInfo_.isColorCamera << std::endl;
            ss << " interface:      " << getInterfaceTypeString_fc2(cameraInfo_.interfaceType) << std::endl;
            ss << std::endl;
        }
        else 
        {
            ss << " Camera not connected " << std::endl << std::endl;
            
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
    // -------------------------------------------------------------------------

    fc2PGRGuid CameraDevice_fc2::getGuid_fc2()
    {
        return guid_.getValue_fc2();
    }

    void CameraDevice_fc2::createRawImage()
    {
        if (rawImageCreated_) { destroyRawImage(); }

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

    void CameraDevice_fc2::createConvertedImage()
    {
        if (convertedImageCreated_) {destroyConvertedImage();}

        fc2Error error = fc2CreateImage(&convertedImage_);

        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 image";
            throw RuntimeError(ERROR_FC2_CREATE_IMAGE, ssError.str());
        }

        convertedImageCreated_ = true;
    }

    void CameraDevice_fc2::destroyRawImage()
    {
        if (rawImageCreated_) 
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

    void CameraDevice_fc2::destroyConvertedImage()
    {
        if (convertedImageCreated_) 
        {
            fc2Error error = fc2DestroyImage(&convertedImage_);

            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to destroy FlyCapture2 image";
                throw RuntimeError(ERROR_FC2_DESTROY_IMAGE, ssError.str());
            }
            
            convertedImageCreated_ = false;
        }
    }

    void CameraDevice_fc2::grabImageCommon()
    {
        fc2Error error;

        if (!capturing_) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to grab Image - not capturing";
            throw RuntimeError(ERROR_FC2_GRAB_IMAGE, ssError.str());
        }

        // Retrieve image from buffer
        error = fc2RetrieveBuffer(context_, &rawImage_);
        if ( error != FC2_ERROR_OK ) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to retrieve image from buffer";
            throw RuntimeError(ERROR_FC2_RETRIEVE_BUFFER, ssError.str());
        }

        // --------------------------------------------------------------
        // Convert image ...
        // --------------------------------------------------------------
        //
        //printImageInfo_fc2(rawImage_);
        //std::cout << std::flush;
        //
        //error = fc2ConvertImageTo(
        //        FC2_PIXEL_FORMAT_MONO8, 
        //        &rawImage_, 
        //        &convertedImage_
        //        );
        //if ( error != FC2_ERROR_OK ) 
        //{
        //    std::stringstream ssError;
        //    ssError << __PRETTY_FUNCTION__;
        //    ssError << ": unable to convert image";
        //    throw RuntimeError(ERROR_FC2_CONVERT_IMAGE, ssError.str());
        //}
        //
        //printImageInfo_fc2(convertedImage_);
        //std::cout << std::flush;
        //----------------------------------------------------------------
    }


    // fc2 get methods
    // ---------------

    void CameraDevice_fc2::getVideoModeAndFrameRate(
            fc2VideoMode &vidMode, 
            fc2FrameRate &frmRate
            )
    {
        fc2Error error = fc2GetVideoModeAndFrameRate(context_, &vidMode, &frmRate);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 VideoMode and FrameRate";
            throw RuntimeError(ERROR_FC2_GET_VIDEOMODE_AND_FRAMERATE, ssError.str());
        }
    }

    fc2PropertyInfo CameraDevice_fc2::getPropertyInfo_fc2(fc2PropertyType propType)
    {
        fc2Error error;
        fc2PropertyInfo propInfo;
        propInfo.type = propType;

        error = fc2GetPropertyInfo(context_, &propInfo);
        if (error != FC2_ERROR_OK)  
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError < ": unable to get FlyCapture2 properyInfo";
            throw RuntimeError(ERROR_FC2_GET_PROPERTY_INFO, ssError.str());
        }
        return propInfo;
    }

    fc2Property CameraDevice_fc2::getProperty_fc2(fc2PropertyType propType)
    {
        fc2Error error;
        fc2Property prop;
        prop.type = propType;

        error = fc2GetProperty(context_, &prop);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 property";
            throw RuntimeError(ERROR_FC2_GET_PROPERTY, ssError.str());
        }
        return prop;
    }

    fc2Format7Configuration CameraDevice_fc2::getFormat7Configuration()
    {
        fc2Error error;
        fc2Format7Configuration config;
        error = fc2GetFormat7Configuration(
                context_, 
                &(config.imageSettings),
                &(config.packetSize),
                &(config.percentage)
                );
        if (error != FC2_ERROR_OK)
        { 
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to get FlyCapture2 format 7 configuration"; 
            throw RuntimeError(ERROR_FC2_GET_FORMAT7_CONFIGURATION, ssError.str());
        }
        return config;
    }

    fc2TriggerMode CameraDevice_fc2::getTriggerMode_fc2()
    {
        fc2Error error;
        fc2TriggerMode triggerMode;
        error = fc2GetTriggerMode(context_, &triggerMode);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 TriggerMode";
            throw RuntimeError(ERROR_FC2_GET_TRIGGER_MODE, ssError.str());
        }
        return triggerMode;
    }

    fc2TriggerModeInfo CameraDevice_fc2::getTriggerModeInfo_fc2()
    {
        fc2Error error;
        fc2TriggerModeInfo triggerModeInfo;
        error = fc2GetTriggerModeInfo(context_, &triggerModeInfo);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 TriggerModeInfo";
            throw RuntimeError(ERROR_FC2_GET_TRIGGER_MODE_INFO, ssError.str());
        }
    }


    // fc2 set methods
    // ---------------

    void CameraDevice_fc2::setProperty(fc2Property prop)
    {
        fc2Error error = fc2SetProperty(context_, &prop);
        if (error != FC2_ERROR_OK) 
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set FlyCapture2 prop";
            throw RuntimeError(ERROR_FC2_SET_PROPERTY, ssError.str());
        }
    }

    void CameraDevice_fc2::setTriggerMode(fc2TriggerMode trigMode)
    {
        fc2Error error = fc2SetTriggerMode(context_, &trigMode);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set FlyCapture2 TriggerMode";
            throw RuntimeError(ERROR_FC2_SET_TRIGGER_MODE, ssError.str());
        }
    }

    // Temporary methods
    // ------------------------------------------------------------------------

    void CameraDevice_fc2::setVideoModeToFormat7(ImageMode mode)
    {
        fc2Mode mode_fc2;
        mode_fc2 = convertImageMode_to_fc2(mode);
        setVideoModeToFormat7(mode_fc2);
    }

    void CameraDevice_fc2::setVideoModeToFormat7(fc2Mode mode)
    {
        fc2Error error;
        fc2Format7Info format7Info;
        fc2Format7ImageSettings imageSettings;
        fc2Format7PacketInfo packetInfo;
        fc2PixelFormat defaultPixelFormat;
        unsigned int packetSize;
        float percentage; 
        BOOL supported;
        BOOL settingsAreValid;

        // Get the format 7 info
        format7Info.mode = mode;
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

        if (0) // Print format7 information for selected mode
        {
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
            ssError << ": unable to get FlyCapture2 format 7 configuration"; 
            throw RuntimeError(ERROR_FC2_GET_FORMAT7_CONFIGURATION, ssError.str());
        }

        if (0) // Print current configuration settings
        {
            printFormat7Configuration_fc2(imageSettings,packetSize,percentage);
        }

        // Create desired format7 configuration
        imageSettings.mode = format7Info.mode;
        imageSettings.offsetX = 0;
        imageSettings.offsetY = 0;
        imageSettings.width = format7Info.maxWidth;
        imageSettings.height = format7Info.maxHeight;
        imageSettings.pixelFormat = FC2_PIXEL_FORMAT_MONO8;

        // Check that settings are valid and get packet info
        error = fc2ValidateFormat7Settings(
                context_, 
                &imageSettings, 
                &settingsAreValid,
                &packetInfo
                );
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to validate FlyCapture2 format 7 settings"; 
            throw RuntimeError(ERROR_FC2_VALIDATE_FORMAT7_SETTINGS, ssError.str());
        }
        if (!settingsAreValid)
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to validate FlyCapture2 format 7 settings"; 
            throw RuntimeError(ERROR_FC2_INVALID_FORMAT7_SETTINGS, ssError.str());
        }

        if (0)  // Print packet info
        {
            printFormat7ImageSettings_fc2(imageSettings);
            printFormat7PacketInfo_fc2(packetInfo);
        }

        // Set format 7 configuration settings
        error = fc2SetFormat7Configuration(context_, &imageSettings, 100);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to sete FlyCapture2 format 7 configuration"; 
            throw RuntimeError(ERROR_FC2_SET_FORMAT7_CONFIGURATION, ssError.str());
        }
    }
}
#endif

