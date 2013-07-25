#ifdef WITH_FC2
#include "camera_device_fc2.hpp"
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <bitset>


namespace bias {

    const unsigned int MAX_CYCLE_SECONDS = 128;
    const unsigned int MAX_CYCLE_COUNT = 8000; 
    const unsigned int MAX_CYCLE_OFFSET = 3072;
    const unsigned int USEC_PER_CYCLE_COUNT = (1000000/MAX_CYCLE_COUNT);
    const unsigned int CYCLE_OFFSET_MASK = 0b111111110000;

    CameraDevice_fc2::CameraDevice_fc2() : CameraDevice()
    {
        initialize();
    }


    CameraDevice_fc2::CameraDevice_fc2(Guid guid) : CameraDevice(guid)
    {
        initialize();
        fc2Error error= fc2CreateContext(&context_);
        if (error != FC2_ERROR_OK) 
        {
            // TODO ... shouldn't throw exception in construction change this
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create FlyCapture2 context"; 
            throw RuntimeError(ERROR_FC2_CREATE_CONTEXT, ssError.str());
        }
    }


    void CameraDevice_fc2::initialize()
    {
        isFirst_ = true;
        rawImageCreated_ = false;
        convertedImageCreated_ = false;
        haveEmbeddedTimeStamp_ = false;
        timeStamp_.seconds = 0;
        timeStamp_.microSeconds = 0;
        cycleSecondsLast_ = 0;
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
            setupTimeStamping();

            fc2Error error = fc2StartCapture(context_);
            if (error != FC2_ERROR_OK) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to start FlyCapture2 capture";
                throw RuntimeError(ERROR_FC2_START_CAPTURE, ssError.str());
            }
            capturing_ = true;
            isFirst_ = true;
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

        // Use either raw or converted image
        fc2Image *imagePtr_fc2;
        if (useConverted)
        {
            imagePtr_fc2 = &convertedImage_;
        }
        else
        {
            imagePtr_fc2 = &rawImage_;
        }

        // Check image size and type
        if ((image.cols != (imagePtr_fc2->cols)) | (image.rows != (imagePtr_fc2->rows)))
        {
            resize = true;
        }

        // Check image type
        int currType = CV_MAKETYPE(image.depth(),image.channels());
        int compType = getCompatibleOpencvFormat(imagePtr_fc2->format);
        
        // If size or type changed remake image
        if ((resize) || (currType != compType)) {
            image = cv::Mat(imagePtr_fc2->rows, imagePtr_fc2->cols, compType);
        }

        // Copy data -- TO DO might be able to do this without copying.
        unsigned char *pData0 = imagePtr_fc2->pData;
        unsigned char *pData1 = imagePtr_fc2->pData + imagePtr_fc2->dataSize - 1;
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
                supported |= isSupported(imgMode);
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


    Format7Settings CameraDevice_fc2::getFormat7Settings()
    {
        fc2Error error_fc2;
        fc2Format7ImageSettings settings_fc2;
        unsigned int packetSize;
        float percentage; 
        Format7Settings settings;

        // Get Current format7 image settings
        error_fc2 = fc2GetFormat7Configuration(
                context_, 
                &settings_fc2,
                &packetSize,
                &percentage
                );
        if (error_fc2 != FC2_ERROR_OK)
        { 
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to get FlyCapture2 format 7 configuration"; 
            throw RuntimeError(ERROR_FC2_GET_FORMAT7_CONFIGURATION, ssError.str());
        }
        settings.mode = convertImageMode_from_fc2(settings_fc2.mode);
        settings.offsetX = settings_fc2.offsetX;
        settings.offsetY = settings_fc2.offsetY;
        settings.width = settings_fc2.width;
        settings.height = settings_fc2.height;
        settings.pixelFormat = convertPixelFormat_from_fc2(settings_fc2.pixelFormat);
        return settings;
    }


    Format7Info CameraDevice_fc2::getFormat7Info(ImageMode imgMode)
    {
        fc2Error error_fc2;
        fc2Format7Info format7Info_fc2;
        BOOL supported;
        Format7Info format7Info;

        format7Info_fc2.mode = convertImageMode_to_fc2(imgMode);
        error_fc2 = fc2GetFormat7Info(context_, &format7Info_fc2, &supported);
        if (error_fc2 != FC2_ERROR_OK) 
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to get FlyCapture2 format7 information"; 
            throw RuntimeError(ERROR_FC2_GET_FORMAT7_INFO, ssError.str());
        }

