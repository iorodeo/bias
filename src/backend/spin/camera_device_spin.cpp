#ifdef WITH_SPIN
#include "camera_device_spin.hpp"
#include "utils_spin.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <bitset>
#include <fstream>

#include "base_node_spin.hpp"
#include "string_node_spin.hpp"
#include "enum_node_spin.hpp"
#include "entry_node_spin.hpp"
#include "number_node_spin.hpp"
#include "float_node_spin.hpp"
#include "integer_node_spin.hpp"
#include "bool_node_spin.hpp"
#include "enum_node_spin.hpp"


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

        //isFirst_ = true;
        //int64_t timeStamp_ns_ =0 ;
        //timeStamp_.seconds = 0;
        //timeStamp_.microSeconds = 0;
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
        return guid_.getCameraLib(); 
    }


    void CameraDevice_spin::connect() 
    {
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

            // Get Camera info
            cameraInfo_ = nodeMapTLDevice_.cameraInfo();
            cameraInfo_.print();


            // Default settings - may want to test for availability before setting.
            // ----------------------------------------------------------------------------------------------

            // Exposure defaults 
            EnumNode_spin exposureModeNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureMode");
            exposureModeNode.setEntryBySymbolic("Timed");

            EnumNode_spin exposureAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureAuto");
            exposureAutoNode.setEntryBySymbolic("Off");

            // Blacklevel defaults
            EnumNode_spin blackLevelSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("BlackLevelSelector");
            blackLevelSelectorNode.setEntryBySymbolic("All");

            // Gain defualts
            EnumNode_spin gainSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainSelector");
            gainSelectorNode.setEntryBySymbolic("All");

            EnumNode_spin gainAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainAuto");
            gainAutoNode.setEntryBySymbolic("Off");

            // Trigger defaults
            EnumNode_spin triggerSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerSelector");
            triggerSelectorNode.setEntryBySymbolic("FrameStart");

            EnumNode_spin triggerModeNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerMode");
            triggerModeNode.setEntryBySymbolic("Off");

            EnumNode_spin triggerSourceNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerSource");
            triggerSourceNode.setEntryBySymbolic("Software");

            EnumNode_spin triggerOverlapNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerOverlap");
            triggerOverlapNode.setEntryBySymbolic("Off");

            // Framerate defaults
            BoolNode_spin frameRateEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("AcquisitionFrameRateEnable");
            frameRateEnableNode.setValue(true);

            // DEVEL
            // ----------------------------------------------------------------------------------------------

            std::cout << "# TLDevice nodes: " << (nodeMapTLDevice_.numberOfNodes()) << std::endl;
            std::cout << "# Camera nodes:   " << (nodeMapCamera_.numberOfNodes()) << std::endl;
            std::cout << std::endl;

            ////std::vector<EnumNode_spin> nodeVec = nodeMapTLDevice_.nodes<EnumNode_spin>();
            ////std::vector<EnumNode_spin> nodeVec = nodeMapCamera_.nodes<EnumNode_spin>();
            ////
            //std::vector<BaseNode_spin> nodeVec = nodeMapCamera_.nodes<BaseNode_spin>();

            //std::ofstream fout;
            //fout.open("camera_map_nodes.txt");

            //for (auto node : nodeVec)
            //{
            //    fout <<  node.name() << ",  " << node.typeAsString() << std::endl;
            //    std::cout << node.name() << ",  " << node.typeAsString() << std::endl;

            ////    std::cout << "name: " << node.name() << ", numberOfEntries: " << node.numberOfEntries() << std::endl;
            ////    std::vector<EntryNode_spin> entryNodeVec = node.entries();
            ////    EntryNode_spin currEntryNode = node.currentEntry();
            ////    std::cout << "  current: " << currEntryNode.name() << std::endl;
            ////    for (auto entry : entryVec)
            ////    {
            ////        std::cout << "  name:    " << entry.name() << ", " << entry.displayName() << std::endl;
            ////    }
            //}
            //
            //fout.close();
            // ----------------------------------------------------------------------------------------------


            // ----------------------------------------------------------------------------------------------
            // TODO: - setup strobe output on GPIO pin?? Is this possible?
            // ----------------------------------------------------------------------------------------------

        }
    }


    void CameraDevice_spin::disconnect()
    {
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

            setupTimeStamping();

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
        
        spinPixelFormatEnums origPixelFormat = getImagePixelFormat_spin(hSpinImage_);
        spinPixelFormatEnums convPixelFormat = getSuitablePixelFormat(origPixelFormat);
        
        err = spinImageConvert(hSpinImage_, convPixelFormat, hSpinImageConv);
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

    
    VideoMode CameraDevice_spin::getVideoMode() 
    {
        VideoModeList allowedVideoModes = getAllowedVideoModes();
        if (allowedVideoModes.size() != 1)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": more than one video mode supported (DEVEL)";
            throw RuntimeError(ERROR_SPIN_VIDEOMODE_SUPPORT, ssError.str());
        }
        VideoMode videoMode = allowedVideoModes.front();
        return videoMode; 
    } 


    FrameRate CameraDevice_spin::getFrameRate() 
    {
        VideoMode videoMode = getVideoMode();
        FrameRateList allowedFrameRates = getAllowedFrameRates(videoMode);
        if (allowedFrameRates.size() != 1)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": more than one framerate supported (DEVEL)";
            throw RuntimeError(ERROR_SPIN_FRAMERATE_SUPPORT, ssError.str());
        }
        FrameRate frameRate = allowedFrameRates.front();
        return frameRate;
    }


    ImageMode CameraDevice_spin::getImageMode()
    { 
        ImageModeList allowedModeList = getAllowedImageModes();
        if (allowedModeList.size() != 1)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": more than one imagemode supported (DEVEL)";
            throw RuntimeError(ERROR_SPIN_FRAMERATE_SUPPORT, ssError.str());
        }
        ImageMode mode = allowedModeList.front();
        return mode;
    }


    VideoModeList CameraDevice_spin::getAllowedVideoModes()
    {
        // Note:
        // --------------------------------------------------------------------
        // Spinnaker SDK doesn't really have the same concept of VideoModes as
        // FlyCapture2 and libdc1394 so we fake it.
        // --------------------------------------------------------------------
        
        VideoModeList allowedVideoModes = {VIDEOMODE_FORMAT7}; 
        return allowedVideoModes;

    }


    FrameRateList CameraDevice_spin::getAllowedFrameRates(VideoMode vidMode)
    { 
        FrameRateList allowedFrameRates = {};
        if (vidMode == VIDEOMODE_FORMAT7)
        {
            allowedFrameRates.push_back(FRAMERATE_FORMAT7);
        }
        return allowedFrameRates;

    }


    ImageModeList CameraDevice_spin::getAllowedImageModes()
    {
        // Note:
        // -------------------------------------------------------------------
        // Spinnaker SDK doesn't really have ImageModes like FlyCapture2 and 
        // libdc1394 so we fake it. Current only support IMAGEMODE_0, but we
        // can add synthetic image modes using binning.
        // -------------------------------------------------------------------
        ImageModeList allImageModes = {IMAGEMODE_0};
        return allImageModes;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfo(PropertyType propType)
    {
        PropertyInfo propInfo;
        propInfo.type = propType;

        if (isSpinSupportedPropertyType(propType))
        {
            if (getPropertyInfoDispatchMap_.count(propType) > 0)
            {
                propInfo = getPropertyInfoDispatchMap_[propType](this);
            } 
        }

        return propInfo;
    }


    Property CameraDevice_spin::getProperty(PropertyType propType)
    {
        Property prop;
        prop.type = propType;

        if (isSpinSupportedPropertyType(propType))
        {
            if (getPropertyDispatchMap_.count(propType) > 0)
            {
                prop = getPropertyDispatchMap_[propType](this);
            }
        }

        return prop;
    }


    void CameraDevice_spin::setProperty(Property prop)
    {
        std::string settableMsg("");
        bool isSettable = isPropertySettable(prop.type, settableMsg);

        if (!isSettable)
        {
            throw RuntimeError(ERROR_SPIN_PROPERTY_NOT_SETTABLE, settableMsg);
        }

        setPropertyDispatchMap_[prop.type](this,prop);
    }

   

    bool CameraDevice_spin::isPropertySettable(PropertyType propType, std::string &msg)
    {
        if (!isSpinSupportedPropertyType(propType))
        {
            msg = std::string("PropertyType is not supported by Spinnaker Backend");
            return false;
        }


        if (setPropertyDispatchMap_.count(propType) <= 0)
        {
            msg = std::string("PropertyType is not is setter dispatch map");
            return false;
        }
        
        PropertyInfo propInfo = getPropertyInfo(propType);
        // -------------------------------------
        // DEBUG
        // -------------------------------------
        // propInfo.present = true;
        // -------------------------------------
        if (!propInfo.present)
        {
            msg = std::string("PropertyType is not present");
            return false;
        }
        return true;
    }

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




    bool CameraDevice_spin::isSupported(VideoMode vidMode, FrameRate frmRate)
    {
        VideoModeList allowedVideoModes = getAllowedVideoModes();
        FrameRateList allowedFrameRates = getAllowedFrameRates(vidMode);
        bool videoModeFound = (std::find(allowedVideoModes.begin(), allowedVideoModes.end(), vidMode) != allowedVideoModes.end());
        bool frameRateFound = (std::find(allowedFrameRates.begin(), allowedFrameRates.end(), frmRate) != allowedFrameRates.end());
        return (videoModeFound && frameRateFound);
    }


    bool CameraDevice_spin::isSupported(ImageMode imgMode)
    {
        ImageModeList allowedModes = getAllowedImageModes();
        bool found = (std::find(allowedModes.begin(), allowedModes.end(), imgMode) != allowedModes.end());
        return found;
    }


    unsigned int CameraDevice_spin::getNumberOfImageMode()
    {
        return getAllowedImageModes().size();
    }


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


    TimeStamp CameraDevice_spin::getImageTimeStamp()
    {
        return timeStamp_;
    }


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

        updateTimeStamp();

        //std::cout << "timeStamp_ns_           = " << timeStamp_ns_ << std::endl;
        //std::cout << "timeStamp_.seconds      = " << timeStamp_.seconds << std::endl;
        //std::cout << "timeStamp_.microSeconds = " << timeStamp_.microSeconds << std::endl;

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



    void CameraDevice_spin::setupTimeStamping()
    {
        // Enable chunk mode 
        BoolNode_spin chunkModeActiveNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("ChunkModeActive");
        chunkModeActiveNode.setValue(true);

        // Get chunk mode selector and  set entry to Timestamp 
        EnumNode_spin chunkSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ChunkSelector");
        chunkSelectorNode.setEntryBySymbolic("Timestamp");

        // Enable timestamping
        BoolNode_spin timeStampEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("ChunkEnable");
        timeStampEnableNode.setValue(true);
    }


    void CameraDevice_spin::updateTimeStamp()
    {
        spinError err = spinImageChunkDataGetIntValue(hSpinImage_, "ChunkTimestamp", &timeStamp_ns_);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to timestamp from image chunk data, error = " << err; 
            throw RuntimeError(ERROR_SPIN_CHUNKDATA_TIMESTAMP, ssError.str());
        }

        int64_t seconds = timeStamp_ns_/INT64_C(1000000000);
        int64_t microSeconds = timeStamp_ns_/INT64_C(1000) - INT64_C(1000000)*seconds; 

        timeStamp_.seconds = (unsigned long long)(seconds);
        timeStamp_.microSeconds = (unsigned int)(microSeconds);
    }

    // Get PropertyInfo methods
    // --------------------------

    PropertyInfo CameraDevice_spin::getPropertyInfoBrightness()
    {
        FloatNode_spin blackLevelNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("BlackLevel");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_BRIGHTNESS; 
        propInfo.present = blackLevelNode.isAvailable();

        if (propInfo.present)
        {
            propInfo.autoCapable = false;
            propInfo.manualCapable = true;
            propInfo.absoluteCapable = true;
            propInfo.onePushCapable = false;
            propInfo.onOffCapable = false;
            propInfo.readOutCapable = false;
            propInfo.minValue = blackLevelNode.minIntValue();
            propInfo.maxValue = blackLevelNode.maxIntValue();
            propInfo.minAbsoluteValue = blackLevelNode.minValue();
            propInfo.maxAbsoluteValue = blackLevelNode.maxValue();
            propInfo.haveUnits = !blackLevelNode.unit().empty();
            propInfo.units = blackLevelNode.unit();
            propInfo.unitsAbbr = blackLevelNode.unit();
        }

        return propInfo;
    }

    PropertyInfo CameraDevice_spin::getPropertyInfoGamma()
    {
        FloatNode_spin gammaNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gamma");
        BoolNode_spin gammaEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("GammaEnable");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_GAMMA;
        propInfo.present = gammaEnableNode.isAvailable();

        if (propInfo.present)
        {
            propInfo.autoCapable = false;
            propInfo.manualCapable = true;
            propInfo.absoluteCapable = true;
            propInfo.onePushCapable = false;
            propInfo.onOffCapable = gammaEnableNode.isAvailable() && gammaEnableNode.isWritable();
            propInfo.readOutCapable = false;
            if (gammaNode.isAvailable())
            {
                propInfo.minValue = gammaNode.minIntValue();
                propInfo.maxValue = gammaNode.maxIntValue();
                propInfo.minAbsoluteValue = gammaNode.minValue();
                propInfo.maxAbsoluteValue = gammaNode.maxValue();
                propInfo.haveUnits = !gammaNode.unit().empty();
                propInfo.units =  gammaNode.unit();
                propInfo.unitsAbbr = gammaNode.unit();
            }
            
        }

        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoShutter()
    {
        EnumNode_spin exposureAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureAuto");
        FloatNode_spin exposureTimeNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("ExposureTime");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_SHUTTER;
        propInfo.present = exposureAutoNode.isAvailable();

        if (propInfo.present)
        {
            if (exposureAutoNode.isReadable())
            {
                propInfo.autoCapable = exposureAutoNode.hasEntrySymbolic("Continuous");
                propInfo.manualCapable = exposureAutoNode.hasEntrySymbolic("Off");
                propInfo.onePushCapable = exposureAutoNode.hasEntrySymbolic("Once");
            }
            propInfo.absoluteCapable = true;
            propInfo.onOffCapable = false;
            propInfo.readOutCapable = false;
            if (exposureTimeNode.isAvailable() && exposureTimeNode.isReadable())
            {
                propInfo.minValue = exposureTimeNode.minIntValue();
                propInfo.maxValue = exposureTimeNode.maxIntValue();
                propInfo.minAbsoluteValue = std::max(exposureTimeNode.minValue(), MinAllowedShutterUs);
                propInfo.maxAbsoluteValue = std::min(exposureTimeNode.maxValue(), MaxAllowedShutterUs);
                propInfo.haveUnits = !exposureTimeNode.unit().empty();
                propInfo.units =  exposureTimeNode.unit();
                propInfo.unitsAbbr = exposureTimeNode.unit();
            }
        }

        std::cout << __PRETTY_FUNCTION__ << std::endl;
        propInfo.print();

        return propInfo;
    }



    PropertyInfo CameraDevice_spin::getPropertyInfoGain()
    {
        EnumNode_spin gainAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainAuto");
        FloatNode_spin gainNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gain");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_GAIN;
        propInfo.present = gainNode.isAvailable();

        if (propInfo.present)
        {
            if (gainAutoNode.isAvailable() && gainAutoNode.isReadable())
            {
                propInfo.autoCapable = gainAutoNode.hasEntrySymbolic("Continuous");
                propInfo.manualCapable = gainAutoNode.hasEntrySymbolic("Off");
                propInfo.onePushCapable = gainAutoNode.hasEntrySymbolic("Once");
            }
            propInfo.absoluteCapable = true;
            propInfo.onOffCapable = false;
            propInfo.readOutCapable = false;
            propInfo.minValue = gainNode.minIntValue();
            propInfo.maxValue = gainNode.maxIntValue();
            propInfo.minAbsoluteValue = gainNode.minValue();
            propInfo.maxAbsoluteValue = gainNode.maxValue();
            propInfo.haveUnits = !gainNode.unit().empty();
            propInfo.units =  gainNode.unit();
            propInfo.unitsAbbr = gainNode.unit();

        }

        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoTriggerDelay()
    {
        FloatNode_spin triggerDelayNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("TriggerDelay");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_TRIGGER_DELAY;
        propInfo.present = triggerDelayNode.isAvailable();

        if (propInfo.present)
        {
            propInfo.autoCapable = false;
            propInfo.manualCapable = true;
            propInfo.absoluteCapable = true;
            propInfo.onePushCapable = false;
            propInfo.onOffCapable = false;
            propInfo.readOutCapable = false;
            propInfo.minValue = triggerDelayNode.minIntValue();
            propInfo.maxValue = triggerDelayNode.maxIntValue();
            propInfo.minAbsoluteValue = triggerDelayNode.minValue();
            propInfo.maxAbsoluteValue = triggerDelayNode.maxValue();
            propInfo.haveUnits = !triggerDelayNode.unit().empty();
            propInfo.units =  triggerDelayNode.unit();
            propInfo.unitsAbbr = triggerDelayNode.unit();
        }

        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoFrameRate()
    {
        FloatNode_spin frameRateNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("AcquisitionFrameRate");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_FRAME_RATE;
        propInfo.present = frameRateNode.isAvailable();

        if (propInfo.present)
        {
            propInfo.autoCapable = false;
            propInfo.manualCapable = true;
            propInfo.absoluteCapable = true;
            propInfo.onePushCapable = false;
            propInfo.onOffCapable = false;
            propInfo.readOutCapable = false;
            propInfo.minValue = frameRateNode.minIntValue();
            propInfo.maxValue = frameRateNode.maxIntValue();
            propInfo.minAbsoluteValue = frameRateNode.minValue();
            propInfo.maxAbsoluteValue = frameRateNode.maxValue();
            propInfo.haveUnits = !frameRateNode.unit().empty();
            propInfo.units =  frameRateNode.unit();
            propInfo.unitsAbbr = frameRateNode.unit();
        }

        return propInfo;
    }



    PropertyInfo CameraDevice_spin::getPropertyInfoTemperature()
    {
        FloatNode_spin tempNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("DeviceTemperature");

        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_TEMPERATURE;
        propInfo.present = tempNode.isAvailable();

        if (propInfo.present)
        {
            propInfo.autoCapable = false;
            propInfo.manualCapable = false;
            propInfo.absoluteCapable = true;
            propInfo.onePushCapable = false;
            propInfo.onOffCapable = false;
            propInfo.readOutCapable = false;
            propInfo.minValue = tempNode.minIntValue();
            propInfo.maxValue = tempNode.maxIntValue();
            propInfo.minAbsoluteValue = tempNode.minValue();
            propInfo.maxAbsoluteValue = tempNode.maxValue();
            propInfo.haveUnits = !tempNode.unit().empty();
            propInfo.units =  tempNode.unit();
            propInfo.unitsAbbr = tempNode.unit();
        }

        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoTriggerMode()
    {
        // Not implemented - dummy method
        PropertyInfo propInfo;
        propInfo.type = PROPERTY_TYPE_TRIGGER_MODE;
        propInfo.present = true;
        return propInfo;
    }


    // Get Property methods
    // ------------------------
    
    Property CameraDevice_spin::getPropertyBrightness()
    {
        FloatNode_spin blackLevelNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("BlackLevel");

        Property prop;
        prop.type = PROPERTY_TYPE_BRIGHTNESS;
        prop.present = blackLevelNode.isAvailable();
        
        if(prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.on = true;
            prop.autoActive = false;
            prop.value = blackLevelNode.intValue();
            prop.valueA = 0;
            prop.valueB = 0;
            prop.absoluteValue = blackLevelNode.value();
        }

        return prop;
    }


    Property CameraDevice_spin::getPropertyGamma()
    {
        FloatNode_spin gammaNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gamma");
        BoolNode_spin gammaEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("GammaEnable");

        Property prop;
        prop.type = PROPERTY_TYPE_GAMMA;
        prop.present = gammaEnableNode.isAvailable();

        if (prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.autoActive = false;
            prop.on = gammaEnableNode.value();
            if (gammaNode.isAvailable())
            {
                prop.value = gammaNode.intValue();
                prop.absoluteValue = gammaNode.value();
            }
        }

        return prop;
    }


    Property CameraDevice_spin::getPropertyShutter()
    {
        EnumNode_spin exposureAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureAuto");
        FloatNode_spin exposureTimeNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("ExposureTime");

        Property prop;
        prop.type = PROPERTY_TYPE_SHUTTER;
        prop.present = exposureAutoNode.isAvailable();

        if (prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.autoActive = false;
            if (exposureAutoNode.isReadable())
            {
                EntryNode_spin autoEntry = exposureAutoNode.currentEntry();
                prop.autoActive = autoEntry.isSymbolicValueEqualTo("Continuous");
            }
            if (exposureTimeNode.isAvailable() && exposureTimeNode.isReadable())
            {
                prop.value = exposureTimeNode.intValueWithLimits(MinAllowedShutterUs, MaxAllowedShutterUs);
                prop.absoluteValue = exposureTimeNode.value();
            }
        }

        std::cout << __PRETTY_FUNCTION__ << std::endl;
        prop.print();

        return prop;
    }


    Property CameraDevice_spin::getPropertyGain()
    {
        EnumNode_spin gainAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainAuto");
        FloatNode_spin gainNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gain");

        Property prop;
        prop.type = PROPERTY_TYPE_GAIN;
        prop.present = gainNode.isAvailable();

        if (prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.autoActive = false;
            if (gainAutoNode.isReadable())
            {
                EntryNode_spin autoEntry = gainAutoNode.currentEntry();
                prop.autoActive = autoEntry.isSymbolicValueEqualTo("Continuous");
            }
            prop.value = gainNode.intValue();
            prop.valueA = 0;
            prop.valueB = 0;
            prop.absoluteValue = gainNode.value();

        }

        return prop;
    }


    Property CameraDevice_spin::getPropertyTriggerDelay()
    {
        FloatNode_spin triggerDelayNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("TriggerDelay");

        Property prop;
        prop.type = PROPERTY_TYPE_TRIGGER_DELAY;
        prop.present = triggerDelayNode.isAvailable();

        if (prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.autoActive = false;
            prop.value = triggerDelayNode.intValue();
            prop.valueA = 0;
            prop.valueB = 0;
            prop.absoluteValue = triggerDelayNode.value();
        }

        return prop;
    }



    Property CameraDevice_spin::getPropertyFrameRate()
    {
        FloatNode_spin frameRateNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("AcquisitionFrameRate");

        Property prop;
        prop.type = PROPERTY_TYPE_FRAME_RATE;
        prop.present = frameRateNode.isAvailable();

        if (prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.autoActive = false;
            prop.value = frameRateNode.intValue();
            prop.valueA = 0;
            prop.valueB = 0;
            prop.absoluteValue = frameRateNode.value();
        }

        return prop;
    }




    Property CameraDevice_spin::getPropertyTemperature()
    {
        FloatNode_spin tempNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("DeviceTemperature");

        Property prop;
        prop.type = PROPERTY_TYPE_TEMPERATURE;
        prop.present = tempNode.isAvailable();

        if (prop.present)
        {
            prop.absoluteControl = true;
            prop.onePush = false;
            prop.autoActive = false;
            prop.value = tempNode.intValue();
            prop.valueA = 0;
            prop.valueB = 0;
            prop.absoluteValue = tempNode.value();
        }

        return prop;
    }


    Property CameraDevice_spin::getPropertyTriggerMode()
    {
        // Not implemented - dummy method
        Property prop;
        prop.type = PROPERTY_TYPE_TRIGGER_MODE;
        prop.present = true;
        return prop;
    }


    // Set Property methods 
    // ---------------------

    void CameraDevice_spin::setPropertyBrightness(Property prop)
    {
        FloatNode_spin blackLevelNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("BlackLevel");
        if (blackLevelNode.isWritable())
        {
            if (prop.absoluteControl)
            {
                blackLevelNode.setValue(prop.absoluteValue);
            }
            else
            {
                blackLevelNode.setValueFromInt(prop.value);
            }
        }
    }


    void CameraDevice_spin::setPropertyGamma(Property prop)
    {
        FloatNode_spin gammaNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gamma");
        BoolNode_spin gammaEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("GammaEnable");


        if (gammaEnableNode.isAvailable() && gammaEnableNode.isWritable()) 
        {
                gammaEnableNode.setValue(prop.on);
        }
        if (gammaNode.isAvailable() && gammaNode.isReadable() && gammaNode.isWritable()) 
        {
            if (prop.absoluteControl)
            {
                gammaNode.setValue(prop.absoluteValue);
            }
            else
            {
                gammaNode.setValueFromInt(prop.value);
            }
        }
    }

    void CameraDevice_spin::setPropertyShutter(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        prop.print();

        EnumNode_spin exposureAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureAuto");
        FloatNode_spin exposureTimeNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("ExposureTime");

        if (exposureAutoNode.isAvailable() && exposureAutoNode.isWritable()) 
        {
            if (prop.onePush)
            {
                // Seems to need to be called a couple of time to stabilize to final value.
                // Not sure why this is.
                for (int i=0; i<AutoOnePushSetCount; i++)
                {
                    exposureAutoNode.setEntryBySymbolic("Once");
                }
                return;
            }

            if (prop.autoActive)
            {
                exposureAutoNode.setEntryBySymbolic("Continuous");
                return;
            }
            else
            {
                exposureAutoNode.setEntryBySymbolic("Off");
            }
        }

        if (exposureTimeNode.isAvailable() && exposureTimeNode.isReadable() && exposureTimeNode.isWritable()) 
        {
            if (prop.absoluteControl)
            {
                exposureTimeNode.setValue(prop.absoluteValue);
            }
            else
            {
                exposureTimeNode.setValueFromIntWithLimits(prop.value, MinAllowedShutterUs, MaxAllowedShutterUs);
            }
        }
    }


    void CameraDevice_spin::setPropertyGain(Property prop)
    {
        EnumNode_spin gainAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainAuto");
        FloatNode_spin gainNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gain");

        if (gainAutoNode.isWritable())
        {
            if (prop.onePush)
            {
                gainAutoNode.setEntryBySymbolic("Once");
                return;
            }

            if (prop.autoActive)
            {
                gainAutoNode.setEntryBySymbolic("Continuous");
                return;
            }
        }

        if (gainNode.isWritable())
        {
            if (prop.absoluteControl)
            {
                gainNode.setValue(prop.absoluteValue);
            }
            else
            {
                gainNode.setValue(prop.value);
            }
        }
    }



    void CameraDevice_spin::setPropertyTriggerDelay(Property prop)
    {
        FloatNode_spin triggerDelayNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("TriggerDelay");

        if (triggerDelayNode.isWritable())
        {
            if (prop.absoluteControl)
            {
                triggerDelayNode.setValue(prop.absoluteValue);
            }
            else
            {
                triggerDelayNode.setValueFromInt(prop.value);
            }
        }
    }

    
    void CameraDevice_spin::setPropertyFrameRate(Property prop)
    {
        FloatNode_spin frameRateNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("AcquisitionFrameRate");

        if (frameRateNode.isWritable())
        {
            if (prop.absoluteControl)
            {
                frameRateNode.setValue(prop.absoluteValue);
            }
            else
            {
                frameRateNode.setValueFromInt(prop.value);
            }
        }
    }


    void CameraDevice_spin::setPropertyTemperature(Property prop)
    { 
        // Do nothing
    }


    void CameraDevice_spin::setPropertyTriggerMode(Property prop)
    {
        // Do nothing
    }


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


    std::map<PropertyType, std::function<PropertyInfo(CameraDevice_spin*)>> CameraDevice_spin::getPropertyInfoDispatchMap_ = 
    { 
        {PROPERTY_TYPE_BRIGHTNESS,     std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoBrightness)},
        {PROPERTY_TYPE_GAMMA,          std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoGamma)},
        {PROPERTY_TYPE_SHUTTER,        std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoShutter)},
        {PROPERTY_TYPE_GAIN,           std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoGain)},
        {PROPERTY_TYPE_TRIGGER_DELAY,  std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoTriggerDelay)},
        {PROPERTY_TYPE_FRAME_RATE,     std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoFrameRate)},
        {PROPERTY_TYPE_TEMPERATURE,    std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoTemperature)},
        {PROPERTY_TYPE_TRIGGER_MODE,   std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoTriggerMode)},
    };


    std::map<PropertyType, std::function<Property(CameraDevice_spin*)>> CameraDevice_spin::getPropertyDispatchMap_ = 
    { 
        {PROPERTY_TYPE_BRIGHTNESS,     std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyBrightness)},
        {PROPERTY_TYPE_GAMMA,          std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyGamma)},
        {PROPERTY_TYPE_SHUTTER,        std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyShutter)},
        {PROPERTY_TYPE_GAIN,           std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyGain)},
        {PROPERTY_TYPE_TRIGGER_DELAY,  std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyTriggerDelay)},
        {PROPERTY_TYPE_FRAME_RATE,     std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyFrameRate)},
        {PROPERTY_TYPE_TEMPERATURE,    std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyTemperature)},
        {PROPERTY_TYPE_TRIGGER_MODE,   std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyTriggerMode)},
    };


    std::map<PropertyType, std::function<void(CameraDevice_spin*,Property)>> CameraDevice_spin::setPropertyDispatchMap_ = 
    { 
        {PROPERTY_TYPE_BRIGHTNESS,     std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyBrightness)},
        {PROPERTY_TYPE_GAMMA,          std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyGamma)},
        {PROPERTY_TYPE_SHUTTER,        std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyShutter)},
        {PROPERTY_TYPE_GAIN,           std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyGain)},
        {PROPERTY_TYPE_TRIGGER_DELAY,  std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyTriggerDelay)},
        {PROPERTY_TYPE_FRAME_RATE,     std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyFrameRate)},
        {PROPERTY_TYPE_TEMPERATURE,    std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyTemperature)},
        {PROPERTY_TYPE_TRIGGER_MODE,   std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyTriggerMode)},
    };

    // DEVEL
    // ----------------------------------------------------------------------------------------------------------------------------------
    void CameraDevice_spin::develExpProps()
    {
        if (!connected_)
        {
            std::cout << "camera is not connected - can't explore properties" << std::endl;
        }


        // Exposure (Used instead of Shutter ... what about gain, intensity, etc. ???
        // --------------------------------------------------------------------------------------------------

        EnumNode_spin exposureModeNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureMode");
        exposureModeNode.setEntryBySymbolic("Timed");
        exposureModeNode.print();

        EnumNode_spin exposureAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureAuto");
        exposureAutoNode.setEntryBySymbolic("Off");
        exposureAutoNode.print();

        FloatNode_spin exposureTimeNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("ExposureTime");
        exposureTimeNode.print();

        // Gain .
        // --------------------------------------------------------------------------------------------------

        EnumNode_spin gainSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainSelector");
        gainSelectorNode.print();

        EnumNode_spin gainAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("GainAuto");
        gainAutoNode.print();

        FloatNode_spin gainNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gain");
        gainNode.print();

        // Trigger 
        // --------------------------------------------------------------------------------------------------

        EnumNode_spin triggerSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerSelector");
        triggerSelectorNode.print();

        EnumNode_spin triggerModeNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerMode");
        triggerModeNode.print();

        EnumNode_spin triggerSourceNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerSource");
        triggerSourceNode.print();

        EnumNode_spin triggerOverlapNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("TriggerOverlap");
        triggerOverlapNode.print();

        FloatNode_spin triggerDelayNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("TriggerDelay");
        triggerDelayNode.print();

        // FrameRate
        // --------------------------------------------------------------------------------------------------

        BoolNode_spin frameRateEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("AcquisitionFrameRateEnable");
        frameRateEnableNode.setValue(true);
        frameRateEnableNode.print();

        FloatNode_spin frameRateNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("AcquisitionFrameRate");
        frameRateNode.print();

        FloatNode_spin resFrameRateNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("AcquisitionResultingFrameRate");
        resFrameRateNode.print();


        // Temperature
        //---------------------------------------------------------------------------------------------------
        FloatNode_spin tempNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("DeviceTemperature");
        tempNode.print();

        // Blacklevel
        // ---------------------------------------------------------------------------------------------------
        EnumNode_spin blackLevelSelectorNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("BlackLevelSelector");
        blackLevelSelectorNode.print();

        FloatNode_spin blackLevelNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("BlackLevel");
        blackLevelNode.print();

        // Gamma
        // ----------------------------------------------------------------------------------------------------
        FloatNode_spin gammaNode = nodeMapCamera_.getNodeByName<FloatNode_spin>("Gamma");
        gammaNode.print();

        BoolNode_spin gammaEnableNode = nodeMapCamera_.getNodeByName<BoolNode_spin>("GammaEnable");
        gammaEnableNode.print();


    }
}
#endif

