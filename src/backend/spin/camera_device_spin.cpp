#ifdef WITH_SPIN
#include "camera_device_spin.hpp"
#include "utils_spin.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <bitset>

// Temporary ???
#include "node_map_utils.hpp"
#include "enum_node_spin.hpp"
#include "string_node_spin.hpp"
#include <fstream>


namespace bias {


    CameraDevice_spin::CameraDevice_spin() : CameraDevice()
    {
        //initialize();
    }


    CameraDevice_spin::CameraDevice_spin(Guid guid) : CameraDevice(guid)
    {
        //initialize();
        spinError err = spinSystemGetInstance(&hSystem_);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create Spinnaker context, error = " << err; 
            throw RuntimeError(ERROR_SPIN_CREATE_CONTEXT, ssError.str());
        }
    }


    //void CameraDevice_spin::initialize()
    //{

    //    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    //    //isFirst_ = true;
    //    //rawImageCreated_ = false;
    //    //convertedImageCreated_ = false;
    //    //haveEmbeddedTimeStamp_ = false;
    //    //timeStamp_.seconds = 0;
    //    //timeStamp_.microSeconds = 0;
    //    //cycleSecondsLast_ = 0;
    //}


    CameraDevice_spin::~CameraDevice_spin() 
    {

        if (capturing_) 
        { 
            stopCapture(); 
        }

        if (connected_) 
        { 
            disconnect(); 
        }

        spinError err = spinSystemReleaseInstance(hSystem_);
        if ( err != SPINNAKER_ERR_SUCCESS ) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to destroy Spinnaker context, error = " << err;
            throw RuntimeError(ERROR_SPIN_DESTROY_CONTEXT, ssError.str());
        }
    }


    CameraLib CameraDevice_spin::getCameraLib() 
    { 
        //std::cout << __PRETTY_FUNCTION__ << std::endl;
        return guid_.getCameraLib(); 
    }


    void CameraDevice_spin::connect() 
    {
        //std::cout << __PRETTY_FUNCTION__ << std::endl;

        if (!connected_) 
        {
            spinError err = SPINNAKER_ERR_SUCCESS;
            spinCameraList hCameraList = nullptr;

            // Create empty camera list
            err = spinCameraListCreateEmpty(&hCameraList);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to create Spinnaker empty camera list, error=" << err;
                throw RuntimeError(ERROR_SPIN_CREATE_CAMERA_LIST, ssError.str());
            }

            // Retrieve list of cameras from system
            err = spinSystemGetCameras(hSystem_, hCameraList);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to enumerate Spinnaker cameras, error=" << err;
                throw RuntimeError(ERROR_SPIN_ENUMERATE_CAMERAS, ssError.str());
            }

            err = spinCameraListGetBySerial(hCameraList, guid_.toString().c_str() , &hCamera_);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to get Spinnaker camera from list, error = " << err;
                throw RuntimeError(ERROR_SPIN_GET_CAMERA, ssError.str());
            }
            connected_ = true;

            // Clear Spinnaker camera list
            err = spinCameraListClear(hCameraList);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to clear Spinnaker camera list, error=" << err;
                throw RuntimeError(ERROR_SPIN_CLEAR_CAMERA_LIST, ssError.str());
            }

            // Destroy Spinnaker camera list
            err = spinCameraListDestroy(hCameraList);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to destroy Spinnaker camera list, error=" << err;
                throw RuntimeError(ERROR_SPIN_DESTROY_CAMERA_LIST, ssError.str());
            }


            // Initialize camera
            err = spinCameraInit(hCamera_);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                hCamera_ = nullptr;
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to initialize Spinnaker camera, error=" << err;
                throw RuntimeError(ERROR_SPIN_GET_TLDEVICE_NODE_MAP, ssError.str());
            }
            connected_ = true;

            // Setup node maps for TLDevice and camera and get camera info
            nodeMapTLDevice_ = NodeMapTLDevice_spin(hCamera_);
            nodeMapCamera_ = NodeMapCamera_spin(hCamera_);
            cameraInfo_ = nodeMapTLDevice_.cameraInfo();
            cameraInfo_.print();


            // ---

            std::cout << "# TLDevice nodes: " << (nodeMapTLDevice_.numberOfNodes()) << std::endl;
            std::cout << "# Camera nodes:   " << (nodeMapCamera_.numberOfNodes()) << std::endl;
            std::cout << std::endl;

            //std::vector<EnumNode_spin> enumNodeVec = nodeMapTLDevice_.nodes<EnumNode_spin>();
            std::vector<EnumNode_spin> enumNodeVec = nodeMapCamera_.nodes<EnumNode_spin>();
            for (auto enumNode : enumNodeVec)
            {

                std::cout << "name: " << enumNode.name() << ", numberOfEntries: " << enumNode.numberOfEntries() << std::endl;
                std::vector<EntryNode_spin> entryNodeVec = enumNode.entries();
                EntryNode_spin currEntryNode = enumNode.currentEntry();
                std::cout << "  current: " << currEntryNode.name() << std::endl;
                for (auto entryNode : entryNodeVec)
                {
                    std::cout << "  name:    " << entryNode.name() << ", " << entryNode.displayName() << std::endl;
                }
            }
            
            

            std::vector<spinPixelFormatEnums> pixelFormatVec = getSupportedPixelFormats_spin();



            // --------------------------------------------------------------------
            // TODO: - setup strobe output on GPIO pin?? Is this possible?
            // --------------------------------------------------------------------


            // DEVEL
            // ---------------------------------------------------------------
            //std::ofstream fout;

            //std::map<std::string, std::string> nodeNameToValueMap = getStringNodeNameToValueMap(hNodeMapCamera_);
            //fout.open("node_name_to_value_map_camera.txt");
            //for (auto kv : nodeNameToValueMap)
            //{
            //    fout << kv.first << ",   " << kv.second << std::endl;
            //}
            //fout.close();

        }
    }


    void CameraDevice_spin::disconnect()
    {
        // std::cout << __PRETTY_FUNCTION__ << std::endl;

        if (capturing_) 
        { 
            stopCapture(); 
        }

        if (connected_) 
        {


            // Deinitialize camera
            spinError err = spinCameraDeInit(hCamera_);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to deinitialize Spinnaker camera, error=" << err;
                throw RuntimeError(ERROR_SPIN_RELEASE_CAMERA, ssError.str());
            }

            // Release Camera
            err = spinCameraRelease(hCamera_);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to get Spinnaker camera, error=" << err;
                throw RuntimeError(ERROR_SPIN_RELEASE_CAMERA, ssError.str());
            }

            connected_ = false;
        }
    }


    void CameraDevice_spin::startCapture()
    {
        if (!connected_) 
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to start Spinnaker capture - not connected";
            throw RuntimeError(ERROR_SPIN_START_CAPTURE, ssError.str());
        }

        // DEBUG 
        // ----------------------------
        //printFormat7Configuration();
        // ----------------------------


        if (!capturing_) 
        {

            // Set acquisition mode 
            EnumNode_spin acqModeNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("AcquisitionMode");
            acqModeNode.setEntryBySymbolic("Continuous");

            //setupTimeStamping();

            // Begin acquisition
            spinError err = spinCameraBeginAcquisition(hCamera_);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to begin camera acquisition, error = " << err; 
                throw RuntimeError(ERROR_SPIN_START_CAPTURE, ssError.str());
            }

            capturing_ = true;
            //isFirst_ = true;
        }
    }


    void CameraDevice_spin::stopCapture()
    {
        if (capturing_) 
        {
            if (!releaseSpinImage(hSpinImage_))
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to release spinImage";
                throw RuntimeError(ERROR_SPIN_RELEASE_SPIN_IMAGE, ssError.str());
            }

            spinError err = spinCameraEndAcquisition(hCamera_);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError <<": unable to stop Spinnaker capture";
                throw RuntimeError(ERROR_SPIN_STOP_CAPTURE, ssError.str());
            }
            capturing_ = false;
        }
    }


    cv::Mat CameraDevice_spin::grabImage()
    {
        cv::Mat image;  
        grabImage(image);
        return image;
    }


    void CameraDevice_spin::grabImage(cv::Mat &image)
    {

    //    bool resize = false;

        std::string errMsg;

        bool ok = grabImageCommon(errMsg);
        if (!ok)
        {
            image.release();
            return;
        }


        spinError err = SPINNAKER_ERR_SUCCESS;
        spinImage hSpinImageConv = nullptr; 

        err = spinImageCreateEmpty(&hSpinImageConv);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to create empty spinImage, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_CREATE_EMPTY, ssError.str());
        }

        // TEMP  
        // ----------------------------------------------------------------------------
        // Need to test for compatible image format and preform appropriate converion. 
        //
        // * spinnaker getSuitableImageFormat 
        // * opencv getCompatibleOpencvFormat
        //
        // ----------------------------------------------------------------------------
        
        spinPixelFormatEnums origPixelFormat = getImagePixelFormat_spin(hSpinImage_);
        spinPixelFormatEnums convPixelFormat = getSuitablePixelFormat(origPixelFormat);
        
        err = spinImageConvert(hSpinImage_, convPixelFormat, hSpinImageConv);
        //err = spinImageConvert(hSpinImage_, PixelFormat_BGR8, hSpinImageConv);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert spinImage, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_CONVERT, ssError.str());
        }

        ImageInfo_spin imageInfo = getImageInfo_spin(hSpinImageConv);

        int opencvPixelFormat = getCompatibleOpencvFormat(convPixelFormat);

        cv::Mat imageTmp = cv::Mat( 
                imageInfo.rows+imageInfo.ypad, 
                imageInfo.cols+imageInfo.xpad, 
                opencvPixelFormat, 
                imageInfo.dataPtr, 
                imageInfo.stride
                );

        imageTmp.copyTo(image);

        // ----------------------------------------------------------------------------

        if (!destroySpinImage(hSpinImageConv))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to release spinImage";
            throw RuntimeError(ERROR_SPIN_RELEASE_SPIN_IMAGE, ssError.str());
        }

    }


    bool CameraDevice_spin::isColor()
    {
        std::vector<spinPixelFormatEnums> cameraPixelFormats = getSupportedPixelFormats_spin();
        std::vector<spinPixelFormatEnums> colorFormats = getAllowedColorPixelFormats_spin(); 

        bool test = false;
        for (auto format : cameraPixelFormats)
        {
            test = std::find(colorFormats.begin(), colorFormats.end(), format) != colorFormats.end();
            if (test)
            {
                break;
            }
        }
        return test;
    } 

    //
    //VideoMode CameraDevice_spin::getVideoMode() 
    //{
    //    spinVideoMode vidMode_spin;
    //    spinFrameRate dummy;
    //    getVideoModeAndFrameRate(vidMode_spin, dummy);
    //    return convertVideoMode_from_spin(vidMode_spin);
    //} 


    //FrameRate CameraDevice_spin::getFrameRate() 
    //{
    //    spinVideoMode dummy;
    //    spinFrameRate frmRate_spin;
    //    getVideoModeAndFrameRate(dummy, frmRate_spin);
    //    return convertFrameRate_from_spin(frmRate_spin);
    //}


    //ImageMode CameraDevice_spin::getImageMode()
    //{
    //    ImageMode mode = IMAGEMODE_UNSPECIFIED;
    //    VideoMode vidMode = getVideoMode();
    //    if (vidMode == VIDEOMODE_FORMAT7)
    //    {
    //        spinFormat7Configuration f7config; 
    //        spinMode mode_spin;

    //        f7config = getFormat7Configuration();
    //        mode_spin = f7config.imageSettings.mode;
    //        mode = convertImageMode_from_spin(mode_spin);
    //    }
    //    return mode;
    //} 


    VideoModeList CameraDevice_spin::getAllowedVideoModes()
    {
        VideoModeList allowedVideoModes = {VIDEOMODE_FORMAT7}; 
        return allowedVideoModes;

    }
    //    bool supported;

    //    // Test for non-format7 vidModes
    //    // --------------------------------------------------------------------
    //    VideoModeList allVideoModes = getListOfVideoModes();
    //    VideoModeList::iterator vit;

    //    for (vit=allVideoModes.begin(); vit!=allVideoModes.end(); vit++)
    //    {
    //        VideoMode vidMode = *vit;
    //        if (vidMode == VIDEOMODE_FORMAT7) 
    //        { 
    //            // Skip format7 modes ... as we need a separate test for them.
    //            continue; 
    //        }

    //        FrameRateList allFrameRates = getListOfFrameRates();
    //        FrameRateList::iterator fit;
    //        supported = false;

    //        for (fit=allFrameRates.begin(); fit!=allFrameRates.end(); fit++)
    //        {
    //            FrameRate frmRate = *fit; 
    //            try
    //            {
    //                supported |= isSupported(vidMode, frmRate);
    //            }
    //            catch (RuntimeError &runtimeError)
    //            {
    //                // Device query can sometimes fail for some combinations of 
    //                // vidMode and frmRate - handle failure gracefully by
    //                // continuing to examine vidMode and frmRate combinations.                      
    //                continue;
    //            }
    //        } 
    //        if (supported) 
    //        { 
    //            allowedVideoModes.push_back(vidMode); 
    //        }
    //    }

    //    // Test for format7 vidMode
    //    // --------------------------------------------------------------------
    //    ImageModeList allImageModes = getListOfImageModes();
    //    ImageModeList::iterator it;
    //    supported = false;

    //    for (it=allImageModes.begin(); it!=allImageModes.end(); it++)
    //    {
    //        ImageMode imgMode = *it;
    //        try
    //        {
    //            supported |= isSupported(imgMode);
    //        }
    //        catch (RuntimeError &runtimeError)
    //        {
    //            // Fail gracefully in cases where the query for information
    //            // from the device fails.  
    //            continue;
    //        }
    //        if (supported) 
    //        { 
    //            // Only nee to validate support for on imageMode.
    //            allowedVideoModes.push_back(VIDEOMODE_FORMAT7);
    //            break; 
    //        }
    //    }
    //    return allowedVideoModes;
    //}


    //FrameRateList CameraDevice_spin::getAllowedFrameRates(VideoMode vidMode)
    //{
    //    FrameRateList allowedFramesRates;
    //    bool supported;

    //    if (vidMode == VIDEOMODE_FORMAT7) {
    //        allowedFramesRates.push_back(FRAMERATE_FORMAT7);
    //    } 
    //    else
    //    {
    //        FrameRateList allFrameRates = getListOfFrameRates();
    //        FrameRateList::iterator it;

    //        for (it=allFrameRates.begin(); it!=allFrameRates.end(); it++)  
    //        {
    //            FrameRate frmRate = *it; 
    //            try
    //            {
    //                supported = isSupported(vidMode, frmRate);
    //            }
    //            catch (RuntimeError &runtimeError)
    //            {
    //                // Continue checking even in case where device query fails
    //                continue;
    //            }
    //            if (supported) 
    //            {
    //                allowedFramesRates.push_back(frmRate);
    //            }
    //        }
    //    }
    //    return allowedFramesRates;
    //}


    //ImageModeList CameraDevice_spin::getAllowedImageModes()
    //{
    //    ImageModeList allImageModes = getListOfImageModes();
    //    ImageModeList::iterator it;
    //    bool supported;

    //    for (it=allImageModes.begin(); it!=allImageModes.end(); it++)
    //    {
    //        ImageMode imgMode = *it;
    //        try
    //        {
    //            supported = isSupported(imgMode);
    //        }
    //        catch (RuntimeError &runtimeError)
    //        {
    //            // Continue checking even is case where device query fails.
    //            continue;
    //        }
    //        if (supported)
    //        {
    //            allowedImageModes.push_back(imgMode);
    //        }
    //    }
    //    return allowedImageModes;
    //}

    //
    //Property CameraDevice_spin::getProperty(PropertyType propType)
    //{
    //    spinPropertyType propType_spin;
    //    spinProperty prop_spin;
    //    Property prop;

    //    propType_spin = convertPropertyType_to_spin(propType);
    //    prop_spin = getProperty_spin(propType_spin);

    //    // DEBUG
    //    // --------------------------------------------------------------------
    //    //printProperty_spin(prop_spin);
    //    // --------------------------------------------------------------------

    //    prop = convertProperty_from_spin(prop_spin);
    //    return prop;
    //}


    //PropertyInfo CameraDevice_spin::getPropertyInfo(PropertyType propType)
    //{
    //    spinPropertyType propType_spin;
    //    spinPropertyInfo propInfo_spin;
    //    PropertyInfo propInfo;

    //    propType_spin = convertPropertyType_to_spin(propType);
    //    propInfo_spin = getPropertyInfo_spin(propType_spin);

    //    // DEBUG
    //    // --------------------------------------------------------------------
    //    //printPropertyInfo_spin(propInfo_spin);
    //    // --------------------------------------------------------------------

    //    propInfo =  convertPropertyInfo_from_spin(propInfo_spin);
    //    return propInfo;
    //}

   

    //Format7Settings CameraDevice_spin::getFormat7Settings()
    //{
    //    spinError error_spin;
    //    spinFormat7ImageSettings settings_spin;
    //    unsigned int packetSize;
    //    float percentage; 
    //    Format7Settings settings;

    //    // Get Current format7 image settings
    //    error_spin = spinGetFormat7Configuration(
    //            context_, 
    //            &settings_spin,
    //            &packetSize,
    //            &percentage
    //            );
    //    if (error_spin != SPIN_ERROR_OK)
    //    { 
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to get Spinnaker format 7 configuration"; 
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_CONFIGURATION, ssError.str());
    //    }
    //    settings.mode = convertImageMode_from_spin(settings_spin.mode);
    //    settings.offsetX = settings_spin.offsetX;
    //    settings.offsetY = settings_spin.offsetY;
    //    settings.width = settings_spin.width;
    //    settings.height = settings_spin.height;
    //    settings.pixelFormat = convertPixelFormat_from_spin(settings_spin.pixelFormat);
    //    return settings;
    //}


    //Format7Info CameraDevice_spin::getFormat7Info(ImageMode imgMode)
    //{
    //    spinError error_spin;
    //    spinFormat7Info format7Info_spin;
    //    BOOL supported;
    //    Format7Info format7Info;

    //    format7Info_spin.mode = convertImageMode_to_spin(imgMode);
    //    error_spin = spinGetFormat7Info(context_, &format7Info_spin, &supported);
    //    if (error_spin != SPIN_ERROR_OK) 
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to get Spinnaker format7 information"; 
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_INFO, ssError.str());
    //    }

    //    format7Info.mode = imgMode;
    //    format7Info.supported = supported;
    //    if (supported) 
    //    {
    //        format7Info.maxWidth = format7Info_spin.maxWidth;
    //        format7Info.maxHeight = format7Info_spin.maxHeight;
    //        format7Info.offsetHStepSize = format7Info_spin.offsetHStepSize;
    //        format7Info.offsetVStepSize = format7Info_spin.offsetVStepSize;
    //        format7Info.imageHStepSize = format7Info_spin.imageHStepSize;
    //        format7Info.imageVStepSize = format7Info_spin.imageVStepSize;
    //        format7Info.pixelFormatBitField = format7Info_spin.pixelFormatBitField;
    //        format7Info.vendorPixelFormatBitField = format7Info_spin.vendorPixelFormatBitField;
    //        format7Info.packetSize = format7Info_spin.packetSize;
    //        format7Info.minPacketSize = format7Info_spin.minPacketSize;
    //        format7Info.maxPacketSize = format7Info_spin.maxPacketSize;
    //        format7Info.percentage = format7Info.percentage;
    //    }
    //    return format7Info;
    //}


    //bool CameraDevice_spin::validateFormat7Settings(Format7Settings settings)
    //{
    //    spinFormat7ImageSettings settings_spin = convertFormat7Settings_to_spin(settings);

    //    BOOL settingsAreValid_spin;
    //    spinFormat7PacketInfo packetInfo_spin;

    //    spinError error_spin = spinValidateFormat7Settings(
    //            context_, 
    //            &settings_spin, 
    //            &settingsAreValid_spin,
    //            &packetInfo_spin
    //            );

    //    if (error_spin != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to validate Spinnaker format 7 settings"; 
    //        throw RuntimeError(ERROR_SPIN_VALIDATE_FORMAT7_SETTINGS, ssError.str());
    //    }
    //    return bool(settingsAreValid_spin);
    //}


    //void CameraDevice_spin::setFormat7Configuration(Format7Settings settings, float percentSpeed)
    //{
    //    spinFormat7ImageSettings settings_spin = convertFormat7Settings_to_spin(settings);
    //    spinError error_spin = spinSetFormat7Configuration(context_, &settings_spin, percentSpeed);
    //    if (error_spin != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to set Spinnaker format 7 configuration"; 
    //        throw RuntimeError(ERROR_SPIN_SET_FORMAT7_CONFIGURATION, ssError.str());
    //    }
    //}


    //PixelFormatList CameraDevice_spin::getListOfSupportedPixelFormats(ImageMode imgMode)
    //{
    //    PixelFormatList formatList = getListOfPixelFormats();
    //    PixelFormatList supportedList;

    //    Format7Info info = getFormat7Info(imgMode);
    //    if (info.supported)
    //    {
    //        PixelFormatList::iterator it;
    //        for (it=formatList.begin(); it!=formatList.end(); it++)
    //        {
    //            PixelFormat format = *it;
    //            spinPixelFormat format_spin = convertPixelFormat_to_spin(format);
    //            if (format_spin & info.pixelFormatBitField)
    //            {
    //                supportedList.push_back(format);
    //            }
    //        }
    //    }
    //    return supportedList;
    //}


    //void CameraDevice_spin::setProperty(Property prop)
    //{
    //    spinProperty prop_spin;
    //    prop_spin = convertProperty_to_spin(prop);
    //    setProperty(prop_spin);
    //}
    //

    //bool CameraDevice_spin::isSupported(VideoMode vidMode, FrameRate frmRate)
    //{
    //    spinVideoMode vidMode_spin;
    //    spinFrameRate frmRate_spin;
    //    BOOL supported;
    //    spinError error;

    //    try 
    //    {
    //        vidMode_spin = convertVideoMode_to_spin(vidMode);
    //        frmRate_spin = convertFrameRate_to_spin(frmRate);
    //    }
    //    catch (RuntimeError &runtimeError)
    //    {
    //        // Conversion failed - combination not supported
    //        return false;
    //    }

    //    error = spinGetVideoModeAndFrameRateInfo(
    //            context_, 
    //            vidMode_spin, 
    //            frmRate_spin, 
    //            &supported
    //            ); 

    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker VideoMode and ";
    //        ssError << "FrameRate information";
    //        throw RuntimeError(ERROR_SPIN_GET_VIDEOMODE_FRAMERATE_INFO, ssError.str());
    //        return false;
    //    }

    //    return (supported == TRUE ? true : false);
    //}


    //bool CameraDevice_spin::isSupported(ImageMode imgMode)
    //{
    //    spinError error;
    //    spinMode mode_spin;
    //    spinFormat7Info info_spin;
    //    BOOL supported;

    //    try 
    //    {
    //        mode_spin = convertImageMode_to_spin(imgMode);
    //    }
    //    catch (RuntimeError &runtimeError)
    //    {
    //        // If conversion failed - mode not supported
    //        return false;
    //    }

    //    info_spin.mode = mode_spin;
    //    error = spinGetFormat7Info(context_, &info_spin, &supported);
    //    if (error != SPIN_ERROR_OK)
    //    {   
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get format7 information for given mode";
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_INFO, ssError.str());
    //    }
    //    return (supported==TRUE ? true : false);
    //}

    //unsigned int CameraDevice_spin::getNumberOfImageMode()
    //{
    //    return NUMBER_OF_SPIN_IMAGEMODE;
    //}


    //void CameraDevice_spin::setVideoMode(VideoMode vidMode, FrameRate frmRate) 
    //{
    //    if (vidMode != VIDEOMODE_FORMAT7) 
    //    {
    //        // For all non-format7 video modes
    //        spinVideoMode vidMode_spin = convertVideoMode_to_spin(vidMode);
    //        spinFrameRate frmRate_spin = convertFrameRate_to_spin(frmRate);
    //        spinError error = spinSetVideoModeAndFrameRate(
    //                context_, 
    //                vidMode_spin, 
    //                frmRate_spin
    //                );
    //        if (error != SPIN_ERROR_OK)
    //        {
    //            std::stringstream ssError;
    //            ssError << __PRETTY_FUNCTION__;
    //            ssError << ": unable to set VideoMode and frame rate";
    //            throw RuntimeError(
    //                    ERROR_SPIN_SET_VIDEOMODE_AND_FRAMERATE, 
    //                    ssError.str()
    //                    );
    //        }
    //    }
    //    else 
    //    {
    //        // For format 7 video modes ... 
    //        //
    //        // Temporary. TO DO uses setVideoModeFormat7 which needs to be 
    //        // worked on to make it more flexible.
    //        ImageModeList allowedImageModes = getAllowedImageModes();
    //        if (allowedImageModes.empty())
    //        {
    //            std::stringstream ssError;
    //            ssError << __PRETTY_FUNCTION__;
    //            ssError << ": unable to set Format7 video mode not support imageModes";
    //            throw RuntimeError(ERROR_SPIN_SET_VIDEOMODE_FORMAT7, ssError.str());
    //        }
    //        ImageMode mode = allowedImageModes.front();
    //        setVideoModeToFormat7(mode);
    //        //setVideoModeToFormat7(IMAGEMODE_1);
    //    }
    //}


    //void CameraDevice_spin::setFormat7ImageMode(ImageMode imgMode) 
    //{
    //    // -------------------------------------------------
    //    // TO DO ...
    //    // -------------------------------------------------
    //}


    //void CameraDevice_spin::setTriggerInternal()
    //{
    //    spinTriggerMode trigMode = getTriggerMode_spin();
    //    trigMode.onOff = FALSE;
    //    setTriggerMode(trigMode);
    //}
    //

    //void CameraDevice_spin::setTriggerExternal()
    //{
    //    // ------------------------------------------------
    //    // TO DO ... not really finished yet
    //    // ------------------------------------------------
    //    //
    //    // Currently only sets to mode 0, doesn't check for
    //    // support, etc.
    //    // ------------------------------------------------ 
    //    spinError error = spinSetGPIOPinDirection(context_, 0, 0);
    //    if (error != SPIN_ERROR_OK) 
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to set GPIO direction";
    //        throw RuntimeError(ERROR_SPIN_CREATE_IMAGE, ssError.str());
    //    }
    //    
    //    spinTriggerMode trigMode = getTriggerMode_spin();
    //    trigMode.onOff = TRUE;
    //    trigMode.mode = 0;
    //    trigMode.source = 0;
    //    trigMode.parameter = 0;
    //    trigMode.polarity = 0;
    //    setTriggerMode(trigMode);
    //}

    //TriggerType CameraDevice_spin::getTriggerType()
    //{
    //    spinTriggerMode trigMode = getTriggerMode_spin();
    //    if ((trigMode.onOff == TRUE) && (trigMode.mode != 3)) // Note, mode 3 is frame skip
    //    {
    //        return TRIGGER_EXTERNAL;
    //    }
    //    else
    //    {
    //        return TRIGGER_INTERNAL;
    //    }
    //}

    //TimeStamp CameraDevice_spin::getImageTimeStamp()
    //{
    //    return timeStamp_;
    //}


    std::string CameraDevice_spin::getVendorName()
    {
        return cameraInfo_.vendorName();
    }

    
    std::string CameraDevice_spin::getModelName()
    {
       return cameraInfo_.modelName();
    }


    std::string CameraDevice_spin::toString()
    {
        return cameraInfo_.toString();
    }


    void CameraDevice_spin::printGuid() 
    { 
        guid_.printValue(); 
    }


    void CameraDevice_spin::printInfo()
    {
        std::cout << toString();
    }


    //   
    //// Private methods
    //// -------------------------------------------------------------------------

    //spinPGRGuid CameraDevice_spin::getGuid_spin()
    //{
    //    return guid_.getValue_spin();
    //}



    //void CameraDevice_spin::destroyConvertedImage()
    //{
    //    if (convertedImageCreated_) 
    //    {
    //        spinError error = spinDestroyImage(&convertedImage_);

    //        if (error != SPIN_ERROR_OK) 
    //        {
    //            std::stringstream ssError;
    //            ssError << __PRETTY_FUNCTION__;
    //            ssError << ": unable to destroy Spinnaker image";
    //            throw RuntimeError(ERROR_SPIN_DESTROY_IMAGE, ssError.str());
    //        }
    //        
    //        convertedImageCreated_ = false;
    //    }
    //}


    bool CameraDevice_spin::grabImageCommon(std::string &errMsg)
    {

        spinError err = SPINNAKER_ERR_SUCCESS;

        if (!capturing_) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to grab Image - not capturing";
            errMsg = ssError.str();
            return false;
        }

        if (!releaseSpinImage(hSpinImage_))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to release existing spinImage";
            errMsg = ssError.str();
            return false;
        }

        imageOK_ = false;
    
        // Get next image from camera
		err = spinCameraGetNextImageEx(hCamera_, 0, &hSpinImage_);
		if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get next image";
            errMsg = ssError.str();
            return false;
        }

        // Check to see if image is incomplete
		bool8_t isIncomplete = False;
		err = spinImageIsIncomplete(hSpinImage_, &isIncomplete);
		if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to determine if image is complete";
            errMsg = ssError.str();
            return false;
        }

        if (isIncomplete==True)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": image is incomplete";
            errMsg = ssError.str();
            return false;
        }

        imageOK_ = true;

        return true;
    }


    bool CameraDevice_spin::releaseSpinImage(spinImage &hImage)
    {
        bool rval = true;
        if (hImage != nullptr)
        {
            spinError err = spinImageRelease(hImage);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                rval = false;
            }
            else
            {
                hImage = nullptr;
            }
        }
        return rval;
    }

    bool CameraDevice_spin::destroySpinImage(spinImage &hImage)
    {
        bool rval = true;
        if (hImage != nullptr)
        {
            spinError err = spinImageDestroy(hImage);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                rval = false;
            }
            else
            {
                hImage = nullptr;
            }
        }
        return rval;
    }



    //void CameraDevice_spin::setupTimeStamping()
    //{
    //    spinError error;
    //    spinEmbeddedImageInfo embeddedInfo;

    //    error = spinGetEmbeddedImageInfo(context_, &embeddedInfo); 
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker embedded image info";
    //        throw RuntimeError(ERROR_SPIN_GET_EMBEDDED_IMAGE_INFO, ssError.str());
    //    }

    //    // If embedded time stamping available enable it
    //    if (embeddedInfo.timestamp.available == TRUE)
    //    {
    //        haveEmbeddedTimeStamp_ = true;
    //        embeddedInfo.timestamp.onOff = true;
    //    }
    //    else
    //    {
    //        haveEmbeddedTimeStamp_ = false;
    //        embeddedInfo.timestamp.onOff = false;
    //    }

    //    error = spinSetEmbeddedImageInfo(context_, &embeddedInfo); 
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to set Spinnaker embedded image info";
    //        throw RuntimeError(ERROR_SPIN_SET_EMBEDDED_IMAGE_INFO, ssError.str());
    //    }

    //    // Initalize time stamp data
    //    timeStamp_.seconds = 0;
    //    timeStamp_.microSeconds = 0;
    //    cycleSecondsLast_ = 0;
    //}


    //void CameraDevice_spin::updateTimeStamp()
    //{
    //    spinTimeStamp timeStamp_spin = spinGetImageTimeStamp(&rawImage_);

    //    if (haveEmbeddedTimeStamp_)
    //    {
    //        // DEVEL - get raw time stamp data from image 
    //        // ----------------------------------------------------------------
    //        //unsigned int  stamp;
    //        //unsigned char* pStamp = (unsigned char*) &stamp;
    //        //for (int i=0; i<4; i++)
    //        //{
    //        //    pStamp[i] = rawImage_.pData[3-i];
    //        //}
    //        // ----------------------------------------------------------------

    //        if (isFirst_)
    //        {
    //            timeStamp_.seconds = 0;
    //            timeStamp_.microSeconds = 0;
    //            cycleSecondsLast_ = timeStamp_spin.cycleSeconds;
    //        }
    //        else
    //        {
    //            unsigned int cycleSeconds;
    //            unsigned int cycleCount;
    //            unsigned int cycleOffset;
    //            unsigned int deltaCycleSeconds;

    //            // DEVEL - convert raw time stamp data to cycleSeconds, etc.
    //            // --------------------------------------------------------------------------
    //            //cycleSeconds = (stamp >> 25) & 0b1111111;
    //            //cycleCount   = (stamp >> 12) & 0b1111111111111;  
    //            //cycleOffset  = (stamp >> 0 ) & 0b111111110000;   
    //            //std::cout << cycleSeconds << ", " << cycleCount << ", " << cycleOffset << std::endl;
    //            //std::cout << cycleSeconds << ", " << cycleCount << ", " << cycleOffset << std::endl;
    //            //std::cout << std::endl;
    //            // ---------------------------------------------------------------------------

    //            cycleSeconds = timeStamp_spin.cycleSeconds;
    //            cycleCount = timeStamp_spin.cycleCount;
    //            cycleOffset = timeStamp_spin.cycleOffset;
    //            cycleOffset &= CYCLE_OFFSET_MASK;
    //            if (timeStamp_spin.cycleSeconds >= cycleSecondsLast_)
    //            {
    //                deltaCycleSeconds = cycleSeconds; 
    //                deltaCycleSeconds -= cycleSecondsLast_;
    //            }
    //            else
    //            {
    //                deltaCycleSeconds = MAX_CYCLE_SECONDS; 
    //                deltaCycleSeconds += timeStamp_spin.cycleSeconds; 
    //                deltaCycleSeconds -= cycleSecondsLast_;
    //            }
    //            timeStamp_.seconds += (unsigned long long)(deltaCycleSeconds);
    //            timeStamp_.microSeconds = USEC_PER_CYCLE_COUNT*cycleCount;
    //            timeStamp_.microSeconds += (USEC_PER_CYCLE_COUNT*cycleOffset)/MAX_CYCLE_OFFSET;
    //            cycleSecondsLast_ = timeStamp_spin.cycleSeconds;
    //        }
    //    }
    //    else
    //    {
    //        timeStamp_.seconds = (unsigned long long)(timeStamp_spin.seconds);
    //        timeStamp_.microSeconds = timeStamp_spin.microSeconds;
    //    }
    //}


    // spin get methods
    // ---------------
    
    std::vector<spinPixelFormatEnums> CameraDevice_spin::getSupportedPixelFormats_spin()
    {   
        EnumNode_spin pixelFormatNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("PixelFormat");
        std::vector<EntryNode_spin> pixelFormatEntryVec = pixelFormatNode.entries();

        std::vector<spinPixelFormatEnums> pixelFormatValueVec;
        for (auto entry: pixelFormatEntryVec)
        {
            //std::cout << entry.symbolic() << ", " << entry.value() << std::endl;
            pixelFormatValueVec.push_back(spinPixelFormatEnums(entry.value()));
        }
        return pixelFormatValueVec;
    }


    //void CameraDevice_spin::getVideoModeAndFrameRate(
    //        spinVideoMode &vidMode, 
    //        spinFrameRate &frmRate
    //        )
    //{
    //    spinError error = spinGetVideoModeAndFrameRate(context_, &vidMode, &frmRate);
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker VideoMode and FrameRate";
    //        throw RuntimeError(ERROR_SPIN_GET_VIDEOMODE_AND_FRAMERATE, ssError.str());
    //    }
    //}


    //spinPropertyInfo CameraDevice_spin::getPropertyInfo_spin(spinPropertyType propType)
    //{
    //    spinError error;
    //    spinPropertyInfo propInfo;
    //    propInfo.type = propType;

    //    error = spinGetPropertyInfo(context_, &propInfo);
    //    if (error != SPIN_ERROR_OK)  
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError < ": unable to get Spinnaker properyInfo";
    //        throw RuntimeError(ERROR_SPIN_GET_PROPERTY_INFO, ssError.str());
    //    }
    //    return propInfo;
    //}


    //spinProperty CameraDevice_spin::getProperty_spin(spinPropertyType propType)
    //{
    //    spinError error;
    //    spinProperty prop;
    //    prop.type = propType;

    //    error = spinGetProperty(context_, &prop);
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker property";
    //        throw RuntimeError(ERROR_SPIN_GET_PROPERTY, ssError.str());
    //    }
    //    return prop;
    //}


    //spinFormat7Configuration CameraDevice_spin::getFormat7Configuration()
    //{
    //    spinError error;
    //    spinFormat7Configuration config;
    //    error = spinGetFormat7Configuration(
    //            context_, 
    //            &(config.imageSettings),
    //            &(config.packetSize),
    //            &(config.percentage)
    //            );
    //    if (error != SPIN_ERROR_OK)
    //    { 
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to get Spinnaker format 7 configuration"; 
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_CONFIGURATION, ssError.str());
    //    }
    //    return config;
    //}


    //spinTriggerMode CameraDevice_spin::getTriggerMode_spin()
    //{
    //    spinError error;
    //    spinTriggerMode triggerMode;
    //    error = spinGetTriggerMode(context_, &triggerMode);
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker TriggerMode";
    //        throw RuntimeError(ERROR_SPIN_GET_TRIGGER_MODE, ssError.str());
    //    }
    //    return triggerMode;
    //}


    //spinTriggerModeInfo CameraDevice_spin::getTriggerModeInfo_spin()
    //{
    //    spinError error;
    //    spinTriggerModeInfo triggerModeInfo;
    //    error = spinGetTriggerModeInfo(context_, &triggerModeInfo);
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker TriggerModeInfo";
    //        throw RuntimeError(ERROR_SPIN_GET_TRIGGER_MODE_INFO, ssError.str());
    //    }
    //}


    //// spin set methods
    //// ---------------

    //void CameraDevice_spin::setProperty(spinProperty prop)
    //{
    //    spinError error = spinSetProperty(context_, &prop);
    //    if (error != SPIN_ERROR_OK) 
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to set Spinnaker prop";
    //        throw RuntimeError(ERROR_SPIN_SET_PROPERTY, ssError.str());
    //    }
    //}


    //void CameraDevice_spin::setTriggerMode(spinTriggerMode trigMode)
    //{
    //    spinError error = spinSetTriggerMode(context_, &trigMode);
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to set Spinnaker TriggerMode";
    //        throw RuntimeError(ERROR_SPIN_SET_TRIGGER_MODE, ssError.str());
    //    }
    //}


    //spinConfig CameraDevice_spin::getConfiguration_spin()
    //{
    //    spinConfig config;
    //    if (connected_)
    //    {
    //        spinError error = spinGetConfiguration(context_,&config);
    //        if (error != SPIN_ERROR_OK)
    //        {
    //            std::stringstream ssError;
    //            ssError << __PRETTY_FUNCTION__;
    //            ssError << ": unable to get Spinnaker config - error";
    //            throw RuntimeError(ERROR_SPIN_GET_CONFIG, ssError.str());
    //        }
    //    }
    //    else
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to get Spinnaker config - not connected";
    //        throw RuntimeError(ERROR_SPIN_GET_CONFIG, ssError.str());
    //    }
    //    return config;
    //}


    //void CameraDevice_spin::setConfiguration_spin(spinConfig &config)
    //{
    //    if (connected_)
    //    {
    //        spinError error = spinSetConfiguration(context_, &config);
    //        if (error != SPIN_ERROR_OK)
    //        {
    //            std::stringstream ssError;
    //            ssError << __PRETTY_FUNCTION__;
    //            ssError << ": unable to set Spinnaker config - error";
    //            throw RuntimeError(ERROR_SPIN_GET_CONFIG, ssError.str());
    //        }
    //    }
    //    else
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to set Spinnaker config - not connected";
    //        throw RuntimeError(ERROR_SPIN_SET_CONFIG, ssError.str());
    //    }
    //}



    //// Temporary methods
    //// ------------------------------------------------------------------------

    //void CameraDevice_spin::setVideoModeToFormat7(ImageMode mode)
    //{
    //    spinMode mode_spin;
    //    mode_spin = convertImageMode_to_spin(mode);
    //    setVideoModeToFormat7(mode_spin);
    //}


    //void CameraDevice_spin::setVideoModeToFormat7(spinMode mode)
    //{
    //    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    //    spinError error;
    //    spinFormat7Info format7Info;
    //    spinFormat7ImageSettings imageSettings;
    //    spinFormat7PacketInfo packetInfo;
    //    spinPixelFormat defaultPixelFormat;
    //    unsigned int packetSize;
    //    float percentage; 
    //    BOOL supported;
    //    BOOL settingsAreValid;

    //    // Get the format 7 info
    //    format7Info.mode = mode;
    //    error = spinGetFormat7Info(context_, &format7Info, &supported);
    //    if (error != SPIN_ERROR_OK) 
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to get Spinnaker format7 information"; 
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_INFO, ssError.str());
    //    }
    //    if ( !supported )
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unsupported Spinnaker video mode, "; 
    //        ssError << getModeString_spin(format7Info.mode);
    //        throw RuntimeError(ERROR_SPIN_UNSUPPORTED_VIDEO_MODE, ssError.str());
    //    }

    //    if (0) // Print format7 information for selected mode
    //    {
    //        printFormat7Info_spin(format7Info);
    //        unsigned int test0 = format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RAW8;
    //        unsigned int test1 = format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_MONO8;
    //        std::cout << "SPIN_PIXEL_FORMAT_RAW8  & pixelFormatBitField = "<< std::bitset<32>(test0) << std::endl;
    //        std::cout << "SPIN_PIXEL_FORMAT_MONO8 & pixelFormatBitField = "<< std::bitset<32>(test1) << std::endl;
    //    }

    //    //// Debug
    //    //// ---------------------------------------------------------------
    //    //PixelFormatList pList = getListOfSupportedPixelFormats(convertImageMode_from_spin(mode));
    //    //PixelFormatList::iterator it;
    //    //std::cout << std::endl;
    //    //std::cout << "-----------------" << std::endl;
    //    //std::cout << "Supported formats" << std::endl;
    //    //std::cout << "-----------------" << std::endl;
    //    //for (it=pList.begin(); it!=pList.end(); it++)
    //    //{
    //    //    std::cout << getPixelFormatString(*it) << std::endl;
    //    //}
    //    //std::cout << std::endl;
    //    //// -----------------------------------------------------------------

    //    // Select pixel format currently - this is a bit of a hack 
    //    spinPixelFormat pixelFormat;
    //    bool havePixelFormat = false;

    //    if (isColor())
    //    {
    //        // Camera is color - try to find a suitable color format
    //        //
    //        // TEMPORARY  - this is a bit broken at the moment as it appears
    //        // that even though a pixel format may appear to be supported via
    //        // the bitfield you will be unable to set it to this value. So you
    //        // really need to check that you where able to set the format.
    //        if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RGB8)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_RGB8;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_BGRU)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_BGRU;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RGBU)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_RGBU;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_BGR)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_BGR;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_BGRU16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_BGRU16;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_BGR16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_BGR16;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RGB16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_RGB16;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_S_RGB16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_S_RGB16;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_411YUV8)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_411YUV8;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_422YUV8)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_422YUV8;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_444YUV8)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_444YUV8;
    //            havePixelFormat = true;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_422YUV8_JPEG)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_422YUV8_JPEG;
    //            havePixelFormat = true;
    //        }
    //    }


    //    if (!havePixelFormat)
    //    {
    //        // This is a monochrome camera or couldn't find color pixel format which will work.
    //        if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RAW8)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_RAW8;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_MONO8)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_MONO8;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RAW16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_RAW16;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_MONO16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_MONO16;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_RAW12)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_RAW12;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_MONO12)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_MONO12;
    //        }
    //        else if (format7Info.pixelFormatBitField & SPIN_PIXEL_FORMAT_S_MONO16)
    //        {
    //            pixelFormat = SPIN_PIXEL_FORMAT_S_MONO16;
    //        }
    //        else
    //        {
    //            std::stringstream ssError;
    //            ssError << __PRETTY_FUNCTION__;
    //            ssError << ": no supported pixel formats";
    //            throw RuntimeError(ERROR_SPIN_NO_SUPPORTED_PIXEL_FORMAT, ssError.str());
    //        }
    //    }

    //    // Create desired format7 configuration
    //    imageSettings.mode = format7Info.mode;
    //    imageSettings.offsetX = 0;
    //    imageSettings.offsetY = 0;
    //    imageSettings.width = format7Info.maxWidth;
    //    imageSettings.height = format7Info.maxHeight;
    //    imageSettings.pixelFormat = pixelFormat;

    //    // Check that settings are valid and get packet info
    //    error = spinValidateFormat7Settings(
    //            context_, 
    //            &imageSettings, 
    //            &settingsAreValid,
    //            &packetInfo
    //            );
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to validate Spinnaker format 7 settings"; 
    //        throw RuntimeError(ERROR_SPIN_VALIDATE_FORMAT7_SETTINGS, ssError.str());
    //    }
    //    if (!settingsAreValid)
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": Spinnaker format 7 settings invalid"; 
    //        throw RuntimeError(ERROR_SPIN_INVALID_FORMAT7_SETTINGS, ssError.str());
    //    }

    //    if (0)  // Print packet info
    //    {
    //        std::cout << std::endl << "Sent - to camera" << std::endl;
    //        printFormat7ImageSettings_spin(imageSettings);
    //        printFormat7PacketInfo_spin(packetInfo);
    //    }

    //    // Set format 7 configuration settings
    //    error = spinSetFormat7Configuration(context_, &imageSettings, 100.0);
    //    if (error != SPIN_ERROR_OK)
    //    {
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to sete Spinnaker format 7 configuration"; 
    //        throw RuntimeError(ERROR_SPIN_SET_FORMAT7_CONFIGURATION, ssError.str());
    //    }

    //    // Get Current format7 image settings
    //    error = spinGetFormat7Configuration(
    //            context_, 
    //            &imageSettings,
    //            &packetSize,
    //            &percentage
    //            );
    //    if (error != SPIN_ERROR_OK)
    //    { 
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to get Spinnaker format 7 configuration"; 
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_CONFIGURATION, ssError.str());
    //    }

    //    if (0) // Print current configuration settings
    //    {
    //        std::cout << "Actual - from camera" << std::endl;
    //        printFormat7Configuration_spin(imageSettings,packetSize,percentage);
    //    }

    //    //std::cout << std::endl;
    //}


    //void CameraDevice_spin::printFormat7Configuration()
    //{
    //    spinError error; //    spinFormat7ImageSettings imageSettings;
    //    unsigned int packetSize;
    //    float percentage; 

    //    // Get Current format7 image settings
    //    error = spinGetFormat7Configuration(
    //            context_, 
    //            &imageSettings,
    //            &packetSize,
    //            &percentage
    //            );
    //    if (error != SPIN_ERROR_OK)
    //    { 
    //        std::stringstream ssError; 
    //        ssError << __PRETTY_FUNCTION__; 
    //        ssError << ": unable to get Spinnaker format 7 configuration"; 
    //        throw RuntimeError(ERROR_SPIN_GET_FORMAT7_CONFIGURATION, ssError.str());
    //    }

    //    std::cout << "Format7Configuration" << std::endl;
    //    std::cout << "--------------------" << std::endl;
    //    printFormat7Configuration_spin(imageSettings,packetSize,percentage);
    //}
}
#endif

