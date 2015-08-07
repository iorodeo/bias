#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#include "utils_dc1394.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include <sstream>
#include <algorithm>
#ifdef WIN32
#include<Windows.h>
#endif

namespace bias {

    const unsigned int NUMBER_OF_DC1394_IMAGEMODE = DC1394_VIDEO_MODE_FORMAT7_NUM; 

    CameraDevice_dc1394::CameraDevice_dc1394() : CameraDevice()
    {
        context_dc1394_ = NULL;
        camera_dc1394_ = NULL;
        numDMABuffer_ = DEFAULT_NUM_DMA_BUFFER; 

        timeStamp_ = {0,0};
        startTime_ = 0;
        timerFreq_ = 1;
        isFirst_ = true;
    }


    CameraDevice_dc1394::CameraDevice_dc1394(Guid guid) : CameraDevice(guid)
    {
        context_dc1394_ = NULL;
        camera_dc1394_ = NULL;
        numDMABuffer_ = DEFAULT_NUM_DMA_BUFFER; 

        context_dc1394_ = dc1394_new();
        if (!context_dc1394_) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error creating dc1394_t" << std::endl;
            throw RuntimeError(ERROR_DC1394_CREATE_CONTEXT, ssError.str());
        }
    }


    CameraDevice_dc1394::~CameraDevice_dc1394()
    {
        if (capturing_) 
        { 
            stopCapture(); 
        } 
        if (connected_) 
        { 
            disconnect(); 
        }
        dc1394_free(context_dc1394_);
    }


    CameraLib CameraDevice_dc1394::getCameraLib()
    {
        return guid_.getCameraLib();
    }


    void CameraDevice_dc1394::connect() 
    {
        if (!connected_) 
        {
            // Create new dc1394 camera object
            camera_dc1394_ = dc1394_camera_new(context_dc1394_, guid_.getValue_dc1394());

            if ( ! camera_dc1394_ ) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to create dc1394 camera";
                throw RuntimeError(ERROR_DC1394_NEW_CAMERA, ssError.str());
            }
            connected_ = true;


            // Temporary - just pick a video mode which works.
            dc1394error_t error = dc1394_video_set_mode(camera_dc1394_, DC1394_VIDEO_MODE_FORMAT7_0);
            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set dc1394 video mode, error code ";
                ssError << error  << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_MODE, ssError.str());
            }


            setTriggerInternal();

            //getFormat7Info(IMAGEMODE_0);

            // DEVEL 
            // -------------------------------------------------------------------
            //Format7Settings f7Settings = getFormat7Settings();
            //f7Settings.print();
            //setFormat7Configuration(f7Settings,100.0);

            //PixelFormatList pixelFormatList = getListOfSupportedPixelFormats(IMAGEMODE_0);
            //PixelFormatList::iterator it;
            //for (it=pixelFormatList.begin(); it!=pixelFormatList.end(); it++)
            //{
            //    PixelFormat format = *it;
            //    std::string formatStr = getPixelFormatString(format); 
            //    std::cout <<  formatStr << std::endl;
            //}

            // --------------------------------------------------------------------
            
        }
    }


    void CameraDevice_dc1394::disconnect()
    {
        if (capturing_) { stopCapture(); }
        if (connected_) 
        {
            dc1394_camera_free(camera_dc1394_);
            connected_ = false;
        }
    }


    void CameraDevice_dc1394::startCapture()
    {
        dc1394error_t error;
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to start dc1394 capture - not connected";
            throw RuntimeError(ERROR_DC1394_START_CAPTURE, ssError.str());
        }

        if (!capturing_) {

            // ------------------------------------------------------------------
            // WBD DEVEL
            //
            // Need to modify so that we pick a resonable initial video mode
            // ------------------------------------------------------------------

            // Temporary - just pick a video mode which works.
            error = dc1394_video_set_mode(camera_dc1394_, DC1394_VIDEO_MODE_FORMAT7_0);
            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set dc1394 video mode, error code ";
                ssError << error  << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_MODE, ssError.str());
            }
        
            // Temporary - set color coding to mono8
            error = dc1394_format7_set_color_coding(
                    camera_dc1394_,
                    DC1394_VIDEO_MODE_FORMAT7_0,
                    DC1394_COLOR_CODING_MONO8
                    );
            if (error != DC1394_SUCCESS)
            { 
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set dc1394 color_coding, error code ";
                ssError << error  << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_MODE, ssError.str());
            }

            // Set number of DMA buffers and capture flags
            error = dc1394_capture_setup(
                    camera_dc1394_,
                    numDMABuffer_, 
                    DC1394_CAPTURE_FLAGS_DEFAULT
                    );
            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to setup dc1394 capture, error code ";
                ssError << error << std::endl;
                throw RuntimeError(ERROR_DC1394_CAPTURE_SETUP, ssError.str());
            }

            // Start video transmission
            error = dc1394_video_set_transmission(camera_dc1394_, DC1394_ON);
            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to turn on dc1394 video transmission, error code ";
                ssError << error << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_TRANSMISSION, ssError.str());
            }
            isFirst_ = true;
            capturing_ = true;
        }
    }

    
    void CameraDevice_dc1394::stopCapture()
    {
        if ( capturing_ ) {
            dc1394_video_set_transmission(camera_dc1394_, DC1394_OFF);
            dc1394_capture_stop(camera_dc1394_);
            capturing_ = false;
        }
    }


    void CameraDevice_dc1394::grabImage(cv::Mat &image)
    {
        if (!capturing_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to grab dc1394 image - not capturing";
            throw RuntimeError(ERROR_DC1394_GRAB_IMAGE, ssError.str());
        }

        dc1394error_t error = dc1394_capture_dequeue(
                camera_dc1394_, 
                DC1394_CAPTURE_POLICY_POLL, 
                &frame_dc1394_
                );

        if (error != DC1394_SUCCESS) 
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to dequeue dc1394 frame, error code ";
            ssError << error << std::endl;
            throw RuntimeError(ERROR_DC1394_CAPTURE_DEQUEUE, ssError.str());
        }
        if (frame_dc1394_ != NULL)
        {
            // update time stamp
            updateTimeStamp();
            isFirst_ = false;

            // Copy to cv image  (Temporary) - assume mono8 format
            // Need to modify to handle color images.
            // --------------------------------------------------------------------------
            if ((image.rows != frame_dc1394_ -> size[1]) || (image.cols != frame_dc1394_ -> size[0]))
            {
                image = cv::Mat(frame_dc1394_-> size[1], frame_dc1394_-> size[0], CV_8UC1); 
            }

            unsigned int frameSize = (frame_dc1394_ -> size[0])*(frame_dc1394_ -> size[1]);
            unsigned char *pData0 = frame_dc1394_ -> image;
            unsigned char *pData1 = pData0 +  frameSize - 1;
            std::copy(pData0, pData1, image.data);


            // Put frame back 
            error = dc1394_capture_enqueue(camera_dc1394_, frame_dc1394_);

            //std::cout << "color coding: " << getColorCodingString_dc1394(frame_dc1394_ -> color_coding) << std::endl;
            //std::cout << "size:         " << frame_dc1394_ -> size[0] << ", " << frame_dc1394_ -> size[1] << std::endl;
            //std::cout << "total bytes:  " << frame_dc1394_ -> total_bytes << std::endl;
            //std::cout << "frameSize:    " << frameSize << std::endl;
        }
    }


    cv::Mat CameraDevice_dc1394::grabImage()
    {
        cv::Mat image;
        grabImage(image);
        return image;
    }


    bool CameraDevice_dc1394::isColor()
    {
        bool isColor = false;
        if (!connected_)
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get color coding - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_IS_COLOR, ssError.str());
        }

        dc1394video_modes_t supportedModes_dc1394;
        dc1394error_t rsp = dc1394_video_get_supported_modes(
                camera_dc1394_, 
                &supportedModes_dc1394
                );
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get supported video modes" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_VIDEOMODES, ssError.str());
        }

        for (int i=0; i<supportedModes_dc1394.num; i++)
        {
            dc1394color_coding_t colorCoding_dc1934;
            rsp = dc1394_get_color_coding_from_video_mode(
                    camera_dc1394_, 
                    supportedModes_dc1394.modes[i], 
                    &colorCoding_dc1934
                    );
            if (rsp != DC1394_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": error unable to get color coding from video mode" << std::endl;
                throw RuntimeError(ERROR_DC1394_GET_COLOR_CODING_FROM_VIDEOMODE, ssError.str());
            } 
            dc1394bool_t isColor_dc1394;
            rsp = dc1394_is_color(colorCoding_dc1934, &isColor_dc1394);
            if (rsp != DC1394_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": error unable to determine if color coding is color or monochrome" << std::endl;
                throw RuntimeError(ERROR_DC1394_IS_COLOR, ssError.str());
            }
            if (isColor_dc1394 == DC1394_TRUE)
            {
                isColor = true;
            }
        } // for (int i= ...

        return isColor;
    }


    bool CameraDevice_dc1394::isSupported(ImageMode imgMode)
    {
        try 
        {
            convertVideoMode_to_dc1394(VIDEOMODE_FORMAT7, imgMode);
        }
        catch (RuntimeError &runtimeError)
        {
            return false;
        }
        return true;
    }

    unsigned int CameraDevice_dc1394::getNumberOfImageMode()
    {
        return NUMBER_OF_DC1394_IMAGEMODE;
    }


    bool CameraDevice_dc1394::isSupported(VideoMode vidMode, FrameRate frmRate)
    {
        bool value = false;
        VideoModeList videoModeList = getAllowedVideoModes();
        VideoModeList::iterator modeIt = std::find(videoModeList.begin(), videoModeList.end(), vidMode);
        if (*modeIt == vidMode)
        {
            FrameRateList frameRateList = getAllowedFrameRates(vidMode);
            FrameRateList::iterator rateIt = std::find(frameRateList.begin(), frameRateList.end(), frmRate);
            if (*rateIt == frmRate)
            {
                value = true;
            }
        }
        return value;
    }


    VideoMode CameraDevice_dc1394::getVideoMode()
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get current video mode - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_VIDEOMODE, ssError.str());
        }

        dc1394video_mode_t videoMode_dc1394;
        dc1394error_t rsp = dc1394_video_get_mode(camera_dc1394_, &videoMode_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get current video mode" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_VIDEOMODE, ssError.str());
        }
        return convertVideoMode_from_dc1394(videoMode_dc1394);
    }


    FrameRate CameraDevice_dc1394::getFrameRate()
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get current framerate - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_FRAMERATE, ssError.str());
        }

        FrameRate frameRate;
        VideoMode videoMode = getVideoMode();
        if (videoMode ==VIDEOMODE_FORMAT7)
        {
            frameRate = FRAMERATE_FORMAT7;
        }
        else
        {
            dc1394framerate_t frameRate_dc1394;
            dc1394error_t rsp = dc1394_video_get_framerate(camera_dc1394_, &frameRate_dc1394);
            if (rsp != DC1394_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": error unable to get current framerate" << std::endl;
                throw RuntimeError(ERROR_DC1394_GET_FRAMERATE, ssError.str());
            }
            frameRate = convertFrameRate_from_dc1394(frameRate_dc1394);
        }
        return frameRate;
    }


    ImageMode CameraDevice_dc1394::getImageMode()
    { 
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get image mode from video mode - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_VIDEOMODE, ssError.str());
        }

        dc1394video_mode_t videoMode_dc1394;
        dc1394error_t  rsp = dc1394_video_get_mode(camera_dc1394_, &videoMode_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get current video mode" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_VIDEOMODE, ssError.str());
        }
        return convertImageMode_from_dc1394(videoMode_dc1394);
    }


    VideoModeList CameraDevice_dc1394::getAllowedVideoModes()
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get supported video modes - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_VIDEOMODES, ssError.str());
        }

        VideoModeList videoModeList;
        dc1394video_modes_t supportedModes_dc1394;
        dc1394error_t rsp = dc1394_video_get_supported_modes(
                camera_dc1394_, 
                &supportedModes_dc1394
                );
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get supported video modes" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_VIDEOMODES, ssError.str());
        }

        bool hasFormat7 = false;
        for (int i=0; i<supportedModes_dc1394.num; i++)
        {
            VideoMode videoMode = convertVideoMode_from_dc1394(supportedModes_dc1394.modes[i]);
            if (videoMode == VIDEOMODE_FORMAT7)
            {
                if (hasFormat7)
                {
                    continue;
                }
                else
                {
                    hasFormat7 = true;
                }
            }
            videoModeList.push_back(videoMode);
        }
        return videoModeList;
    }


    FrameRateList CameraDevice_dc1394::getAllowedFrameRates(VideoMode vidMode) 
    { 
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get supported framerates - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_FRAMERATES, ssError.str());
        }

        FrameRateList frameRateList;

        if (vidMode == VIDEOMODE_FORMAT7)
        {
            frameRateList.push_back(FRAMERATE_FORMAT7);
        }
        else
        {
            // Note, ImageMode doesn't matter if VideoMode is not format7.
            dc1394video_mode_t vidMode_dc1394 = convertVideoMode_to_dc1394(vidMode,IMAGEMODE_0);
            dc1394framerates_t supportedRates_dc1394;
            dc1394error_t rsp = dc1394_video_get_supported_framerates(
                    camera_dc1394_, 
                    vidMode_dc1394, 
                    &supportedRates_dc1394
                    );
            if (rsp != DC1394_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": error unable to get supported framerates" << std::endl;
                throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_FRAMERATES, ssError.str());
            }

            for (int i=0; i<supportedRates_dc1394.num; i++)
            {
                FrameRate frameRate = convertFrameRate_from_dc1394(supportedRates_dc1394.framerates[i]);
                frameRateList.push_back(frameRate);
            }
        }
        return frameRateList; 
    } 


    ImageModeList CameraDevice_dc1394::getAllowedImageModes()
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get supported modes - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_FRAMERATES, ssError.str());
        }

        ImageModeList imageModeList;
        dc1394video_modes_t supportedModes_dc1394;
        dc1394error_t rsp = dc1394_video_get_supported_modes(
                camera_dc1394_, 
                &supportedModes_dc1394
                ); 
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get supported modes" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_SUPPORTED_FRAMERATES, ssError.str());
        }

        for (int i=0; i<supportedModes_dc1394.num; i++)
        {
            ImageMode imageMode = convertImageMode_from_dc1394(supportedModes_dc1394.modes[i]);
            imageModeList.push_back(imageMode);
        }
        return imageModeList;
    }


    Property CameraDevice_dc1394::getProperty(PropertyType propType) 
    {
        dc1394feature_info_t featureInfo_dc1394;
        getFeatureInfo_dc1394(propType, featureInfo_dc1394);
        Property prop = convertProperty_from_dc1394(featureInfo_dc1394);
        return prop;
    }

    PropertyInfo CameraDevice_dc1394::getPropertyInfo(PropertyType propType) 
    {
        dc1394feature_info_t featureInfo_dc1394;
        getFeatureInfo_dc1394(propType, featureInfo_dc1394);
        PropertyInfo propInfo = convertPropertyInfo_from_dc1394(featureInfo_dc1394);
        return propInfo;
    }

    void CameraDevice_dc1394::setProperty(Property prop) 
    {
        dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(prop.type);

        dc1394feature_info_t featureInfo_dc1394;
        getFeatureInfo_dc1394(prop.type, featureInfo_dc1394);
        Property propCurr = convertProperty_from_dc1394(featureInfo_dc1394);
        PropertyInfo propInfoCurr = convertPropertyInfo_from_dc1394(featureInfo_dc1394);

        if (!propInfoCurr.present)
        {
            return;
        }


        if (prop.autoActive && propCurr.autoActive)
        {
            return;
        }

        if (prop.autoActive && !propCurr.autoActive && propInfoCurr.autoCapable)
        {
            setFeatureModeAuto_dc1394(prop.type);
            return;
        }

        if(prop.onePush && propInfoCurr.onePushCapable)
        {
            setFeatureModeOnePush_dc1394(prop.type);
            return;
        }

        if (!prop.autoActive && propCurr.autoActive && propInfoCurr.manualCapable)
        {
            setFeatureModeManual_dc1394(prop.type);
        }

        if (prop.absoluteControl)
        {
            if (!propCurr.absoluteControl)
            {
                setFeatureAbsoluteControl_dc1394(prop.type);
            }
            setFeatureAbsoluteValue_dc1394(prop.type, prop.absoluteValue, propInfoCurr);
        }
        else
        {
            if (prop.type == PROPERTY_TYPE_WHITE_BALANCE)
            {
                setFeatureWhiteBalance_dc1394(prop.valueA, prop.valueB);
            }
            else
            {
                setFeatureValue_dc1394(prop.type,prop.value,propInfoCurr);
            }
        }
    }

    TimeStamp CameraDevice_dc1394::getImageTimeStamp()
    {
        return timeStamp_;
    }


    Format7Settings CameraDevice_dc1394::getFormat7Settings()
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get format7 settings - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_FORMAT7_SETTINGS , ssError.str()); 
        }

        // Get current video mode
        dc1394video_mode_t videoMode_dc1394 = getVideoMode_dc1394();

        // Get position of ROI
        uint32_t left;
        uint32_t top;
        dc1394error_t rsp = dc1394_format7_get_image_position(camera_dc1394_, videoMode_dc1394, &left, &top);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get image position" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_IMAGE_POSITION , ssError.str()); 
        }

        Format7Settings format7Settings;
        format7Settings.offsetX = (unsigned int)(left);
        format7Settings.offsetY = (unsigned int)(top);

        // Get width of ROI
        uint32_t width;
        uint32_t height;
        rsp = dc1394_format7_get_image_size(camera_dc1394_, videoMode_dc1394, &width, &height);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get image position" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_IMAGE_POSITION , ssError.str()); 
        }
        format7Settings.width = (unsigned int)(width);
        format7Settings.height = (unsigned int)(height);

        // Get pixel format
        dc1394color_coding_t colorCoding_dc1394;
        rsp = dc1394_format7_get_color_coding(camera_dc1394_, videoMode_dc1394, &colorCoding_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get format7 color coding" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_COLOR_CODING , ssError.str()); 
        }
        format7Settings.pixelFormat = convertPixelFormat_from_dc1394(colorCoding_dc1394);

        // Get image mode
        format7Settings.mode = convertImageMode_from_dc1394(videoMode_dc1394);

        return format7Settings;
    }

    Format7Info CameraDevice_dc1394::getFormat7Info(ImageMode imgMode)
    {
        dc1394video_mode_t videoMode_dc1394 = convertVideoMode_to_dc1394(VIDEOMODE_FORMAT7, imgMode);

        // Get videomode info sturcture
        dc1394format7modeset_t format7ModeSet_dc1394;
        dc1394error_t rsp =  dc1394_format7_get_modeset(camera_dc1394_,&format7ModeSet_dc1394);
        dc1394format7mode_t *format7ModeInfo_dc1394 = &format7ModeSet_dc1394.mode[imgMode];
        rsp = dc1394_format7_get_mode_info(camera_dc1394_, videoMode_dc1394, format7ModeInfo_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get format7 mode info" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_COLOR_CODING , ssError.str()); 
        }
        //printFormat7ModeInfo_dc1394(*format7ModeInfo_dc1394);
        Format7Info format7Info = convertFormat7Info_from_dc1394(imgMode, *format7ModeInfo_dc1394);
        //format7Info.print();
        return format7Info;
    }


    bool CameraDevice_dc1394::validateFormat7Settings(Format7Settings settings)
    {
        return true;
    } 
   
    void CameraDevice_dc1394::setFormat7Configuration(Format7Settings settings, float percentSpeed)
    {

        dc1394video_mode_t videoMode_dc1394 = getVideoMode_dc1394();

        dc1394error_t rsp = dc1394_format7_set_image_size(camera_dc1394_, videoMode_dc1394, settings.width, settings.height);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set format7 image size" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FORMAT7_IMAGE_SIZE , ssError.str()); 
        }

        rsp = dc1394_format7_set_image_position(camera_dc1394_, videoMode_dc1394, settings.offsetX, settings.offsetY);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set format7 image position" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FORMAT7_IMAGE_POSITION , ssError.str()); 
        }

        dc1394color_coding_t colorCoding_dc1394 = convertPixelFormat_to_dc1394(settings.pixelFormat);
        rsp = dc1394_format7_set_color_coding(camera_dc1394_, videoMode_dc1394, colorCoding_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set format7 pixel format (color coding)" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FORMAT7_COLOR_CODING , ssError.str()); 
        }
    }


    PixelFormatList CameraDevice_dc1394::getListOfSupportedPixelFormats(ImageMode imgMode)
    {
        dc1394video_mode_t videoMode_dc1394 = convertVideoMode_to_dc1394(VIDEOMODE_FORMAT7,imgMode);

        dc1394color_codings_t colorCodings_dc1394;
        dc1394error_t rsp = dc1394_format7_get_color_codings(camera_dc1394_, videoMode_dc1394, &colorCodings_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get dc1394 format7 color codings" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_EXTERNAL_TRIGGER_POWER, ssError.str());
        }

        PixelFormatList pixelFormatList;
        for (int i=0; i<colorCodings_dc1394.num; i++)
        {
            dc1394color_coding_t coding_dc1394 = colorCodings_dc1394.codings[i];
            PixelFormat pixelFormat = convertPixelFormat_from_dc1394(coding_dc1394);
            pixelFormatList.push_back(pixelFormat);
        }
        return pixelFormatList;
    }


    void CameraDevice_dc1394::setTriggerInternal() 
    {
        dc1394error_t rsp = dc1394_external_trigger_set_power(camera_dc1394_, DC1394_OFF);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 external trigger power setting to off" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_EXTERNAL_TRIGGER_POWER, ssError.str());
        }
    }     


    void CameraDevice_dc1394::setTriggerExternal() 
    {

        setExternalTriggerMode_dc1394(DC1394_TRIGGER_MODE_0); // class method

        dc1394error_t rsp = dc1394_external_trigger_set_polarity(camera_dc1394_, DC1394_TRIGGER_ACTIVE_HIGH);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 external trigger polarity" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_EXTERNAL_TRIGGER_POLARITY, ssError.str());
        }

        rsp = dc1394_external_trigger_set_power(camera_dc1394_, DC1394_ON);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 external trigger power setting to on" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_EXTERNAL_TRIGGER_POWER, ssError.str());
        }
    }     


    TriggerType CameraDevice_dc1394::getTriggerType() 
    {
        dc1394switch_t trigSwitch_dc1394 = DC1394_OFF;
        dc1394error_t rsp = dc1394_external_trigger_get_power(camera_dc1394_, &trigSwitch_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get dc1394 external trigger power setting" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_EXTERNAL_TRIGGER_POWER, ssError.str());
        }

        TriggerType trigType = TRIGGER_TYPE_UNSPECIFIED;
        if (trigSwitch_dc1394 == DC1394_ON)
        {
            trigType = TRIGGER_EXTERNAL;
        }
        else
        {
            trigType = TRIGGER_INTERNAL;
        }
        return trigType;
    }


    std::string CameraDevice_dc1394::getVendorName() 
    { 
        std::string vendorName;
        if (connected_)
        {
            vendorName = std::string(camera_dc1394_ -> vendor);
        }
        else
        {
            vendorName = std::string("None");
        }
        return vendorName;
    } 

    std::string CameraDevice_dc1394::getModelName() 
    { 
        std::string modelName;
        if (connected_)
        {
            modelName = std::string(camera_dc1394_ -> model);
        }
        else
        {
            modelName = std::string("None");
        }
        return modelName;
    } 

    std::string CameraDevice_dc1394::toString()
    {
        std::stringstream ss;
        ss << std::endl;
        ss << " ------------------ " << std::endl;
        ss << " Camera Information " << std::endl;
        ss << " ------------------ " << std::endl;
        ss << std::endl;
        if (connected_)
        {

            ss << " guid:        " << guid_ << std::endl;
            ss << " vendor Name: " << camera_dc1394_ -> vendor << std::endl;
            ss << " vendor Id:   " << std::hex << camera_dc1394_ -> vendor_id  << std::dec << std::endl;
            ss << " model Name:  " << std::hex << camera_dc1394_ -> model << std::dec << std::endl;
            ss << " model Id:    " << camera_dc1394_ -> model_id << std::endl;
        }
        else 
        {
            ss << " camera not connected" << std::endl;
        }
        ss << std::endl;
        return ss.str();
    }


    void CameraDevice_dc1394::printGuid()
    {
        guid_.printValue();
    }


    void CameraDevice_dc1394::printInfo()
    {
        std::cout << toString();
    }

    // Private methods
    // ------------------------------------------------------------------------
    

    void CameraDevice_dc1394::updateTimeStamp()
    {
        double elapsedTime = 0.0;
#ifdef WIN32
        if (isFirst_) 
        {
            // Get Start time and timer frequency
            LARGE_INTEGER startTimeLI;
            QueryPerformanceCounter(&startTimeLI);
            startTime_ = uint64_t(startTimeLI.QuadPart);

            LARGE_INTEGER timerFreqLI;
            QueryPerformanceFrequency(&timerFreqLI);
            timerFreq_ = uint64_t(timerFreqLI.QuadPart);
        }

        LARGE_INTEGER currentTimeLI;
        QueryPerformanceCounter(&currentTimeLI);
        uint64_t currentTime = uint64_t(currentTimeLI.QuadPart);
        elapsedTime = (currentTime - startTime_)/double(timerFreq_);

#else
        if (isFirst_)
        {
            startTime_ = frame_dc1394_  -> timestamp; 
            timerFreq_ = 1;  // Not used
        }
        elapsedTime = 1.0e-6*double(frame_dc1394_ -> timestamp - startTime_);
#endif
        timeStamp_.seconds = (long long)(elapsedTime);
        timeStamp_.microSeconds = (unsigned int)(1.0e6*(elapsedTime - double(timeStamp_.seconds)));
    }
    

    void CameraDevice_dc1394::getFeatureInfo_dc1394(
            PropertyType propType, 
            dc1394feature_info_t &featureInfo_dc1394
            )
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get dc1394 feature information - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_FEATURE_INFO, ssError.str());
        }

        featureInfo_dc1394.id = convertPropertyType_to_dc1394(propType);
        dc1394error_t rsp = dc1394_feature_get(camera_dc1394_, &featureInfo_dc1394);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get dc1394 feature information" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_FEATURE_INFO, ssError.str());
        }
    }


    void CameraDevice_dc1394::setFeatureModeAuto_dc1394(PropertyType propType)
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode to auto - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }

        dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(propType);
        dc1394error_t rsp = dc1394_feature_set_mode( camera_dc1394_,featureId_dc1394, DC1394_FEATURE_MODE_AUTO);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode to auto" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }
    }


    void CameraDevice_dc1394::setFeatureModeManual_dc1394(PropertyType propType)
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode to maunual - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }
        dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(propType);
        dc1394error_t rsp = dc1394_feature_set_mode( camera_dc1394_,featureId_dc1394, DC1394_FEATURE_MODE_MANUAL);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode to manual" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }
    }


    void CameraDevice_dc1394::setFeatureModeOnePush_dc1394(PropertyType propType)
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode to one push auto - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }

        dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(propType);
        dc1394error_t rsp = dc1394_feature_set_mode( camera_dc1394_,featureId_dc1394, DC1394_FEATURE_MODE_ONE_PUSH_AUTO);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode to one push auto" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }
    }


    void CameraDevice_dc1394::setFeatureAbsoluteControl_dc1394(PropertyType propType)
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode absolute control - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }

        dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(propType);
        dc1394error_t rsp = dc1394_feature_set_absolute_control( camera_dc1394_,featureId_dc1394, DC1394_ON);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature mode absolute control" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }
    }


    void CameraDevice_dc1394::setFeatureAbsoluteValue_dc1394(PropertyType propType, float absValue, PropertyInfo propInfo)
    {
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature absolute value - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }

        if ((absValue >= propInfo.minAbsoluteValue) && (absValue <= propInfo.maxAbsoluteValue))
        {
            dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(propType);
            dc1394error_t rsp = dc1394_feature_set_absolute_value(camera_dc1394_, featureId_dc1394, absValue);
            if (rsp != DC1394_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": error unable to set dc1394 feature absolute value" << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
            }
        }
    }


   void CameraDevice_dc1394::setFeatureValue_dc1394(PropertyType propType, unsigned int value, PropertyInfo propInfo)
   {
       if (!connected_)
       {
           std::stringstream ssError;
           ssError << __PRETTY_FUNCTION__;
           ssError << ": error unable to set dc1394 feature value - not connected" << std::endl;
           throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
       }

       if ((value >= propInfo.minValue) && (value <= propInfo.maxValue))
       {
           dc1394feature_t featureId_dc1394 = convertPropertyType_to_dc1394(propType);
           dc1394error_t  rsp = dc1394_feature_set_value(camera_dc1394_, featureId_dc1394, (uint32_t)(value));
           if (rsp != DC1394_SUCCESS)
           {
               std::stringstream ssError;
               ssError << __PRETTY_FUNCTION__;
               ssError << ": error unable to set dc1394 feature value" << std::endl;
               throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
           }
       }
   }


   void CameraDevice_dc1394::setFeatureWhiteBalance_dc1394(unsigned int valueA, unsigned int valueB)
   {
       if (!connected_)
       {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature white balance - not connected" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
       }

        dc1394error_t  rsp = dc1394_feature_whitebalance_set_value(camera_dc1394_,valueB,valueA);
        if (rsp != DC1394_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 feature white balance" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_FEATURE, ssError.str());
        }
   }

   void CameraDevice_dc1394::setExternalTriggerMode_dc1394(dc1394trigger_mode_t triggerMode_dc1394)
   {
       dc1394error_t rsp = dc1394_external_trigger_set_mode(camera_dc1394_, triggerMode_dc1394);
       if (rsp != DC1394_SUCCESS)
       {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 external trigger mode" << std::endl;
            throw RuntimeError(ERROR_DC1394_SET_EXTERNAL_TRIGGER_MODE, ssError.str());
       }
   }

   dc1394trigger_mode_t CameraDevice_dc1394::getExternalTriggerMode_dc1394()
   {
       dc1394trigger_mode_t triggerMode_dc1394 = DC1394_TRIGGER_MODE_0;
       dc1394error_t rsp = dc1394_external_trigger_get_mode(camera_dc1394_, &triggerMode_dc1394);
       if (rsp != DC1394_SUCCESS)
       {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to set dc1394 external trigger mode" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_EXTERNAL_TRIGGER_MODE, ssError.str());
       }
       return triggerMode_dc1394;
   }

   dc1394video_mode_t CameraDevice_dc1394::getVideoMode_dc1394()
   {
        dc1394video_mode_t videoMode_dc1394;
        dc1394error_t rsp = dc1394_video_get_mode(camera_dc1394_, &videoMode_dc1394);
        if (rsp != DC1394_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error unable to get video mode" << std::endl;
            throw RuntimeError(ERROR_DC1394_GET_VIDEOMODE, ssError.str()); 
        }
        return videoMode_dc1394;
   }

} // namespace bias

#endif  // #ifdef WITH_DC1394
