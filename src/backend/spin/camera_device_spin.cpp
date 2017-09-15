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


    std::map<PropertyType, std::function<PropertyInfo(CameraDevice_spin*)>> CameraDevice_spin::getPropertyInfoDispatchMap_ = 
    { 
        {PROPERTY_TYPE_BRIGHTNESS,     std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoBrightness)},
        {PROPERTY_TYPE_GAMMA,          std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoGamma)},
        {PROPERTY_TYPE_SHUTTER,        std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoShutter)},
        {PROPERTY_TYPE_GAIN,           std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoGain)},
        {PROPERTY_TYPE_TRIGGER_MODE,   std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoTriggerMode)},
        {PROPERTY_TYPE_TRIGGER_DELAY,  std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoTriggerDelay)},
        {PROPERTY_TYPE_FRAME_RATE,     std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoFrameRate)},
        {PROPERTY_TYPE_TEMPERATURE,    std::function<PropertyInfo(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyInfoTemperature)},
    };


    std::map<PropertyType, std::function<Property(CameraDevice_spin*)>> CameraDevice_spin::getPropertyDispatchMap_ = 
    { 
        {PROPERTY_TYPE_BRIGHTNESS,     std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyBrightness)},
        {PROPERTY_TYPE_GAMMA,          std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyGamma)},
        {PROPERTY_TYPE_SHUTTER,        std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyShutter)},
        {PROPERTY_TYPE_GAIN,           std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyGain)},
        {PROPERTY_TYPE_TRIGGER_MODE,   std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyTriggerMode)},
        {PROPERTY_TYPE_TRIGGER_DELAY,  std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyTriggerDelay)},
        {PROPERTY_TYPE_FRAME_RATE,     std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyFrameRate)},
        {PROPERTY_TYPE_TEMPERATURE,    std::function<Property(CameraDevice_spin*)>(&CameraDevice_spin::getPropertyTemperature)},
    };


    std::map<PropertyType, std::function<void(CameraDevice_spin*,Property)>> CameraDevice_spin::setPropertyDispatchMap_ = 
    { 
        {PROPERTY_TYPE_BRIGHTNESS,     std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyBrightness)},
        {PROPERTY_TYPE_GAMMA,          std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyGamma)},
        {PROPERTY_TYPE_SHUTTER,        std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyShutter)},
        {PROPERTY_TYPE_GAIN,           std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyGain)},
        {PROPERTY_TYPE_TRIGGER_MODE,   std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyTriggerMode)},
        {PROPERTY_TYPE_TRIGGER_DELAY,  std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyTriggerDelay)},
        {PROPERTY_TYPE_FRAME_RATE,     std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyFrameRate)},
        {PROPERTY_TYPE_TEMPERATURE,    std::function<void(CameraDevice_spin*, Property)>(&CameraDevice_spin::setPropertyTemperature)},
    };


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

        // Gain - don't understand this yet.
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

            // Setup exposure defaults 
            EnumNode_spin exposureModeNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureMode");
            exposureModeNode.setEntryBySymbolic("Timed");

            EnumNode_spin exposureAutoNode = nodeMapCamera_.getNodeByName<EnumNode_spin>("ExposureAuto");
            exposureAutoNode.setEntryBySymbolic("Off");


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

        updateTimeStamp();
        std::cout << "timeStamp_ns_           = " << timeStamp_ns_ << std::endl;
        std::cout << "timeStamp_.seconds      = " << timeStamp_.seconds << std::endl;
        std::cout << "timeStamp_.microSeconds = " << timeStamp_.microSeconds << std::endl;

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
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoGamma()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoShutter()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoGain()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoTriggerMode()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoTriggerDelay()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoFrameRate()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    PropertyInfo CameraDevice_spin::getPropertyInfoTemperature()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        PropertyInfo propInfo;
        return propInfo;
    }


    // Set Property methods 
    // ---------------------

    Property CameraDevice_spin::getPropertyBrightness()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyGamma()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyShutter()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyGain()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyTriggerMode()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyTriggerDelay()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyFrameRate()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }


    Property CameraDevice_spin::getPropertyTemperature()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Property prop;
        return prop;
    }

    // Set Property methods 
    // ---------------------

    void CameraDevice_spin::setPropertyBrightness(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyGamma(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyShutter(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyGain(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyTriggerMode(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyTriggerDelay(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyFrameRate(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
    }


    void CameraDevice_spin::setPropertyTemperature(Property prop)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return;
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