        format7Info.mode = imgMode;
        format7Info.supported = supported;
        if (supported) 
        {
            format7Info.maxWidth = format7Info_fc2.maxWidth;
            format7Info.maxHeight = format7Info_fc2.maxHeight;
            format7Info.offsetHStepSize = format7Info_fc2.offsetHStepSize;
            format7Info.offsetVStepSize = format7Info_fc2.offsetVStepSize;
            format7Info.imageHStepSize = format7Info_fc2.imageHStepSize;
            format7Info.imageVStepSize = format7Info_fc2.imageVStepSize;
            format7Info.pixelFormatBitField = format7Info_fc2.pixelFormatBitField;
            format7Info.vendorPixelFormatBitField = format7Info_fc2.vendorPixelFormatBitField;
            format7Info.packetSize = format7Info_fc2.packetSize;
            format7Info.minPacketSize = format7Info_fc2.minPacketSize;
            format7Info.maxPacketSize = format7Info_fc2.maxPacketSize;
            format7Info.percentage = format7Info.percentage;
        }
        return format7Info;
    }


    bool CameraDevice_fc2::validateFormat7Settings(Format7Settings settings)
    {
        fc2Format7ImageSettings settings_fc2 = convertFormat7Settings_to_fc2(settings);

        BOOL settingsAreValid_fc2;
        fc2Format7PacketInfo packetInfo_fc2;

        fc2Error error_fc2 = fc2ValidateFormat7Settings(
                context_, 
                &settings_fc2, 
                &settingsAreValid_fc2,
                &packetInfo_fc2
                );

        if (error_fc2 != FC2_ERROR_OK)
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to validate FlyCapture2 format 7 settings"; 
            throw RuntimeError(ERROR_FC2_VALIDATE_FORMAT7_SETTINGS, ssError.str());
        }
        return bool(settingsAreValid_fc2);
    }


    void CameraDevice_fc2::setFormat7Configuration(Format7Settings settings, float percentSpeed)
    {
        fc2Format7ImageSettings settings_fc2 = convertFormat7Settings_to_fc2(settings);
        fc2Error error_fc2 = fc2SetFormat7Configuration(context_, &settings_fc2, percentSpeed);
        if (error_fc2 != FC2_ERROR_OK)
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to set FlyCapture2 format 7 configuration"; 
            throw RuntimeError(ERROR_FC2_SET_FORMAT7_CONFIGURATION, ssError.str());
        }
    }


    PixelFormatList CameraDevice_fc2::getListOfSupportedPixelFormats(ImageMode imgMode)
    {
        PixelFormatList formatList = getListOfPixelFormats();
        PixelFormatList supportedList;

        Format7Info info = getFormat7Info(imgMode);
        if (info.supported)
        {
            PixelFormatList::iterator it;
            for (it=formatList.begin(); it!=formatList.end(); it++)
            {
                PixelFormat format = *it;
                fc2PixelFormat format_fc2 = convertPixelFormat_to_fc2(format);
                if (format_fc2 & info.pixelFormatBitField)
                {
                    supportedList.push_back(format);
                }
            }
        }
        return supportedList;
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
            //setVideoModeToFormat7(IMAGEMODE_1);
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
        // support, etc.
        // ------------------------------------------------ 
        fc2Error error = fc2SetGPIOPinDirection(context_, 0, 0);
        if (error != FC2_ERROR_OK) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set GPIO direction";
            throw RuntimeError(ERROR_FC2_CREATE_IMAGE, ssError.str());
        }
        
        fc2TriggerMode trigMode = getTriggerMode_fc2();
        trigMode.onOff = TRUE;
        trigMode.mode = 0;
        trigMode.source = 0;
        trigMode.parameter = 0;
        trigMode.polarity = 0;
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

    TimeStamp CameraDevice_fc2::getImageTimeStamp()
    {
        return timeStamp_;
    }

    std::string CameraDevice_fc2::getVendorName()
    {
        return cameraInfo_.vendorName;
    }

    std::string CameraDevice_fc2::getModelName()
    {
        return cameraInfo_.modelName;
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
    }


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
        std::cout << __PRETTY_FUNCTION__ << std::endl;

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

        updateTimeStamp();
        isFirst_ = false;

        // Convert image to suitable format 
        fc2PixelFormat convertedFormat = getSuitablePixelFormat(rawImage_.format);

        std::cout << "original Format:  " << getPixelFormatString_fc2(rawImage_.format) << std::endl;
        std::cout << "converted Format: " << getPixelFormatString_fc2(convertedFormat) << std::endl;

        if (rawImage_.format != convertedFormat)
        {
            std::cout << "converting ... ";
            useConverted = true;
            error = fc2ConvertImageTo(
                    convertedFormat,
                    &rawImage_, 
                    &convertedImage_
                    );
            std::cout << "error = " << error << std::endl;

            if ( error != FC2_ERROR_OK ) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to convert image";
                throw RuntimeError(ERROR_FC2_CONVERT_IMAGE, ssError.str());
            }
        }
        else
        {
            useConverted = false;
        }

        std::cout << std::endl;
        

    }


    void CameraDevice_fc2::setupTimeStamping()
    {
        fc2Error error;
        fc2EmbeddedImageInfo embeddedInfo;

        error = fc2GetEmbeddedImageInfo(context_, &embeddedInfo); 
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get FlyCapture2 embedded image info";
            throw RuntimeError(ERROR_FC2_GET_EMBEDDED_IMAGE_INFO, ssError.str());
        }

        // If embedded time stamping available enable it
        if (embeddedInfo.timestamp.available == TRUE)
        {
            haveEmbeddedTimeStamp_ = true;
            embeddedInfo.timestamp.onOff = true;
        }
        else
        {
            haveEmbeddedTimeStamp_ = false;
            embeddedInfo.timestamp.onOff = false;
        }

        error = fc2SetEmbeddedImageInfo(context_, &embeddedInfo); 
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set FlyCapture2 embedded image info";
            throw RuntimeError(ERROR_FC2_SET_EMBEDDED_IMAGE_INFO, ssError.str());
        }

        // Initalize time stamp data
        timeStamp_.seconds = 0;
        timeStamp_.microSeconds = 0;
        cycleSecondsLast_ = 0;
    }


    void CameraDevice_fc2::updateTimeStamp()
    {
        fc2TimeStamp timeStamp_fc2 = fc2GetImageTimeStamp(&rawImage_);

        if (haveEmbeddedTimeStamp_)
        {
            // DEVEL - get raw time stamp data from image 
            // ----------------------------------------------------------------
            //unsigned int  stamp;
            //unsigned char* pStamp = (unsigned char*) &stamp;
            //for (int i=0; i<4; i++)
            //{
            //    pStamp[i] = rawImage_.pData[3-i];
            //}
            // ----------------------------------------------------------------

            if (isFirst_)
            {
                timeStamp_.seconds = 0;
                timeStamp_.microSeconds = 0;
                cycleSecondsLast_ = timeStamp_fc2.cycleSeconds;
            }
            else
            {
                unsigned int cycleSeconds;
                unsigned int cycleCount;
                unsigned int cycleOffset;
                unsigned int deltaCycleSeconds;

                // DEVEL - convert raw time stamp data to cycleSeconds, etc.
                // --------------------------------------------------------------------------
                //cycleSeconds = (stamp >> 25) & 0b1111111;
                //cycleCount   = (stamp >> 12) & 0b1111111111111;  
                //cycleOffset  = (stamp >> 0 ) & 0b111111110000;   
                //std::cout << cycleSeconds << ", " << cycleCount << ", " << cycleOffset << std::endl;
                //std::cout << cycleSeconds << ", " << cycleCount << ", " << cycleOffset << std::endl;
                //std::cout << std::endl;
                // ---------------------------------------------------------------------------

                cycleSeconds = timeStamp_fc2.cycleSeconds;
                cycleCount = timeStamp_fc2.cycleCount;
                cycleOffset = timeStamp_fc2.cycleOffset;
                cycleOffset &= CYCLE_OFFSET_MASK;
                if (timeStamp_fc2.cycleSeconds >= cycleSecondsLast_)
                {
                    deltaCycleSeconds = cycleSeconds; 
                    deltaCycleSeconds -= cycleSecondsLast_;
                }
                else
                {
                    deltaCycleSeconds = MAX_CYCLE_SECONDS; 
                    deltaCycleSeconds += timeStamp_fc2.cycleSeconds; 
                    deltaCycleSeconds -= cycleSecondsLast_;
                }
                timeStamp_.seconds += (unsigned long long)(deltaCycleSeconds);
                timeStamp_.microSeconds = USEC_PER_CYCLE_COUNT*cycleCount;
                timeStamp_.microSeconds += (USEC_PER_CYCLE_COUNT*cycleOffset)/MAX_CYCLE_OFFSET;
                cycleSecondsLast_ = timeStamp_fc2.cycleSeconds;
            }
        }
        else
        {
            timeStamp_.seconds = (unsigned long long)(timeStamp_fc2.seconds);
            timeStamp_.microSeconds = timeStamp_fc2.microSeconds;
        }
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
        std::cout << __PRETTY_FUNCTION__ << std::endl;

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
            unsigned int test0 = format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RAW8;
            unsigned int test1 = format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_MONO8;
            std::cout << "FC2_PIXEL_FORMAT_RAW8  | pixelFormatBitField = "<< std::bitset<32>(test0) << std::endl;
            std::cout << "FC2_PIXEL_FORMAT_MONO8 | pixelFormatBitField = "<< std::bitset<32>(test1) << std::endl;
        }

        // Select pixel format currently - this is a bit of a hack 
        fc2PixelFormat pixelFormat;
        bool havePixelFormat = false;

        if (isColor())
        {
            // Camera is color - try to find a suitable color format
            if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_BGRU)
            {
                pixelFormat = FC2_PIXEL_FORMAT_BGRU;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RGBU)
            {
                pixelFormat = FC2_PIXEL_FORMAT_RGBU;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_BGR)
            {
                pixelFormat = FC2_PIXEL_FORMAT_BGR;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RGB)
            {
                pixelFormat = FC2_PIXEL_FORMAT_RGB;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_BGRU16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_BGRU16;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_BGR16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_BGR16;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RGB16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_RGB16;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_S_RGB16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_S_RGB16;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_411YUV8)
            {
                pixelFormat = FC2_PIXEL_FORMAT_411YUV8;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_422YUV8)
            {
                pixelFormat = FC2_PIXEL_FORMAT_422YUV8;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_444YUV8)
            {
                pixelFormat = FC2_PIXEL_FORMAT_444YUV8;
                havePixelFormat = true;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_422YUV8_JPEG)
            {
                pixelFormat = FC2_PIXEL_FORMAT_422YUV8_JPEG;
                havePixelFormat = true;
            }
        }


        if (!havePixelFormat)
        {
            // This is a monochrome camera or couldn't find color pixel format which will work.
            if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RAW8)
            {
                pixelFormat = FC2_PIXEL_FORMAT_RAW8;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_MONO8)
            {
                pixelFormat = FC2_PIXEL_FORMAT_MONO8;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RAW16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_RAW16;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_MONO16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_MONO16;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_RAW12)
            {
                pixelFormat = FC2_PIXEL_FORMAT_RAW12;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_MONO12)
            {
                pixelFormat = FC2_PIXEL_FORMAT_MONO12;
            }
            else if (format7Info.pixelFormatBitField & FC2_PIXEL_FORMAT_S_MONO16)
            {
                pixelFormat = FC2_PIXEL_FORMAT_S_MONO16;
            }
            else
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": no supported pixel formats";
                throw RuntimeError(ERROR_FC2_NO_SUPPORTED_PIXEL_FORMAT, ssError.str());
            }
        }

        // Create desired format7 configuration
        imageSettings.mode = format7Info.mode;
        imageSettings.offsetX = 0;
        imageSettings.offsetY = 0;
        imageSettings.width = format7Info.maxWidth;
        imageSettings.height = format7Info.maxHeight;
        imageSettings.pixelFormat = pixelFormat;

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
            ssError << ": FlyCapture2 format 7 settings invalid"; 
            throw RuntimeError(ERROR_FC2_INVALID_FORMAT7_SETTINGS, ssError.str());
        }

        if (0)  // Print packet info
        {
            printFormat7ImageSettings_fc2(imageSettings);
            printFormat7PacketInfo_fc2(packetInfo);
        }

        // Set format 7 configuration settings
        error = fc2SetFormat7Configuration(context_, &imageSettings, 100.0);
        if (error != FC2_ERROR_OK)
        {
            std::stringstream ssError; 
            ssError << __PRETTY_FUNCTION__; 
            ssError << ": unable to sete FlyCapture2 format 7 configuration"; 
            throw RuntimeError(ERROR_FC2_SET_FORMAT7_CONFIGURATION, ssError.str());
        }

        // Get Current format7 image settings
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

        if (1) // Print current configuration settings
        {
            printFormat7Configuration_fc2(imageSettings,packetSize,percentage);
        }

        std::cout << std::endl;
    }
}
#endif

