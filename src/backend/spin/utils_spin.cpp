#ifdef WITH_SPIN
#include "utils_spin.hpp"
#include "exception.hpp"
#include <opencv2/core/core.hpp>
#include <map>
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias 
{
    const size_t MAX_BUF_LEN = 256;

    ImageInfo_spin getImageInfo_spin(spinImage hImage)
    {
        ImageInfo_spin info = {0,0,0,0,0,nullptr,UNKNOWN_PIXELFORMAT};
        spinError err = SPINNAKER_ERR_SUCCESS;

        err = spinImageGetPaddingX(hImage, &info.xpad);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage x padding, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PADDING_X, ssError.str());
        }

        err = spinImageGetPaddingY(hImage, &info.ypad);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage y padding, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PADDING_Y, ssError.str());
        }

        err = spinImageGetWidth(hImage, &info.cols);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage width, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_WIDTH, ssError.str());
        }

        err = spinImageGetHeight(hImage, &info.rows);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage width, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_HEIGHT, ssError.str());
        }

        err = spinImageGetStride(hImage, &info.stride);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage stride, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_STRIDE, ssError.str());
        }

        err = spinImageGetData(hImage,&info.dataPtr);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage data pointer, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_DATA_PTR, ssError.str());
        }

        err = spinImageGetPixelFormat(hImage, &info.pixelFormat);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage pixel format, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PIXEL_FORMAT, ssError.str());
        }

        return info;
    }

    //// Image conversion - for mapping from Spinnaker to opencv 
    //// -----------------------------------------------------------------------
    //
    //spinPixelFormat getSuitablePixelFormat(spinPixelFormat pixFormat)
    //{
    //    spinPixelFormat convPixFormat = SPIN_PIXEL_FORMAT_MONO8;

    //    switch (pixFormat)
    //    {
    //        case SPIN_PIXEL_FORMAT_RAW8:
    //            convPixFormat = SPIN_PIXEL_FORMAT_RAW8;
    //            break;

    //        case SPIN_PIXEL_FORMAT_MONO8:
    //            convPixFormat = SPIN_PIXEL_FORMAT_MONO8;
    //            break;

    //        case SPIN_PIXEL_FORMAT_RAW12:
    //        case SPIN_PIXEL_FORMAT_RAW16:
    //        case SPIN_PIXEL_FORMAT_MONO12:
    //        case SPIN_PIXEL_FORMAT_MONO16:
    //        case SPIN_PIXEL_FORMAT_S_MONO16:
    //            convPixFormat = SPIN_PIXEL_FORMAT_MONO16;
    //            break;

    //        case SPIN_PIXEL_FORMAT_RGB16:
    //        case SPIN_PIXEL_FORMAT_S_RGB16:
    //        case SPIN_PIXEL_FORMAT_BGR:
    //        case SPIN_PIXEL_FORMAT_BGRU:
    //        case SPIN_PIXEL_FORMAT_RGB:
    //        case SPIN_PIXEL_FORMAT_RGBU:
    //        case SPIN_PIXEL_FORMAT_BGR16:
	//        case SPIN_PIXEL_FORMAT_BGRU16:
    //        case SPIN_PIXEL_FORMAT_422YUV8_JPEG:
    //        case SPIN_PIXEL_FORMAT_411YUV8:
    //        case SPIN_PIXEL_FORMAT_422YUV8:
    //        case SPIN_PIXEL_FORMAT_444YUV8:
    //            convPixFormat = SPIN_PIXEL_FORMAT_BGR;
    //            break;

    //        default:
    //            break;
    //    }

    //    return convPixFormat;

    //}


    //int getCompatibleOpencvFormat(spinPixelFormat pixFormat)
    //{
    //    int opencvFormat = CV_8UC1;

    //    switch (pixFormat)
    //    {
    //        case SPIN_PIXEL_FORMAT_RAW8:
    //        case SPIN_PIXEL_FORMAT_MONO8:
    //            opencvFormat = CV_8UC1;
    //            break;

    //        case SPIN_PIXEL_FORMAT_MONO16:
    //            opencvFormat = CV_16UC1;
    //            break;

    //        case SPIN_PIXEL_FORMAT_BGR:
    //            opencvFormat = CV_8UC3;
    //            break;

    //        default:
    //            std::stringstream ssError;
    //            ssError << "no compatible opencv format" << std::endl;
    //            throw RuntimeError(
    //                    ERROR_SPIN_NO_COMPATIBLE_OPENCV_FORMAT, 
    //                    ssError.str()
    //                    );
    //            break;

    //    }
    //    return opencvFormat;
    //}
    //
    //// Conversion from BIAS types to Spinnaker types
    //// ------------------------------------------------------------------------

    //static std::map<PropertyType, spinPropertyType> createPropertyTypeMap_to_spin()
    //{
    //    std::map<PropertyType, spinPropertyType> map;
    //    map[PROPERTY_TYPE_BRIGHTNESS]     =   SPIN_BRIGHTNESS;
    //    map[PROPERTY_TYPE_AUTO_EXPOSURE]  =   SPIN_AUTO_EXPOSURE;
    //    map[PROPERTY_TYPE_SHARPNESS]      =   SPIN_SHARPNESS;
    //    map[PROPERTY_TYPE_WHITE_BALANCE]  =   SPIN_WHITE_BALANCE;
    //    map[PROPERTY_TYPE_HUE]            =   SPIN_HUE;
    //    map[PROPERTY_TYPE_SATURATION]     =   SPIN_SATURATION;
    //    map[PROPERTY_TYPE_GAMMA]          =   SPIN_GAMMA;
    //    map[PROPERTY_TYPE_IRIS]           =   SPIN_IRIS;
    //    map[PROPERTY_TYPE_FOCUS]          =   SPIN_FOCUS;
    //    map[PROPERTY_TYPE_ZOOM]           =   SPIN_ZOOM;
    //    map[PROPERTY_TYPE_PAN]            =   SPIN_PAN;
    //    map[PROPERTY_TYPE_TILT]           =   SPIN_TILT;
    //    map[PROPERTY_TYPE_SHUTTER]        =   SPIN_SHUTTER;
    //    map[PROPERTY_TYPE_GAIN]           =   SPIN_GAIN;
    //    map[PROPERTY_TYPE_TRIGGER_MODE]   =   SPIN_TRIGGER_MODE;
    //    map[PROPERTY_TYPE_TRIGGER_DELAY]  =   SPIN_TRIGGER_DELAY;
    //    map[PROPERTY_TYPE_FRAME_RATE]     =   SPIN_FRAME_RATE;
    //    map[PROPERTY_TYPE_TEMPERATURE]    =   SPIN_TEMPERATURE;
    //    return map;
    //};

    //static std::map<PropertyType, spinPropertyType> propertyTypeMap_to_spin = 
    //    createPropertyTypeMap_to_spin();

    //spinPropertyType convertPropertyType_to_spin(PropertyType propType)
    //{
    //    if (propertyTypeMap_to_spin.count(propType) != 0)
    //    {
    //        return propertyTypeMap_to_spin[propType];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert property to FlyCaptuer2 property";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_PROPERTY_TYPE, ssError.str());
    //    }
    //}

    //static std::map<FrameRate, spinFrameRate> createFrameRateMap_to_spin()
    //{
    //    std::map<FrameRate, spinFrameRate> map;
    //    map[FRAMERATE_1_875]    = SPIN_FRAMERATE_1_875;
    //    map[FRAMERATE_3_75]     = SPIN_FRAMERATE_3_75;
    //    map[FRAMERATE_7_5]      = SPIN_FRAMERATE_7_5;
    //    map[FRAMERATE_15]       = SPIN_FRAMERATE_15;
    //    map[FRAMERATE_30]       = SPIN_FRAMERATE_30;
    //    map[FRAMERATE_60]       = SPIN_FRAMERATE_60;
    //    map[FRAMERATE_120]      = SPIN_FRAMERATE_120;
    //    map[FRAMERATE_240]      = SPIN_FRAMERATE_240;
    //    map[FRAMERATE_FORMAT7]  = SPIN_FRAMERATE_FORMAT7;
    //    return map;
    //};

    //static std::map<FrameRate, spinFrameRate> frameRateMap_to_spin = 
    //    createFrameRateMap_to_spin();


    //spinFrameRate convertFrameRate_to_spin(FrameRate frmRate)
    //{
    //    if (frameRateMap_to_spin.count(frmRate) != 0)
    //    {
    //        return frameRateMap_to_spin[frmRate];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert framerate to spin frame rate";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_FRAMERATE, ssError.str());
    //    }
    //}

    //static std::map<VideoMode, spinVideoMode> createVideoModeMap_to_spin()
    //{
    //    std::map<VideoMode, spinVideoMode> map;
    //    map[VIDEOMODE_160x120YUV444]   =   SPIN_VIDEOMODE_160x120YUV444;
    //    map[VIDEOMODE_320x240YUV422]   =   SPIN_VIDEOMODE_320x240YUV422;
    //    map[VIDEOMODE_640x480YUV411]   =   SPIN_VIDEOMODE_640x480YUV411;
    //    map[VIDEOMODE_640x480YUV422]   =   SPIN_VIDEOMODE_640x480YUV422;
    //    map[VIDEOMODE_640x480RGB]      =   SPIN_VIDEOMODE_640x480RGB;
    //    map[VIDEOMODE_640x480Y8]       =   SPIN_VIDEOMODE_640x480Y8;
    //    map[VIDEOMODE_640x480Y16]      =   SPIN_VIDEOMODE_640x480Y16;
    //    map[VIDEOMODE_800x600YUV422]   =   SPIN_VIDEOMODE_800x600YUV422;
    //    map[VIDEOMODE_800x600RGB]      =   SPIN_VIDEOMODE_800x600RGB;
    //    map[VIDEOMODE_800x600Y8]       =   SPIN_VIDEOMODE_800x600Y8;
    //    map[VIDEOMODE_800x600Y16]      =   SPIN_VIDEOMODE_800x600Y16;
    //    map[VIDEOMODE_1024x768YUV422]  =   SPIN_VIDEOMODE_1024x768YUV422;
    //    map[VIDEOMODE_1024x768RGB]     =   SPIN_VIDEOMODE_1024x768RGB;
    //    map[VIDEOMODE_1024x768Y8]      =   SPIN_VIDEOMODE_1024x768Y8;
    //    map[VIDEOMODE_1024x768Y16]     =   SPIN_VIDEOMODE_1024x768Y16;
    //    map[VIDEOMODE_1280x960YUV422]  =   SPIN_VIDEOMODE_1280x960YUV422;
    //    map[VIDEOMODE_1280x960RGB]     =   SPIN_VIDEOMODE_1280x960RGB;
    //    map[VIDEOMODE_1280x960Y8]      =   SPIN_VIDEOMODE_1280x960Y8;
    //    map[VIDEOMODE_1280x960Y16]     =   SPIN_VIDEOMODE_1280x960Y16;
    //    map[VIDEOMODE_1600x1200YUV422] =   SPIN_VIDEOMODE_1600x1200YUV422;
    //    map[VIDEOMODE_1600x1200RGB]    =   SPIN_VIDEOMODE_1600x1200RGB;
    //    map[VIDEOMODE_1600x1200Y8]     =   SPIN_VIDEOMODE_1600x1200Y8;
    //    map[VIDEOMODE_1600x1200Y16]    =   SPIN_VIDEOMODE_1600x1200Y16;
    //    map[VIDEOMODE_FORMAT7]         =   SPIN_VIDEOMODE_FORMAT7;
    //    return map;
    //}

    //static std::map<VideoMode, spinVideoMode> videoModeMap_to_spin = 
    //    createVideoModeMap_to_spin();

    //spinVideoMode convertVideoMode_to_spin(VideoMode vidMode)
    //{
    //    if (videoModeMap_to_spin.count(vidMode) != 0)
    //    {
    //        return videoModeMap_to_spin[vidMode];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert video mode to Spinnaker video mode";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_VIDEOMODE, ssError.str());
    //    }
    //}

    //static std::map<ImageMode, spinMode> createImageModeMap_to_spin()
    //{
    //    std::map<ImageMode, spinMode> map;
    //    map[IMAGEMODE_0]   =  SPIN_MODE_0;
    //    map[IMAGEMODE_1]   =  SPIN_MODE_1;
    //    map[IMAGEMODE_2]   =  SPIN_MODE_2;
    //    map[IMAGEMODE_3]   =  SPIN_MODE_3;
    //    map[IMAGEMODE_4]   =  SPIN_MODE_4;
    //    map[IMAGEMODE_5]   =  SPIN_MODE_5;
    //    map[IMAGEMODE_6]   =  SPIN_MODE_6;
    //    map[IMAGEMODE_7]   =  SPIN_MODE_7;
    //    map[IMAGEMODE_8]   =  SPIN_MODE_8;
    //    map[IMAGEMODE_9]   =  SPIN_MODE_9;
    //    map[IMAGEMODE_10]  =  SPIN_MODE_10;
    //    map[IMAGEMODE_11]  =  SPIN_MODE_11;
    //    map[IMAGEMODE_12]  =  SPIN_MODE_12;
    //    map[IMAGEMODE_13]  =  SPIN_MODE_13;
    //    map[IMAGEMODE_14]  =  SPIN_MODE_14;
    //    map[IMAGEMODE_15]  =  SPIN_MODE_15;
    //    map[IMAGEMODE_16]  =  SPIN_MODE_16;
    //    map[IMAGEMODE_17]  =  SPIN_MODE_17;
    //    map[IMAGEMODE_18]  =  SPIN_MODE_18;
    //    map[IMAGEMODE_19]  =  SPIN_MODE_19;
    //    map[IMAGEMODE_20]  =  SPIN_MODE_20;
    //    map[IMAGEMODE_21]  =  SPIN_MODE_21;
    //    map[IMAGEMODE_22]  =  SPIN_MODE_22;
    //    map[IMAGEMODE_23]  =  SPIN_MODE_23;
    //    map[IMAGEMODE_24]  =  SPIN_MODE_24;
    //    map[IMAGEMODE_25]  =  SPIN_MODE_25;
    //    map[IMAGEMODE_26]  =  SPIN_MODE_26;
    //    map[IMAGEMODE_27]  =  SPIN_MODE_27;
    //    map[IMAGEMODE_28]  =  SPIN_MODE_28;
    //    map[IMAGEMODE_29]  =  SPIN_MODE_29;
    //    map[IMAGEMODE_30]  =  SPIN_MODE_30;
    //    map[IMAGEMODE_31]  =  SPIN_MODE_31;
    //    return map;
    //}

    //static std::map<ImageMode, spinMode> imageModeMap_to_spin = 
    //    createImageModeMap_to_spin();

    //spinMode convertImageMode_to_spin(ImageMode imgMode)
    //{
    //    if (imageModeMap_to_spin.count(imgMode) != 0) 
    //    {
    //        return imageModeMap_to_spin[imgMode];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert image mode to Spinnaker ";
    //        ssError << "imaging mode";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_IMAGEMODE, ssError.str());
    //    }
    //}

    //spinProperty convertProperty_to_spin(Property prop)
    //{
    //    spinProperty prop_spin;
    //    prop_spin.type = convertPropertyType_to_spin(prop.type);
    //    prop_spin.present = (prop.present) ? 1 : 0;
    //    prop_spin.absControl = (prop.absoluteControl) ? 1 : 0;
    //    prop_spin.onePush = (prop.onePush) ? 1 : 0;
    //    prop_spin.onOff = (prop.on) ? 1 : 0;
    //    prop_spin.autoManualMode = (prop.autoActive) ? 1 : 0;
    //    if (prop.type == PROPERTY_TYPE_WHITE_BALANCE)
    //    {   
    //        prop_spin.valueA = prop.valueA;
    //        prop_spin.valueB = prop.valueB;
    //    }
    //    else
    //    {
    //        prop_spin.valueA = prop.value;
    //        prop_spin.valueB = 0;
    //    }
    //    prop_spin.absValue = prop.absoluteValue;
    //    for (int i=0; i<8; i++) 
    //    {
    //        prop_spin.reserved[i] = 0;
    //    }
    //    return prop_spin;
    //}

    //static std::map<PixelFormat, spinPixelFormat> createPixelFormatMap_to_spin()
    //{
    //    std::map<PixelFormat, spinPixelFormat> map;
    //    map[PIXEL_FORMAT_MONO8]         =   SPIN_PIXEL_FORMAT_MONO8;
    //    map[PIXEL_FORMAT_411YUV8]       =   SPIN_PIXEL_FORMAT_411YUV8;
    //    map[PIXEL_FORMAT_422YUV8]       =   SPIN_PIXEL_FORMAT_422YUV8;
    //    map[PIXEL_FORMAT_444YUV8]       =   SPIN_PIXEL_FORMAT_444YUV8;
    //    map[PIXEL_FORMAT_RGB8]          =   SPIN_PIXEL_FORMAT_RGB8;
    //    map[PIXEL_FORMAT_MONO16]        =   SPIN_PIXEL_FORMAT_MONO16;
    //    map[PIXEL_FORMAT_RGB16]         =   SPIN_PIXEL_FORMAT_RGB16;
    //    map[PIXEL_FORMAT_S_MONO16]      =   SPIN_PIXEL_FORMAT_S_MONO16;
    //    map[PIXEL_FORMAT_S_RGB16]       =   SPIN_PIXEL_FORMAT_S_RGB16;
    //    map[PIXEL_FORMAT_RAW8]          =   SPIN_PIXEL_FORMAT_RAW8;
    //    map[PIXEL_FORMAT_RAW16]         =   SPIN_PIXEL_FORMAT_RAW16;
    //    map[PIXEL_FORMAT_MONO12]        =   SPIN_PIXEL_FORMAT_MONO12;
    //    map[PIXEL_FORMAT_RAW12]         =   SPIN_PIXEL_FORMAT_RAW12;
    //    map[PIXEL_FORMAT_BGR8]          =   SPIN_PIXEL_FORMAT_BGR;
    //    map[PIXEL_FORMAT_BGRU]          =   SPIN_PIXEL_FORMAT_BGRU;
    //    map[PIXEL_FORMAT_RGBU]          =   SPIN_PIXEL_FORMAT_RGBU;
    //    map[PIXEL_FORMAT_BGR16]         =   SPIN_PIXEL_FORMAT_BGR16;
	//    map[PIXEL_FORMAT_BGRU16]        =   SPIN_PIXEL_FORMAT_BGRU16;
    //    map[PIXEL_FORMAT_422YUV8_JPEG]  =   SPIN_PIXEL_FORMAT_422YUV8_JPEG;
    //    return map;
    //};

    //static std::map<PixelFormat, spinPixelFormat> pixelFormatMap_to_spin = 
    //    createPixelFormatMap_to_spin();

    //spinPixelFormat convertPixelFormat_to_spin(PixelFormat pixFormat)
    //{
    //    if (pixelFormatMap_to_spin.count(pixFormat) != 0)
    //    {
    //        return pixelFormatMap_to_spin[pixFormat];
    //    }
    //    else
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert pixel format to Spinnaker ";
    //        ssError << "imaging mode";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_PIXEL_FORMAT, ssError.str());
    //    }
    //}


    //spinFormat7ImageSettings convertFormat7Settings_to_spin(Format7Settings settings)
    //{
    //    spinFormat7ImageSettings settings_spin;
    //    settings_spin.mode = convertImageMode_to_spin(settings.mode);
    //    settings_spin.offsetX = settings.offsetX;
    //    settings_spin.offsetY = settings.offsetY;
    //    settings_spin.width = settings.width;
    //    settings_spin.height = settings.height;
    //    settings_spin.pixelFormat = convertPixelFormat_to_spin(settings.pixelFormat);
    //    return settings_spin;
    //}

    //// Conversion from Spinnaker types to BIAS types
    //// ------------------------------------------------------------------------
    // 
    //static std::map<spinPropertyType, PropertyType> createPropertyTypeMap_from_spin()
    //{
    //    std::map<spinPropertyType, PropertyType> map;
    //    map[SPIN_BRIGHTNESS]     =   PROPERTY_TYPE_BRIGHTNESS;
    //    map[SPIN_AUTO_EXPOSURE]  =   PROPERTY_TYPE_AUTO_EXPOSURE;
    //    map[SPIN_SHARPNESS]      =   PROPERTY_TYPE_SHARPNESS;
    //    map[SPIN_WHITE_BALANCE]  =   PROPERTY_TYPE_WHITE_BALANCE;
    //    map[SPIN_HUE]            =   PROPERTY_TYPE_HUE;
    //    map[SPIN_SATURATION]     =   PROPERTY_TYPE_SATURATION;
    //    map[SPIN_GAMMA]          =   PROPERTY_TYPE_GAMMA;
    //    map[SPIN_IRIS]           =   PROPERTY_TYPE_IRIS;
    //    map[SPIN_FOCUS]          =   PROPERTY_TYPE_FOCUS;
    //    map[SPIN_ZOOM]           =   PROPERTY_TYPE_ZOOM;
    //    map[SPIN_PAN]            =   PROPERTY_TYPE_PAN;
    //    map[SPIN_TILT]           =   PROPERTY_TYPE_TILT;
    //    map[SPIN_SHUTTER]        =   PROPERTY_TYPE_SHUTTER;
    //    map[SPIN_GAIN]           =   PROPERTY_TYPE_GAIN;
    //    map[SPIN_TRIGGER_MODE]   =   PROPERTY_TYPE_TRIGGER_MODE;
    //    map[SPIN_TRIGGER_DELAY]  =   PROPERTY_TYPE_TRIGGER_DELAY;
    //    map[SPIN_FRAME_RATE]     =   PROPERTY_TYPE_FRAME_RATE;
    //    map[SPIN_TEMPERATURE]    =   PROPERTY_TYPE_TEMPERATURE;
    //    return map;

    //}

    //static std::map<spinPropertyType, PropertyType> propertyTypeMap_from_spin = 
    //    createPropertyTypeMap_from_spin();

    //PropertyType convertPropertyType_from_spin(spinPropertyType propType_spin)
    //{
    //    if (propertyTypeMap_from_spin.count(propType_spin) != 0) 
    //    {
    //        return propertyTypeMap_from_spin[propType_spin];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert Spinnaker PropertyType";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_PROPERTY_TYPE, ssError.str());
    //    }
    //}

    //static std::map<spinVideoMode, VideoMode> createVideoModeMap_from_spin()
    //{
    //    std::map<spinVideoMode, VideoMode> map;
    //    map[SPIN_VIDEOMODE_160x120YUV444]     =   VIDEOMODE_160x120YUV444;
    //    map[SPIN_VIDEOMODE_320x240YUV422]     =   VIDEOMODE_320x240YUV422;
    //    map[SPIN_VIDEOMODE_640x480YUV411]     =   VIDEOMODE_640x480YUV411;
    //    map[SPIN_VIDEOMODE_640x480YUV422]     =   VIDEOMODE_640x480YUV422;
    //    map[SPIN_VIDEOMODE_640x480RGB]        =   VIDEOMODE_640x480RGB;
    //    map[SPIN_VIDEOMODE_640x480Y8]         =   VIDEOMODE_640x480Y8;
    //    map[SPIN_VIDEOMODE_640x480Y16]        =   VIDEOMODE_640x480Y16;
    //    map[SPIN_VIDEOMODE_800x600YUV422]     =   VIDEOMODE_800x600YUV422;
    //    map[SPIN_VIDEOMODE_800x600RGB]        =   VIDEOMODE_800x600RGB;
    //    map[SPIN_VIDEOMODE_800x600Y8]         =   VIDEOMODE_800x600Y8;
    //    map[SPIN_VIDEOMODE_800x600Y16]        =   VIDEOMODE_800x600Y16;
    //    map[SPIN_VIDEOMODE_1024x768YUV422]    =   VIDEOMODE_1024x768YUV422;
    //    map[SPIN_VIDEOMODE_1024x768RGB]       =   VIDEOMODE_1024x768RGB;
    //    map[SPIN_VIDEOMODE_1024x768Y8]        =   VIDEOMODE_1024x768Y8;
    //    map[SPIN_VIDEOMODE_1024x768Y16]       =   VIDEOMODE_1024x768Y16;
    //    map[SPIN_VIDEOMODE_1280x960YUV422]    =   VIDEOMODE_1280x960YUV422;
    //    map[SPIN_VIDEOMODE_1280x960RGB]       =   VIDEOMODE_1280x960RGB;
    //    map[SPIN_VIDEOMODE_1280x960Y8]        =   VIDEOMODE_1280x960Y8;
    //    map[SPIN_VIDEOMODE_1280x960Y16]       =   VIDEOMODE_1280x960Y16;
    //    map[SPIN_VIDEOMODE_1600x1200YUV422]   =   VIDEOMODE_1600x1200YUV422;
    //    map[SPIN_VIDEOMODE_1600x1200RGB]      =   VIDEOMODE_1600x1200RGB;
    //    map[SPIN_VIDEOMODE_1600x1200Y8]       =   VIDEOMODE_1600x1200Y8;
    //    map[SPIN_VIDEOMODE_1600x1200Y16]      =   VIDEOMODE_1600x1200Y16;
    //    map[SPIN_VIDEOMODE_FORMAT7]           =   VIDEOMODE_FORMAT7;
    //    return map;
    //}

    //static std::map<spinVideoMode, VideoMode> videoModeMap_from_spin = 
    //    createVideoModeMap_from_spin();

    //VideoMode convertVideoMode_from_spin(spinVideoMode vidMode_spin)
    //{
    //    if (videoModeMap_from_spin.count(vidMode_spin) != 0)
    //    {
    //        return videoModeMap_from_spin[vidMode_spin];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert Spinnaker VideoMode";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_VIDEOMODE, ssError.str());
    //    }
    //}

    //static std::map<spinFrameRate, FrameRate> createFrameRateMap_from_spin()
    //{
    //    std::map<spinFrameRate, FrameRate> map;
    //    map[SPIN_FRAMERATE_1_875]    =   FRAMERATE_1_875;
    //    map[SPIN_FRAMERATE_3_75]     =   FRAMERATE_3_75;
    //    map[SPIN_FRAMERATE_7_5]      =   FRAMERATE_7_5;
    //    map[SPIN_FRAMERATE_15]       =   FRAMERATE_15;
    //    map[SPIN_FRAMERATE_30]       =   FRAMERATE_30;
    //    map[SPIN_FRAMERATE_60]       =   FRAMERATE_60;
    //    map[SPIN_FRAMERATE_120]      =   FRAMERATE_120;
    //    map[SPIN_FRAMERATE_240]      =   FRAMERATE_240;
    //    map[SPIN_FRAMERATE_FORMAT7]  =   FRAMERATE_FORMAT7;
    //    return map;
    //}

    //static std::map<spinFrameRate, FrameRate> frameRateMap_from_spin = 
    //    createFrameRateMap_from_spin();

    //FrameRate convertFrameRate_from_spin(spinFrameRate frmRate_spin)
    //{
    //    if (frameRateMap_from_spin.count(frmRate_spin) != 0)
    //    {
    //        return frameRateMap_from_spin[frmRate_spin];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert Spinnaker FrameRate";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_VIDEOMODE, ssError.str());
    //    }
    //}

    //static std::map<spinMode, ImageMode> createImageModeMap_from_spin()
    //{
    //    std::map<spinMode, ImageMode> map;
    //    map[SPIN_MODE_0]   =   IMAGEMODE_0;
    //    map[SPIN_MODE_1]   =   IMAGEMODE_1;
    //    map[SPIN_MODE_2]   =   IMAGEMODE_2;
    //    map[SPIN_MODE_3]   =   IMAGEMODE_3;
    //    map[SPIN_MODE_4]   =   IMAGEMODE_4;
    //    map[SPIN_MODE_5]   =   IMAGEMODE_5;
    //    map[SPIN_MODE_6]   =   IMAGEMODE_6;
    //    map[SPIN_MODE_7]   =   IMAGEMODE_7;
    //    map[SPIN_MODE_8]   =   IMAGEMODE_8;
    //    map[SPIN_MODE_9]   =   IMAGEMODE_9;
    //    map[SPIN_MODE_10]  =   IMAGEMODE_10;
    //    map[SPIN_MODE_11]  =   IMAGEMODE_11;
    //    map[SPIN_MODE_12]  =   IMAGEMODE_12;
    //    map[SPIN_MODE_13]  =   IMAGEMODE_13;
    //    map[SPIN_MODE_14]  =   IMAGEMODE_14;
    //    map[SPIN_MODE_15]  =   IMAGEMODE_15;
    //    map[SPIN_MODE_16]  =   IMAGEMODE_16;
    //    map[SPIN_MODE_17]  =   IMAGEMODE_17;
    //    map[SPIN_MODE_18]  =   IMAGEMODE_18;
    //    map[SPIN_MODE_19]  =   IMAGEMODE_19;
    //    map[SPIN_MODE_20]  =   IMAGEMODE_20;
    //    map[SPIN_MODE_21]  =   IMAGEMODE_21;
    //    map[SPIN_MODE_22]  =   IMAGEMODE_22;
    //    map[SPIN_MODE_23]  =   IMAGEMODE_23;
    //    map[SPIN_MODE_24]  =   IMAGEMODE_24;
    //    map[SPIN_MODE_25]  =   IMAGEMODE_25;
    //    map[SPIN_MODE_26]  =   IMAGEMODE_26;
    //    map[SPIN_MODE_27]  =   IMAGEMODE_27;
    //    map[SPIN_MODE_28]  =   IMAGEMODE_28;
    //    map[SPIN_MODE_29]  =   IMAGEMODE_29;
    //    map[SPIN_MODE_30]  =   IMAGEMODE_30;
    //    map[SPIN_MODE_31]  =   IMAGEMODE_31;
    //    return map;
    //}

    //static std::map<spinMode, ImageMode> imageModeMap_from_spin = 
    //    createImageModeMap_from_spin();

    //ImageMode convertImageMode_from_spin(spinMode imgMode_spin)
    //{
    //    if (imageModeMap_from_spin.count(imgMode_spin) != 0)
    //    {
    //        return imageModeMap_from_spin[imgMode_spin];
    //    }
    //    else
    //    { 
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert Spinnaker Mode";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_VIDEOMODE, ssError.str());
    //    }
    //}

    //Property convertProperty_from_spin(spinProperty prop_spin)
    //{
    //    Property prop;
    //    prop.type = convertPropertyType_from_spin(prop_spin.type);
    //    prop.present = bool(prop_spin.present);
    //    prop.absoluteControl = bool(prop_spin.absControl);
    //    prop.onePush = bool(prop_spin.onePush);
    //    prop.on = bool(prop_spin.onOff);
    //    prop.autoActive = bool(prop_spin.autoManualMode);
    //    if (prop.type == PROPERTY_TYPE_WHITE_BALANCE)  
    //    {
    //        prop.valueA = prop_spin.valueA;
    //        prop.valueB = prop_spin.valueB;
    //        prop.value = 0;
    //    }
    //    else
    //    {
    //        prop.value = prop_spin.valueA;
    //        prop.valueA = 0;
    //        prop.valueB = 0;
    //    }
    //    prop.absoluteValue = prop_spin.absValue;
    //    return prop;
    //}
    //
    //PropertyInfo convertPropertyInfo_from_spin(spinPropertyInfo propInfo_spin) 
    //{
    //    PropertyInfo propInfo;
    //    propInfo.type = convertPropertyType_from_spin(propInfo_spin.type);
    //    propInfo.present = bool(propInfo_spin.present);
    //    propInfo.autoCapable = bool(propInfo_spin.autoSupported);
    //    propInfo.manualCapable = bool(propInfo_spin.manualSupported);
    //    propInfo.absoluteCapable = bool(propInfo_spin.absValSupported);
    //    propInfo.onePushCapable = bool(propInfo_spin.onePushSupported);
    //    propInfo.onOffCapable = bool(propInfo_spin.onOffSupported);
    //    propInfo.minValue = propInfo_spin.min;
    //    propInfo.maxValue = propInfo_spin.max;
    //    propInfo.minAbsoluteValue = propInfo_spin.absMin;
    //    propInfo.maxAbsoluteValue = propInfo_spin.absMax;
    //    propInfo.units = std::string(propInfo_spin.pUnits);
    //    propInfo.unitsAbbr = std::string(propInfo_spin.pUnitAbbr);
    //    propInfo.haveUnits =  bool(propInfo.units.length());
    //    return propInfo;
    //}

    //static std::map<spinPixelFormat, PixelFormat> createPixelFormatMap_from_spin()
    //{
    //    std::map<spinPixelFormat, PixelFormat> map;
    //    map[SPIN_PIXEL_FORMAT_MONO8]         =   PIXEL_FORMAT_MONO8;
    //    map[SPIN_PIXEL_FORMAT_411YUV8]       =   PIXEL_FORMAT_411YUV8;
    //    map[SPIN_PIXEL_FORMAT_422YUV8]       =   PIXEL_FORMAT_422YUV8;
    //    map[SPIN_PIXEL_FORMAT_444YUV8]       =   PIXEL_FORMAT_444YUV8;
    //    map[SPIN_PIXEL_FORMAT_RGB8]          =   PIXEL_FORMAT_RGB8;
    //    map[SPIN_PIXEL_FORMAT_MONO16]        =   PIXEL_FORMAT_MONO16;
    //    map[SPIN_PIXEL_FORMAT_RGB16]         =   PIXEL_FORMAT_RGB16;
    //    map[SPIN_PIXEL_FORMAT_S_MONO16]      =   PIXEL_FORMAT_S_MONO16;
    //    map[SPIN_PIXEL_FORMAT_S_RGB16]       =   PIXEL_FORMAT_S_RGB16;
    //    map[SPIN_PIXEL_FORMAT_RAW8]          =   PIXEL_FORMAT_RAW8;
    //    map[SPIN_PIXEL_FORMAT_RAW16]         =   PIXEL_FORMAT_RAW16;
    //    map[SPIN_PIXEL_FORMAT_MONO12]        =   PIXEL_FORMAT_MONO12;
    //    map[SPIN_PIXEL_FORMAT_RAW12]         =   PIXEL_FORMAT_RAW12;
    //    map[SPIN_PIXEL_FORMAT_BGR]           =   PIXEL_FORMAT_BGR8;
    //    map[SPIN_PIXEL_FORMAT_BGRU]          =   PIXEL_FORMAT_BGRU;
    //    map[SPIN_PIXEL_FORMAT_RGBU]          =   PIXEL_FORMAT_RGBU;
    //    map[SPIN_PIXEL_FORMAT_BGR16]         =   PIXEL_FORMAT_BGR16;
	//    map[SPIN_PIXEL_FORMAT_BGRU16]        =   PIXEL_FORMAT_BGRU16;
    //    map[SPIN_PIXEL_FORMAT_422YUV8_JPEG]  =   PIXEL_FORMAT_422YUV8_JPEG;
    //    return map;
    //};

    //static std::map<spinPixelFormat, PixelFormat> pixelFormatMap_from_spin = 
    //    createPixelFormatMap_from_spin();

    //PixelFormat convertPixelFormat_from_spin(spinPixelFormat pixFormat_spin)
    //{
    //    if (pixelFormatMap_from_spin.count(pixFormat_spin) != 0)
    //    {
    //        return pixelFormatMap_from_spin[pixFormat_spin];
    //    }
    //    else
    //    {
    //        std::stringstream ssError;
    //        ssError << __PRETTY_FUNCTION__;
    //        ssError << ": unable to convert pixel format from FlyCaptuer2";
    //        throw RuntimeError(ERROR_SPIN_CONVERT_PROPERTY_TYPE, ssError.str());
    //    }
    //}


    //Format7Settings convertFormat7Settings_from_spin(spinFormat7ImageSettings settings_spin)
    //{
    //    Format7Settings settings;
    //    settings.mode = convertImageMode_from_spin(settings_spin.mode);
    //    settings.offsetX = settings_spin.offsetX;
    //    settings.offsetY = settings_spin.offsetY;
    //    settings.width = settings_spin.width;
    //    settings.height = settings_spin.height;
    //    settings.pixelFormat = convertPixelFormat_from_spin(settings_spin.pixelFormat);
    //    return settings;
    //}

    //// Functions for printing information
    //// --------------------------------------------------------------
    //void printImageInfo_spin(spinImage &image)
    //{
    //    std::cout << std::endl;
    //    std::cout << "----------------------" << std::endl;
    //    std::cout << "Spinnaker Image Info" << std::endl;
    //    std::cout << "----------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " rows:             " << image.rows << std::endl;
    //    std::cout << " cols:             " << image.cols << std::endl;
    //    std::cout << " stride:           " << image.stride << std::endl;
    //    std::cout << " dataSize:         " << image.dataSize << std::endl;
    //    std::cout << " receivedDataSize: " << image.receivedDataSize << std::endl;
    //    std::cout << " format            " << std::endl;
    //    std::cout << "   Code:           " << "0x" << std::hex << image.format << std::dec << std::endl;
    //    std::cout << "   String:         " << getPixelFormatString_spin(image.format) << std::endl;
    //    std::cout << " bayerFormat       " << std::endl;
    //    std::cout << "   code:           " << "0x" << std::hex << image.bayerFormat << std::dec << std::endl;
    //    std::cout << "   string:         " << getBayerTileFormatString_spin(image.bayerFormat) << std::endl;
    //    std::cout << std::endl;
    //}

    //void printFormat7ImageSettings_spin(spinFormat7ImageSettings &imageSettings)
    //{
    //    std::cout << std::endl;
    //    std::cout << "------------------------------------" << std::endl;
    //    std::cout << "FlyCatpure2 Format 7 Image settings " << std::endl;
    //    std::cout << "------------------------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " mode:         " << getModeString_spin(imageSettings.mode) << std::endl;
    //    std::cout << " offsetX:      " << imageSettings.offsetX << std::endl;
    //    std::cout << " offsetY:      " << imageSettings.offsetY << std::endl;
    //    std::cout << " width:        " << imageSettings.width << std::endl; 
    //    std::cout << " height:       " << imageSettings.height << std::endl;
    //    std::cout << " pixelFormat:  " << getPixelFormatString_spin(imageSettings.pixelFormat) << std::endl;
    //    std::cout << " reserved:     " << std::endl;
    //    for (int i=0; i<8; i++) 
    //    {
    //        std::cout << "   [" << i << "] = " << std::hex << "0x" << imageSettings.reserved[i];
    //        std::cout << std::dec << std::endl;
    //    }
    //    std::cout << std::endl;
    //}

    //void printFormat7PacketSettings_spin(unsigned int packetSize, float percentage)
    //{
    //    std::cout << std::endl;
    //    std::cout << "------------------------------------" << std::endl;
    //    std::cout << "Spinnaker Format 7 Packet settings" << std::endl;
    //    std::cout << "------------------------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " percentage:   " << percentage << std::endl;
    //    std::cout << " packetSize:   " << packetSize << std::endl;
    //    std::cout << std::endl;
    //}

    //void printFormat7Configuration_spin(
    //        spinFormat7ImageSettings &imageSettings,
    //        unsigned int packetSize,
    //        float percentage 
    //        )
    //{
    //    printFormat7ImageSettings_spin(imageSettings);
    //    printFormat7PacketSettings_spin(packetSize, percentage);
    //}

    //void printFormat7Info_spin(spinFormat7Info &format7Info)
    //{
    //    std::cout << std::endl;
    //    std::cout << "-------------------------" << std::endl;
    //    std::cout << "Spinnaker Format 7 Info" << std::endl;
    //    std::cout << "-------------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " mode:                      " << getModeString_spin(format7Info.mode) << std::endl;
    //    std::cout << " maxWidth:                  " << format7Info.maxWidth << std::endl;
    //    std::cout << " maxHeight:                 " << format7Info.maxHeight << std::endl;
    //    std::cout << " offsetHStepSize:           " << format7Info.offsetHStepSize << std::endl;
    //    std::cout << " offsetVStepSize:           " << format7Info.offsetVStepSize << std::endl;
    //    std::cout << " imageHStepSize:            " << format7Info.imageHStepSize << std::endl;
    //    std::cout << " imageVStepSize:            " << format7Info.imageVStepSize << std:: endl;
    //    std::cout << " pixelFormatBitField:       " << format7Info.pixelFormatBitField << std::endl;
    //    std::cout << " pixelFormatBitField:       " << std::bitset<32>(format7Info.pixelFormatBitField) << std::endl;
    //    std::cout << " vendorPixelFormatBitField: " << format7Info.vendorPixelFormatBitField << std::endl;
    //    std::cout << " vendorPixelFormatBitField: " << std::bitset<32>(format7Info.vendorPixelFormatBitField) << std::endl;
    //    std::cout << " packetSize:                " << format7Info.packetSize << std::endl;
    //    std::cout << " minPacketSize:             " << format7Info.minPacketSize << std::endl;
    //    std::cout << " maxPacketSize:             " << format7Info.maxPacketSize << std::endl;
    //    std::cout << std::endl;
    //}

    //void printFormat7PacketInfo_spin(spinFormat7PacketInfo &packetInfo)
    //{
    //    std::cout << std::endl;
    //    std::cout << "--------------------------------" << std::endl;
    //    std::cout << "Spinnaker Format 7 Packet Info" << std::endl;
    //    std::cout << "--------------------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " recommendedBytesPerPacket:  " << packetInfo.recommendedBytesPerPacket << std::endl;
    //    std::cout << " maxBytesPerPacket:          " << packetInfo.maxBytesPerPacket << std::endl;
    //    std::cout << " unitBytesPerPacket:         " << packetInfo.unitBytesPerPacket << std::endl;
    //    std::cout << " reseverd                    " << std::endl;
    //    for (int i=0; i<8; i++) 
    //    {
    //        std::cout << "   [" << i << "] = " << packetInfo.reserved[i] << std::endl;
    //    }
    //    std::cout << std::endl;
    //}

    //void printPropertyInfo_spin(spinPropertyInfo &propInfo)
    //{
    //    std::cout << std::endl;
    //    std::cout << "-------------------------" << std::endl;
    //    std::cout << "Spinnaker Property Info" << std::endl;
    //    std::cout << "-------------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " type:                " << getPropertyTypeString_spin(propInfo.type) << std::endl;
    //    std::cout << std::boolalpha;
    //    std::cout << " present:             " << bool(propInfo.present) << std::endl;
    //    std::cout << " autoSupported:       " << bool(propInfo.autoSupported) << std::endl;
    //    std::cout << " manualSupported:     " << bool(propInfo.manualSupported) << std::endl;
    //    std::cout << " onOffSupported:      " << bool(propInfo.onOffSupported) << std::endl;
    //    std::cout << " onePushSupported:    " << bool(propInfo.onePushSupported) << std::endl;
    //    std::cout << " absValSupported:     " << bool(propInfo.absValSupported) << std::endl;
    //    std::cout << " readOutSupported:    " << bool(propInfo.absValSupported) << std::endl;
    //    std::cout << std::noboolalpha;
    //    std::cout << " min (unsigned int):  " << propInfo.min << std::endl;
    //    std::cout << " max (unsigned int):  " << propInfo.max << std::endl;
    //    std::cout << " absMin:              " << propInfo.absMin << std::endl;
    //    std::cout << " absMax:              " << propInfo.absMax << std::endl;
    //    std::cout << " pUnits:              " << propInfo.pUnits << std::endl;
    //    std::cout << " pUnitAbbr:           " << propInfo.pUnitAbbr << std::endl;
    //    std::cout << " reserved             " << std::endl;
    //    for (int i=0; i<8; i++)
    //    {
    //        std::cout << "   [" << i << "] = " << propInfo.reserved[i] << std::endl; 
    //    }
    //    std::cout << std::endl; 
    //}

    //void printProperty_spin(spinProperty &prop)
    //{
    //    std::cout << std::endl;
    //    std::cout << "--------------------" << std::endl;
    //    std::cout << "Spinnaker Property" << std::endl;
    //    std::cout << "--------------------" << std::endl;
    //    std::cout << std::endl;
    //    std::cout << " type:            " << getPropertyTypeString_spin(prop.type) << std::endl;
    //    std::cout << std::boolalpha << std::endl;
    //    std::cout << " present:         " << bool(prop.present) << std::endl; 
    //    std::cout << " absControl:      " << bool(prop.absControl) << std::endl;
    //    std::cout << " onePush:         " << bool(prop.onePush) << std::endl;
    //    std::cout << " onOff:           " << bool(prop.onOff) << std::endl;
    //    std::cout << " autoManualMode:  " << bool(prop.autoManualMode) << std::endl;
    //    std::cout << std::noboolalpha;
    //    std::cout << " valueA:          " << prop.valueA << std::endl;
    //    std::cout << " valueB:          " << prop.valueB << std::endl;
    //    std::cout << " absValue:        " << prop.absValue << std::endl;
    //    std::cout << " reserved         " << std::endl;
    //    for (int i=0; i<8; i++)
    //    {
    //        std::cout << "   [" << i << "] = " << prop.reserved[i] << std::endl; 
    //    }
    //    std::cout << std::endl; 
    //}

    //void printTriggerMode_spin(spinTriggerMode &trigMode)
    //{
    //    std::cout << std::endl;
    //    std::cout << "-----------------------" << std::endl;
    //    std::cout << "Spinnaker TriggerMode" << std::endl;
    //    std::cout << "-----------------------" << std::endl;
    //    std::cout << std::endl << std::boolalpha;
    //    std::cout << " onOff:     " << bool(trigMode.onOff) << std::endl;
    //    std::cout << std::noboolalpha;
    //    std::cout << " polarity:  " << trigMode.polarity << std::endl;
    //    std::cout << " source:    " << trigMode.source << std::endl;
    //    std::cout << " mode:      " << trigMode.mode << std::endl;
    //    std::cout << " parameter: " << trigMode.parameter << std::endl;
    //    std::cout << " reserved   " << std::endl;
    //    for (int i=0; i<8; i++)
    //    {
    //        std::cout << "   [" << i << "] = " << trigMode.reserved[i] << std::endl; 
    //    }
    //    std::cout << std::endl; 
    //}

    //void printTriggerModeInfo_spin(spinTriggerModeInfo &trigModeInfo)
    //{
    //    std::cout << std::endl;
    //    std::cout << "---------------------------" << std::endl;
    //    std::cout << "Spinnaker TriggerModeInfo" << std::endl;
    //    std::cout << "---------------------------" << std::endl;
    //    std::cout << std::endl << std::boolalpha;
    //    std::cout << " present:                  " << bool(trigModeInfo.present) << std::endl;
    //    std::cout << " readOutSupported:         " << bool(trigModeInfo.readOutSupported) << std::endl;
    //    std::cout << " onOffSupported:           " << bool(trigModeInfo.onOffSupported) << std::endl;
    //    std::cout << " polaritySupported:        " << bool(trigModeInfo.polaritySupported) << std::endl;
    //    std::cout << " valueReadable:            " << bool(trigModeInfo.valueReadable) << std::endl;
    //    std::cout << " sourceMask:               " << std::bitset<32>(trigModeInfo.sourceMask) << std::endl;
    //    std::cout << " softwareTriggerSupported: " << bool(trigModeInfo.softwareTriggerSupported) << std::endl;
    //    std::cout << " modeMask:                 " << std::bitset<32>(trigModeInfo.modeMask) << std::endl;
    //    std::cout << " reserved  " << std::endl;
    //    for (int i=0; i<8; i++)
    //    {
    //        std::cout << "   [" << i << "] = " << trigModeInfo.reserved[i] << std::endl; 
    //    }
    //    std::cout << std::noboolalpha << std::endl;
    //}


    //void printConfiguration_spin(spinConfig &config)
    //{
    //    std::cout << std::endl;
    //    std::cout << "---------------------------" << std::endl;
    //    std::cout << " Spinnaker Configuration " << std::endl;
    //    std::cout << "---------------------------" << std::endl;
    //    std::cout << std::endl << std::boolalpha;
    //    std::cout << " numBuffers:               " << config.numBuffers << std::endl;
    //    std::cout << " numImageNotifications:    " << config.numImageNotifications << std::endl;
    //    std::cout << " minNumImageNotifications: " << config.minNumImageNotifications << std::endl;
    //    std::cout << " grabTimeout:              " << getGrabTimeoutString_spin(config.grabTimeout) << std::endl;
    //    std::cout << " grabMode:                 " << getGrabModeString_spin(config.grabMode) << std::endl;
    //    std::cout << " isochBusSpeed:            " << getBusSpeedString_spin(config.isochBusSpeed) << std::endl;
    //    std::cout << " asyncBusSpeed:            " << getBusSpeedString_spin(config.asyncBusSpeed) << std::endl;
    //    std::cout << " bandwidthAllocation:      " << getBandwidthAllocationString_spin(config.bandwidthAllocation) << std::endl;
    //    std::cout << " registerTimeoutRetries:   " << config.registerTimeoutRetries << std::endl;
    //    std::cout << " registerTimeout:          " << config.registerTimeout << std::endl;
    //    std::cout << " reserved                  " << std::endl;
    //    for (int i=0; i<8; i++)
    //    {
    //        std::cout << "   [" << i << "] = " << config.reserved[i] << std::endl; 
    //    }
    //    std::cout << std::noboolalpha << std::endl;
    //}



    //// Functions for converting Spinnaker enumerations to strings   
    //// Mostly autogenerated. 
    //// ------------------------------------------------------------------------

    //static std::map<spinInterfaceType, std::string> createInterfaceTypeToStringMap_spin()
    //{
    //    std::map<spinInterfaceType, std::string> map;
    //    map[SPIN_INTERFACE_IEEE1394]  =   std::string("SPIN_INTERFACE_IEEE1394");
    //    map[SPIN_INTERFACE_USB_2]     =   std::string("SPIN_INTERFACE_USB_2");
    //    map[SPIN_INTERFACE_USB_3]     =   std::string("SPIN_INTERFACE_USB_3");
    //    map[SPIN_INTERFACE_GIGE]      =   std::string("SPIN_INTERFACE_GIGE");
    //    map[SPIN_INTERFACE_UNKNOWN]   =   std::string("SPIN_INTERFACE_UNKNOWN");
    //    return map;
    //}

    //static std::map<spinInterfaceType, std::string> interfaceTypeToStringMap_spin = 
    //    createInterfaceTypeToStringMap_spin();

    //std::string getInterfaceTypeString_spin(spinInterfaceType ifaceType)
    //{
    //    if (interfaceTypeToStringMap_spin.count(ifaceType) != 0)
    //    {
    //        return interfaceTypeToStringMap_spin[ifaceType];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin interface value");
    //    }
    //}

    static std::map<spinPixelFormatEnums, std::string> pixelFormatToStringMap_spin = {
        {PixelFormat_Mono1p, std::string("Monochrome 1-bit packed")},
        {PixelFormat_Mono2p, std::string("Monochrome 2-bit packed")},
        {PixelFormat_Mono4p, std::string("Monochrome 4-bit packed")},
        {PixelFormat_Mono8, std::string("Monochrome 8-bit")},
        {PixelFormat_Mono8s, std::string("Monochrome 8-bit signed")},
        {PixelFormat_Mono10, std::string("Monochrome 10-bit unpacked")},
        {PixelFormat_Mono10p, std::string("Monochrome 10-bit packed")},
        {PixelFormat_Mono12, std::string("Monochrome 12-bit unpacked")},
        {PixelFormat_Mono12p, std::string("Monochrome 12-bit packed")},
        {PixelFormat_Mono14, std::string("Monochrome 14-bit unpacked")},
        {PixelFormat_Mono16, std::string("Monochrome 16-bit")},
        {PixelFormat_BayerBG8, std::string("Bayer Blue-Green 8-bit")},
        {PixelFormat_BayerBG10, std::string("Bayer Blue-Green 10-bit unpacked")},
        {PixelFormat_BayerBG10p, std::string("Bayer Blue-Green 10-bit packed")},
        {PixelFormat_BayerBG12, std::string("Bayer Blue-Green 12-bit unpacked")},
        {PixelFormat_BayerBG12p, std::string("Bayer Blue-Green 12-bit packed")},
        {PixelFormat_BayerBG16, std::string("Bayer Blue-Green 16-bit")},
        {PixelFormat_BayerGB8, std::string("Bayer Green-Blue 8-bit")},
        {PixelFormat_BayerGB10, std::string("Bayer Green-Blue 10-bit unpacked")},
        {PixelFormat_BayerGB10p, std::string("Bayer Green-Blue 10-bit packed")},
        {PixelFormat_BayerGB12, std::string("Bayer Green-Blue 12-bit unpacked")},
        {PixelFormat_BayerGB12p, std::string("Bayer Green-Blue 12-bit packed")},
        {PixelFormat_BayerGB16, std::string("Bayer Green-Blue 16-bit")},
        {PixelFormat_BayerGR8, std::string("Bayer Green-Red 8-bit")},
        {PixelFormat_BayerGR10, std::string("Bayer Green-Red 10-bit unpacked")},
        {PixelFormat_BayerGR10p, std::string("Bayer Green-Red 10-bit packed")},
        {PixelFormat_BayerGR12, std::string("Bayer Green-Red 12-bit unpacked")},
        {PixelFormat_BayerGR12p, std::string("Bayer Green-Red 12-bit packed")},
        {PixelFormat_BayerGR16, std::string("Bayer Green-Red 16-bit")},
        {PixelFormat_BayerRG8, std::string("Bayer Red-Green 8-bit")},
        {PixelFormat_BayerRG10, std::string("Bayer Red-Green 10-bit unpacked")},
        {PixelFormat_BayerRG10p, std::string("Bayer Red-Green 10-bit packed")},
        {PixelFormat_BayerRG12, std::string("Bayer Red-Green 12-bit unpacked")},
        {PixelFormat_BayerRG12p, std::string("Bayer Red-Green 12-bit packed")},
        {PixelFormat_BayerRG16, std::string("Bayer Red-Green 16-bit")},
        {PixelFormat_RGBa8, std::string("Red-Green-Blue-alpha 8-bit")},
        {PixelFormat_RGBa10, std::string("Red-Green-Blue-alpha 10-bit unpacked")},
        {PixelFormat_RGBa10p, std::string("Red-Green-Blue-alpha 10-bit packed")},
        {PixelFormat_RGBa12, std::string("Red-Green-Blue-alpha 12-bit unpacked")},
        {PixelFormat_RGBa12p, std::string("Red-Green-Blue-alpha 12-bit packed")},
        {PixelFormat_RGBa14, std::string("Red-Green-Blue-alpha 14-bit unpacked")},
        {PixelFormat_RGBa16, std::string("Red-Green-Blue-alpha 16-bit")},
        {PixelFormat_RGB8, std::string("Red-Green-Blue 8-bit")},
        {PixelFormat_RGB8_Planar, std::string("Red-Green-Blue 8-bit planar")},
        {PixelFormat_RGB10, std::string("Red-Green-Blue 10-bit unpacked")},
        {PixelFormat_RGB10_Planar, std::string("Red-Green-Blue 10-bit unpacked planar")},
        {PixelFormat_RGB10p, std::string("Red-Green-Blue 10-bit packed")},
        {PixelFormat_RGB10p32, std::string("Red-Green-Blue 10-bit packed into 32-bit")},
        {PixelFormat_RGB12, std::string("Red-Green-Blue 12-bit unpacked")},
        {PixelFormat_RGB12_Planar, std::string("Red-Green-Blue 12-bit unpacked planar")},
        {PixelFormat_RGB12p, std::string("Red-Green-Blue 12-bit packed  ")},
        {PixelFormat_RGB14, std::string("Red-Green-Blue 14-bit unpacked")},
        {PixelFormat_RGB16, std::string("Red-Green-Blue 16-bit  ")},
        {PixelFormat_RGB16_Planar, std::string("Red-Green-Blue 16-bit planar")},
        {PixelFormat_RGB565p, std::string("Red-Green-Blue 5/6/5-bit packed")},
        {PixelFormat_BGRa8, std::string("Blue-Green-Red-alpha 8-bit")},
        {PixelFormat_BGRa10, std::string("Blue-Green-Red-alpha 10-bit unpacked")},
        {PixelFormat_BGRa10p, std::string("Blue-Green-Red-alpha 10-bit packed")},
        {PixelFormat_BGRa12, std::string("Blue-Green-Red-alpha 12-bit unpacked")},
        {PixelFormat_BGRa12p, std::string("Blue-Green-Red-alpha 12-bit packed")},
        {PixelFormat_BGRa14, std::string("Blue-Green-Red-alpha 14-bit unpacked")},
        {PixelFormat_BGRa16, std::string("Blue-Green-Red-alpha 16-bit")},
        {PixelFormat_BGR8, std::string("Blue-Green-Red 8-bit")},
        {PixelFormat_BGR10, std::string("Blue-Green-Red 10-bit unpacked")},
        {PixelFormat_BGR10p, std::string("Blue-Green-Red 10-bit packed")},
        {PixelFormat_BGR12, std::string("Blue-Green-Red 12-bit unpacked")},
        {PixelFormat_BGR12p, std::string("Blue-Green-Red 12-bit packed")},
        {PixelFormat_BGR14, std::string("Blue-Green-Red 14-bit unpacked")},
        {PixelFormat_BGR16, std::string("Blue-Green-Red 16-bit")},
        {PixelFormat_BGR565p, std::string("Blue-Green-Red 5/6/5-bit packed")},
        {PixelFormat_R8, std::string("Red 8-bit")},
        {PixelFormat_R10, std::string("Red 10-bit")},
        {PixelFormat_R12, std::string("Red 12-bit")},
        {PixelFormat_R16, std::string("Red 16-bit")},
        {PixelFormat_G8, std::string("Green 8-bit")},
        {PixelFormat_G10, std::string("Green 10-bit")},
        {PixelFormat_G12, std::string("Green 12-bit")},
        {PixelFormat_G16, std::string("Green 16-bit")},
        {PixelFormat_B8, std::string("Blue 8-bit")},
        {PixelFormat_B10, std::string("Blue 10-bit")},
        {PixelFormat_B12, std::string("Blue 12-bit")},
        {PixelFormat_B16, std::string("Blue 16-bit")},
        {PixelFormat_Coord3D_ABC8, std::string("3D coordinate A-B-C 8-bit")},
        {PixelFormat_Coord3D_ABC8_Planar, std::string("3D coordinate A-B-C 8-bit planar")},
        {PixelFormat_Coord3D_ABC10p, std::string("3D coordinate A-B-C 10-bit packed")},
        {PixelFormat_Coord3D_ABC10p_Planar, std::string("3D coordinate A-B-C 10-bit packed planar")},
        {PixelFormat_Coord3D_ABC12p, std::string("3D coordinate A-B-C 12-bit packed")},
        {PixelFormat_Coord3D_ABC12p_Planar, std::string("3D coordinate A-B-C 12-bit packed planar")},
        {PixelFormat_Coord3D_ABC16, std::string("3D coordinate A-B-C 16-bit")},
        {PixelFormat_Coord3D_ABC16_Planar, std::string("3D coordinate A-B-C 16-bit planar")},
        {PixelFormat_Coord3D_ABC32f, std::string("3D coordinate A-B-C 32-bit floating point")},
        {PixelFormat_Coord3D_ABC32f_Planar, std::string("3D coordinate A-B-C 32-bit floating point planar")},
        {PixelFormat_Coord3D_AC8, std::string("3D coordinate A-C 8-bit")},
        {PixelFormat_Coord3D_AC8_Planar, std::string("3D coordinate A-C 8-bit planar")},
        {PixelFormat_Coord3D_AC10p, std::string("3D coordinate A-C 10-bit packed")},
        {PixelFormat_Coord3D_AC10p_Planar, std::string("3D coordinate A-C 10-bit packed planar")},
        {PixelFormat_Coord3D_AC12p, std::string("3D coordinate A-C 12-bit packed")},
        {PixelFormat_Coord3D_AC12p_Planar, std::string("3D coordinate A-C 12-bit packed planar")},
        {PixelFormat_Coord3D_AC16, std::string("3D coordinate A-C 16-bit")},
        {PixelFormat_Coord3D_AC16_Planar, std::string("3D coordinate A-C 16-bit planar")},
        {PixelFormat_Coord3D_AC32f, std::string("3D coordinate A-C 32-bit floating point")},
        {PixelFormat_Coord3D_AC32f_Planar, std::string("3D coordinate A-C 32-bit floating point planar")},
        {PixelFormat_Coord3D_A8, std::string("3D coordinate A 8-bit")},
        {PixelFormat_Coord3D_A10p, std::string("3D coordinate A 10-bit packed")},
        {PixelFormat_Coord3D_A12p, std::string("3D coordinate A 12-bit packed")},
        {PixelFormat_Coord3D_A16, std::string("3D coordinate A 16-bit")},
        {PixelFormat_Coord3D_A32f, std::string("3D coordinate A 32-bit floating point")},
        {PixelFormat_Coord3D_B8, std::string("3D coordinate B 8-bit")},
        {PixelFormat_Coord3D_B10p, std::string("3D coordinate B 10-bit packed")},
        {PixelFormat_Coord3D_B12p, std::string("3D coordinate B 12-bit packed")},
        {PixelFormat_Coord3D_B16, std::string("3D coordinate B 16-bit")},
        {PixelFormat_Coord3D_B32f, std::string("3D coordinate B 32-bit floating point")},
        {PixelFormat_Coord3D_C8, std::string("3D coordinate C 8-bit")},
        {PixelFormat_Coord3D_C10p, std::string("3D coordinate C 10-bit packed")},
        {PixelFormat_Coord3D_C12p, std::string("3D coordinate C 12-bit packed")},
        {PixelFormat_Coord3D_C16, std::string("3D coordinate C 16-bit")},
        {PixelFormat_Coord3D_C32f, std::string("3D coordinate C 32-bit floating point")},
        {PixelFormat_Confidence1, std::string("Confidence 1-bit unpacked")},
        {PixelFormat_Confidence1p, std::string("Confidence 1-bit packed")},
        {PixelFormat_Confidence8, std::string("Confidence 8-bit")},
        {PixelFormat_Confidence16, std::string("Confidence 16-bit")},
        {PixelFormat_Confidence32f, std::string("Confidence 32-bit floating point")},
        {PixelFormat_BiColorBGRG8, std::string("Bi-color Blue/Green - Red/Green 8-bit")},
        {PixelFormat_BiColorBGRG10, std::string("Bi-color Blue/Green - Red/Green 10-bit unpacked")},
        {PixelFormat_BiColorBGRG10p, std::string("Bi-color Blue/Green - Red/Green 10-bit packed")},
        {PixelFormat_BiColorBGRG12, std::string("Bi-color Blue/Green - Red/Green 12-bit unpacked")},
        {PixelFormat_BiColorBGRG12p, std::string("Bi-color Blue/Green - Red/Green 12-bit packed")},
        {PixelFormat_BiColorRGBG8, std::string("Bi-color Red/Green - Blue/Green 8-bit")},
        {PixelFormat_BiColorRGBG10, std::string("Bi-color Red/Green - Blue/Green 10-bit unpacked")},
        {PixelFormat_BiColorRGBG10p, std::string("Bi-color Red/Green - Blue/Green 10-bit packed")},
        {PixelFormat_BiColorRGBG12, std::string("Bi-color Red/Green - Blue/Green 12-bit unpacked")},
        {PixelFormat_BiColorRGBG12p, std::string("Bi-color Red/Green - Blue/Green 12-bit packed")},
        {PixelFormat_SCF1WBWG8, std::string("Sparse Color Filter #1 White-Blue-White-Green 8-bit")},
        {PixelFormat_SCF1WBWG10, std::string("Sparse Color Filter #1 White-Blue-White-Green 10-bit unpacked")},
        {PixelFormat_SCF1WBWG10p, std::string("Sparse Color Filter #1 White-Blue-White-Green 10-bit packed")},
        {PixelFormat_SCF1WBWG12, std::string("Sparse Color Filter #1 White-Blue-White-Green 12-bit unpacked")},
        {PixelFormat_SCF1WBWG12p, std::string("Sparse Color Filter #1 White-Blue-White-Green 12-bit packed")},
        {PixelFormat_SCF1WBWG14, std::string("Sparse Color Filter #1 White-Blue-White-Green 14-bit unpacked")},
        {PixelFormat_SCF1WBWG16, std::string("Sparse Color Filter #1 White-Blue-White-Green 16-bit unpacked")},
        {PixelFormat_SCF1WGWB8, std::string("Sparse Color Filter #1 White-Green-White-Blue 8-bit")},
        {PixelFormat_SCF1WGWB10, std::string("Sparse Color Filter #1 White-Green-White-Blue 10-bit unpacked")},
        {PixelFormat_SCF1WGWB10p, std::string("Sparse Color Filter #1 White-Green-White-Blue 10-bit packed")},
        {PixelFormat_SCF1WGWB12, std::string("Sparse Color Filter #1 White-Green-White-Blue 12-bit unpacked")},
        {PixelFormat_SCF1WGWB12p, std::string("Sparse Color Filter #1 White-Green-White-Blue 12-bit packed")},
        {PixelFormat_SCF1WGWB14, std::string("Sparse Color Filter #1 White-Green-White-Blue 14-bit unpacked")},
        {PixelFormat_SCF1WGWB16, std::string("Sparse Color Filter #1 White-Green-White-Blue 16-bit")},
        {PixelFormat_SCF1WGWR8, std::string("Sparse Color Filter #1 White-Green-White-Red 8-bit")},
        {PixelFormat_SCF1WGWR10, std::string("Sparse Color Filter #1 White-Green-White-Red 10-bit unpacked")},
        {PixelFormat_SCF1WGWR10p, std::string("Sparse Color Filter #1 White-Green-White-Red 10-bit packed")},
        {PixelFormat_SCF1WGWR12, std::string("Sparse Color Filter #1 White-Green-White-Red 12-bit unpacked")},
        {PixelFormat_SCF1WGWR12p, std::string("Sparse Color Filter #1 White-Green-White-Red 12-bit packed")},
        {PixelFormat_SCF1WGWR14, std::string("Sparse Color Filter #1 White-Green-White-Red 14-bit unpacked")},
        {PixelFormat_SCF1WGWR16, std::string("Sparse Color Filter #1 White-Green-White-Red 16-bit")},
        {PixelFormat_SCF1WRWG8, std::string("Sparse Color Filter #1 White-Red-White-Green 8-bit")},
        {PixelFormat_SCF1WRWG10, std::string("Sparse Color Filter #1 White-Red-White-Green 10-bit unpacked")},
        {PixelFormat_SCF1WRWG10p, std::string("Sparse Color Filter #1 White-Red-White-Green 10-bit packed")},
        {PixelFormat_SCF1WRWG12, std::string("Sparse Color Filter #1 White-Red-White-Green 12-bit unpacked")},
        {PixelFormat_SCF1WRWG12p, std::string("Sparse Color Filter #1 White-Red-White-Green 12-bit packed")},
        {PixelFormat_SCF1WRWG14, std::string("Sparse Color Filter #1 White-Red-White-Green 14-bit unpacked")},
        {PixelFormat_SCF1WRWG16, std::string("Sparse Color Filter #1 White-Red-White-Green 16-bit")},
        {PixelFormat_YCbCr8, std::string("YCbCr 4:4:4 8-bit")},
        {PixelFormat_YCbCr8_CbYCr, std::string("YCbCr 4:4:4 8-bit")},
        {PixelFormat_YCbCr10_CbYCr, std::string("YCbCr 4:4:4 10-bit unpacked")},
        {PixelFormat_YCbCr10p_CbYCr, std::string("YCbCr 4:4:4 10-bit packed")},
        {PixelFormat_YCbCr12_CbYCr, std::string("YCbCr 4:4:4 12-bit unpacked")},
        {PixelFormat_YCbCr12p_CbYCr, std::string("YCbCr 4:4:4 12-bit packed")},
        {PixelFormat_YCbCr411_8, std::string("YCbCr 4:1:1 8-bit")},
        {PixelFormat_YCbCr411_8_CbYYCrYY, std::string("YCbCr 4:1:1 8-bit")},
        {PixelFormat_YCbCr422_8, std::string("YCbCr 4:2:2 8-bit")},
        {PixelFormat_YCbCr422_8_CbYCrY, std::string("YCbCr 4:2:2 8-bit")},
        {PixelFormat_YCbCr422_10, std::string("YCbCr 4:2:2 10-bit unpacked")},
        {PixelFormat_YCbCr422_10_CbYCrY, std::string("YCbCr 4:2:2 10-bit unpacked")},
        {PixelFormat_YCbCr422_10p, std::string("YCbCr 4:2:2 10-bit packed")},
        {PixelFormat_YCbCr422_10p_CbYCrY, std::string("YCbCr 4:2:2 10-bit packed")},
        {PixelFormat_YCbCr422_12, std::string("YCbCr 4:2:2 12-bit unpacked")},
        {PixelFormat_YCbCr422_12_CbYCrY, std::string("YCbCr 4:2:2 12-bit unpacked")},
        {PixelFormat_YCbCr422_12p, std::string("YCbCr 4:2:2 12-bit packed")},
        {PixelFormat_YCbCr422_12p_CbYCrY, std::string("YCbCr 4:2:2 12-bit packed")},
        {PixelFormat_YCbCr601_8_CbYCr, std::string("YCbCr 4:4:4 8-bit BT.601")},
        {PixelFormat_YCbCr601_10_CbYCr, std::string("YCbCr 4:4:4 10-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_10p_CbYCr, std::string("YCbCr 4:4:4 10-bit packed BT.601")},
        {PixelFormat_YCbCr601_12_CbYCr, std::string("YCbCr 4:4:4 12-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_12p_CbYCr, std::string("YCbCr 4:4:4 12-bit packed BT.601")},
        {PixelFormat_YCbCr601_411_8_CbYYCrYY, std::string("YCbCr 4:1:1 8-bit BT.601")},
        {PixelFormat_YCbCr601_422_8, std::string("YCbCr 4:2:2 8-bit BT.601")},
        {PixelFormat_YCbCr601_422_8_CbYCrY, std::string("YCbCr 4:2:2 8-bit BT.601")},
        {PixelFormat_YCbCr601_422_10, std::string("YCbCr 4:2:2 10-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_10_CbYCrY, std::string("YCbCr 4:2:2 10-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_10p, std::string("YCbCr 4:2:2 10-bit packed BT.601")},
        {PixelFormat_YCbCr601_422_10p_CbYCrY, std::string("YCbCr 4:2:2 10-bit packed BT.601")},
        {PixelFormat_YCbCr601_422_12, std::string("YCbCr 4:2:2 12-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_12_CbYCrY, std::string("YCbCr 4:2:2 12-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_12p, std::string("YCbCr 4:2:2 12-bit packed BT.601")},
        {PixelFormat_YCbCr601_422_12p_CbYCrY, std::string("YCbCr 4:2:2 12-bit packed BT.601")},
        {PixelFormat_YCbCr709_8_CbYCr, std::string("YCbCr 4:4:4 8-bit BT.709")},
        {PixelFormat_YCbCr709_10_CbYCr, std::string("YCbCr 4:4:4 10-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_10p_CbYCr, std::string("YCbCr 4:4:4 10-bit packed BT.709")},
        {PixelFormat_YCbCr709_12_CbYCr, std::string("YCbCr 4:4:4 12-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_12p_CbYCr, std::string("YCbCr 4:4:4 12-bit packed BT.709")},
        {PixelFormat_YCbCr709_411_8_CbYYCrYY, std::string("YCbCr 4:1:1 8-bit BT.709")},
        {PixelFormat_YCbCr709_422_8, std::string("YCbCr 4:2:2 8-bit BT.709")},
        {PixelFormat_YCbCr709_422_8_CbYCrY, std::string("YCbCr 4:2:2 8-bit BT.709")},
        {PixelFormat_YCbCr709_422_10, std::string("YCbCr 4:2:2 10-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_10_CbYCrY, std::string("YCbCr 4:2:2 10-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_10p, std::string("YCbCr 4:2:2 10-bit packed BT.709")},
        {PixelFormat_YCbCr709_422_10p_CbYCrY, std::string("YCbCr 4:2:2 10-bit packed BT.709")},
        {PixelFormat_YCbCr709_422_12, std::string("YCbCr 4:2:2 12-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_12_CbYCrY, std::string("YCbCr 4:2:2 12-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_12p, std::string("YCbCr 4:2:2 12-bit packed BT.709")},
        {PixelFormat_YCbCr709_422_12p_CbYCrY, std::string("YCbCr 4:2:2 12-bit packed BT.709")},
        {PixelFormat_YUV8_UYV, std::string("YUV 4:4:4 8-bit")},
        {PixelFormat_YUV411_8_UYYVYY, std::string("YUV 4:1:1 8-bit")},
        {PixelFormat_YUV422_8, std::string("YUV 4:2:2 8-bit")},
        {PixelFormat_YUV422_8_UYVY, std::string("YUV 4:2:2 8-bit")},
        {PixelFormat_Mono10Packed, std::string("Pixel format set Mono 10 Packed.")},
        {PixelFormat_Mono12Packed, std::string("Pixel format set Mono 12 Packed.")},
        {PixelFormat_BayerGR12Packed, std::string("Pixel format set BayerGR 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_BayerRG12Packed, std::string("Pixel format set BayerRG 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_BayerGB12Packed, std::string("Pixel format set to BayerGB 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_BayerBG12Packed, std::string("Pixel format set to BayerBG 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_YUV411Packed, std::string("Pixel format set to YUV411 Packed.")},
        {PixelFormat_YUV422Packed, std::string("Pixel format set to YUV 422 Packed.")},
        {PixelFormat_YUV444Packed, std::string("Pixel format set to YUV 444 Packed.")},
        {PixelFormat_RGB8Packed, std::string("Pixel format set to RGB 8 Packed.")},
        {PixelFormat_Raw16, std::string("Raw 16 bit.")},
        {PixelFormat_Raw8, std::string("Raw bit.")},
        {PixelFormat_BayerGR10Packed, std::string("Bayer GR 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_BayerRG10Packed, std::string("Bayer RG 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_BayerGB10Packed, std::string("Bayer GB 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_BayerBG10Packed, std::string("Bayer BG 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_R12_Jpeg, std::string("Red 12-bit JPEG.")},
        {PixelFormat_GR12_Jpeg, std::string("Green Red 12-bit JPEG.")},
        {PixelFormat_GB12_Jpeg, std::string("Green Blue 12-bit JPEG.")},
        {PixelFormat_B12_Jpeg, std::string("Blue 12-bit packed JPEG.")},
        {UNKNOWN_PIXELFORMAT, std::string("Unknown Pixel Format")},
    };

    std::string getPixelFormatString_spin(spinPixelFormatEnums format)
    {
        if (pixelFormatToStringMap_spin.count(format) != 0)
        {
            return pixelFormatToStringMap_spin[format];
        }
        else
        {
            return std::string("unknown spin pixel format");
        }
    }

    void printAllPixelFormatStrings_spin()
    {
        std::cout << std::endl;
        for (auto kv : pixelFormatToStringMap_spin)
        {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }
        std::cout << std::endl;
    }

    //static std::map<spinBayerTileFormat, std::string> createBayerTileFromatToStringMap_spin()
    //{
    //    std::map<spinBayerTileFormat, std::string> map;
    //    map[SPIN_BT_NONE] = std::string("SPIN_BT_NONE");
    //    map[SPIN_BT_RGGB] = std::string("SPIN_BT_RGGB");
    //    map[SPIN_BT_GRBG] = std::string("SPIN_BT_GRBG");
    //    map[SPIN_BT_GBRG] = std::string("SPIN_BT_GBRG");
    //    map[SPIN_BT_BGGR] = std::string("SPIN_BT_BGGR");
    //    return map;
    //}

    //static std::map<spinBayerTileFormat, std::string> bayerTileFormatToStringMap_spin = 
    //    createBayerTileFromatToStringMap_spin();

    //std::string getBayerTileFormatString_spin(spinBayerTileFormat bayerFormat)
    //{
    //    if (bayerTileFormatToStringMap_spin.count(bayerFormat) != 0)
    //    {
    //        return bayerTileFormatToStringMap_spin[bayerFormat];
    //    }
    //    else 
    //    {
    //        return std::string("unknown spin bayer format");
    //    }
    //}

    //static std::map<spinVideoMode, std::string> createVideoModeToStringMap_spin()
    //{
    //    std::map<spinVideoMode, std::string> map;

    //    map[SPIN_VIDEOMODE_160x120YUV444]   =  std::string("SPIN_VIDEOMODE_160x120YUV444");
    //    map[SPIN_VIDEOMODE_320x240YUV422]   =  std::string("SPIN_VIDEOMODE_320x240YUV422");
    //    map[SPIN_VIDEOMODE_640x480YUV411]   =  std::string("SPIN_VIDEOMODE_640x480YUV411");
    //    map[SPIN_VIDEOMODE_640x480YUV422]   =  std::string("SPIN_VIDEOMODE_640x480YUV422");
    //    map[SPIN_VIDEOMODE_640x480RGB]      =  std::string("SPIN_VIDEOMODE_640x480RGB");
    //    map[SPIN_VIDEOMODE_640x480Y8]       =  std::string("SPIN_VIDEOMODE_640x480Y8");
    //    map[SPIN_VIDEOMODE_640x480Y16]      =  std::string("SPIN_VIDEOMODE_640x480Y16");
    //    map[SPIN_VIDEOMODE_800x600YUV422]   =  std::string("SPIN_VIDEOMODE_800x600YUV422");
    //    map[SPIN_VIDEOMODE_800x600RGB]      =  std::string("SPIN_VIDEOMODE_800x600RGB");
    //    map[SPIN_VIDEOMODE_800x600Y8]       =  std::string("SPIN_VIDEOMODE_800x600Y8");
    //    map[SPIN_VIDEOMODE_800x600Y16]      =  std::string("SPIN_VIDEOMODE_800x600Y16");
    //    map[SPIN_VIDEOMODE_1024x768YUV422]  =  std::string("SPIN_VIDEOMODE_1024x768YUV422");
    //    map[SPIN_VIDEOMODE_1024x768RGB]     =  std::string("SPIN_VIDEOMODE_1024x768RGB");
    //    map[SPIN_VIDEOMODE_1024x768Y8]      =  std::string("SPIN_VIDEOMODE_1024x768Y8");
    //    map[SPIN_VIDEOMODE_1024x768Y16]     =  std::string("SPIN_VIDEOMODE_1024x768Y16");
    //    map[SPIN_VIDEOMODE_1280x960YUV422]  =  std::string("SPIN_VIDEOMODE_1280x960YUV422");
    //    map[SPIN_VIDEOMODE_1280x960RGB]     =  std::string("SPIN_VIDEOMODE_1280x960RGB");
    //    map[SPIN_VIDEOMODE_1280x960Y8]      =  std::string("SPIN_VIDEOMODE_1280x960Y8");
    //    map[SPIN_VIDEOMODE_1280x960Y16]     =  std::string("SPIN_VIDEOMODE_1280x960Y16");
    //    map[SPIN_VIDEOMODE_1600x1200YUV422] =  std::string("SPIN_VIDEOMODE_1600x1200YUV422");
    //    map[SPIN_VIDEOMODE_1600x1200RGB]    =  std::string("SPIN_VIDEOMODE_1600x1200RGB");
    //    map[SPIN_VIDEOMODE_1600x1200Y8]     =  std::string("SPIN_VIDEOMODE_1600x1200Y8");
    //    map[SPIN_VIDEOMODE_1600x1200Y16]    =  std::string("SPIN_VIDEOMODE_1600x1200Y16");
    //    map[SPIN_VIDEOMODE_FORMAT7]         =  std::string("SPIN_VIDEOMODE_FORMAT7");
    //    return map;
    //}

    //static std::map<spinVideoMode, std::string> videoModeToStringMap_spin = 
    //    createVideoModeToStringMap_spin();

    //std::string getVideoModeString_spin(spinVideoMode vidMode)
    //{
    //    if (videoModeToStringMap_spin.count(vidMode) != 0)
    //    {
    //        return videoModeToStringMap_spin[vidMode];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin video mode");
    //    }
    //}

    //static std::map<spinFrameRate, std::string> createFrameRateToStringMap_spin()
    //{
    //    std::map<spinFrameRate, std::string> map;
    //    map[SPIN_FRAMERATE_1_875]    =  std::string("SPIN_FRAMERATE_1_875");
    //    map[SPIN_FRAMERATE_3_75]     =  std::string("SPIN_FRAMERATE_3_75");
    //    map[SPIN_FRAMERATE_7_5]      =  std::string("SPIN_FRAMERATE_7_5");
    //    map[SPIN_FRAMERATE_15]       =  std::string("SPIN_FRAMERATE_15");
    //    map[SPIN_FRAMERATE_30]       =  std::string("SPIN_FRAMERATE_30");
    //    map[SPIN_FRAMERATE_60]       =  std::string("SPIN_FRAMERATE_60");
    //    map[SPIN_FRAMERATE_120]      =  std::string("SPIN_FRAMERATE_120");
    //    map[SPIN_FRAMERATE_240]      =  std::string("SPIN_FRAMERATE_240");
    //    map[SPIN_FRAMERATE_FORMAT7]  =  std::string("SPIN_FRAMERATE_FORMAT7");
    //    return map;
    //}

    //static std::map<spinFrameRate, std::string> frameRateToStringMap_spin = 
    //    createFrameRateToStringMap_spin();

    //std::string getFrameRateString_spin(spinFrameRate frmRate)
    //{
    //    if (frameRateToStringMap_spin.count(frmRate) != 0) 
    //    {
    //        return frameRateToStringMap_spin[frmRate];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin framerate");
    //    }
    //}

    //static std::map<spinMode, std::string> createImageModeToStringMap_spin()
    //{
    //    std::map<spinMode, std::string> map;
    //    map[SPIN_MODE_0]   =   std::string("SPIN_MODE_0");
    //    map[SPIN_MODE_1]   =   std::string("SPIN_MODE_1");
    //    map[SPIN_MODE_2]   =   std::string("SPIN_MODE_2");
    //    map[SPIN_MODE_3]   =   std::string("SPIN_MODE_3");
    //    map[SPIN_MODE_4]   =   std::string("SPIN_MODE_4");
    //    map[SPIN_MODE_5]   =   std::string("SPIN_MODE_5");
    //    map[SPIN_MODE_6]   =   std::string("SPIN_MODE_6");
    //    map[SPIN_MODE_7]   =   std::string("SPIN_MODE_7");
    //    map[SPIN_MODE_8]   =   std::string("SPIN_MODE_8");
    //    map[SPIN_MODE_9]   =   std::string("SPIN_MODE_9");
    //    map[SPIN_MODE_10]  =   std::string("SPIN_MODE_10");
    //    map[SPIN_MODE_11]  =   std::string("SPIN_MODE_11");
    //    map[SPIN_MODE_12]  =   std::string("SPIN_MODE_12");
    //    map[SPIN_MODE_13]  =   std::string("SPIN_MODE_13");
    //    map[SPIN_MODE_14]  =   std::string("SPIN_MODE_14");
    //    map[SPIN_MODE_15]  =   std::string("SPIN_MODE_15");
    //    map[SPIN_MODE_16]  =   std::string("SPIN_MODE_16");
    //    map[SPIN_MODE_17]  =   std::string("SPIN_MODE_17");
    //    map[SPIN_MODE_18]  =   std::string("SPIN_MODE_18");
    //    map[SPIN_MODE_19]  =   std::string("SPIN_MODE_19");
    //    map[SPIN_MODE_20]  =   std::string("SPIN_MODE_20");
    //    map[SPIN_MODE_21]  =   std::string("SPIN_MODE_21");
    //    map[SPIN_MODE_22]  =   std::string("SPIN_MODE_22");
    //    map[SPIN_MODE_23]  =   std::string("SPIN_MODE_23");
    //    map[SPIN_MODE_24]  =   std::string("SPIN_MODE_24");
    //    map[SPIN_MODE_25]  =   std::string("SPIN_MODE_25");
    //    map[SPIN_MODE_26]  =   std::string("SPIN_MODE_26");
    //    map[SPIN_MODE_27]  =   std::string("SPIN_MODE_27");
    //    map[SPIN_MODE_28]  =   std::string("SPIN_MODE_28");
    //    map[SPIN_MODE_29]  =   std::string("SPIN_MODE_29");
    //    map[SPIN_MODE_30]  =   std::string("SPIN_MODE_30");
    //    map[SPIN_MODE_31]  =   std::string("SPIN_MODE_31");
    //    return map;
    //}

    //static std::map<spinMode, std::string> imageModeToStringMap_spin = 
    //    createImageModeToStringMap_spin();

    //std::string getModeString_spin(spinMode mode)
    //{

    //    if (imageModeToStringMap_spin.count(mode) != 0)
    //    {
    //        return imageModeToStringMap_spin[mode];
    //    }
    //    else {
    //        return std::string("uknown spin mode");
    //    }
    //}

    //static std::map<spinPropertyType, std::string> createPropertyTypeToStringMap_spin()
    //{
    //    std::map<spinPropertyType, std::string> map;
    //    map[SPIN_BRIGHTNESS]     =   std::string("SPIN_BRIGHTNESS");
    //    map[SPIN_AUTO_EXPOSURE]  =   std::string("SPIN_AUTO_EXPOSURE");
    //    map[SPIN_SHARPNESS]      =   std::string("SPIN_SHARPNESS");
    //    map[SPIN_WHITE_BALANCE]  =   std::string("SPIN_WHITE_BALANCE");
    //    map[SPIN_HUE]            =   std::string("SPIN_HUE");
    //    map[SPIN_SATURATION]     =   std::string("SPIN_SATURATION");
    //    map[SPIN_GAMMA]          =   std::string("SPIN_GAMMA");
    //    map[SPIN_IRIS]           =   std::string("SPIN_IRIS");
    //    map[SPIN_FOCUS]          =   std::string("SPIN_FOCUS");
    //    map[SPIN_ZOOM]           =   std::string("SPIN_ZOOM");
    //    map[SPIN_PAN]            =   std::string("SPIN_PAN");
    //    map[SPIN_TILT]           =   std::string("SPIN_TILT");
    //    map[SPIN_SHUTTER]        =   std::string("SPIN_SHUTTER");
    //    map[SPIN_GAIN]           =   std::string("SPIN_GAIN");
    //    map[SPIN_TRIGGER_MODE]   =   std::string("SPIN_TRIGGER_MODE");
    //    map[SPIN_TRIGGER_DELAY]  =   std::string("SPIN_TRIGGER_DELAY");
    //    map[SPIN_FRAME_RATE]     =   std::string("SPIN_FRAME_RATE");
    //    map[SPIN_TEMPERATURE]    =   std::string("SPIN_TEMPERATURE");
    //    return map;
    //}

    //static std::map<spinPropertyType, std::string> propertyTypeToStringMap_spin =  
    //    createPropertyTypeToStringMap_spin();

    //std::string getPropertyTypeString_spin(spinPropertyType propType)
    //{
    //    if (propertyTypeToStringMap_spin.count(propType) != 0)
    //    {
    //        return propertyTypeToStringMap_spin[propType];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin property type");
    //    }
    //}


    //static std::map<spinGrabTimeout, std::string> grabTimeoutToStringMap_spin
    //{
    //    {SPIN_TIMEOUT_NONE,        std::string("SPIN_TIMEOUT_NONE")},
    //    {SPIN_TIMEOUT_INFINITE,    std::string("SPIN_TIMEOUT_INFINITE")},
    //    {SPIN_TIMEOUT_UNSPECIFIED, std::string("SPIN_TIMEOUT_UNSPECIFIED")} 
    //};

    //std::string getGrabTimeoutString_spin(spinGrabTimeout grabTimeout)
    //{
    //    if (grabTimeoutToStringMap_spin.count(grabTimeout) != 0)
    //    {
    //        return grabTimeoutToStringMap_spin[grabTimeout];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin grabTimeout");
    //    }
    //}

    //std::string getGrabTimeoutString_spin(int grabTimeout)
    //{

    //    return getGrabTimeoutString_spin(spinGrabTimeout(grabTimeout));
    //}

    //static std::map<spinGrabMode, std::string> grabModeToStringMap_spin 
    //{ 
    //    {SPIN_DROP_FRAMES,            std::string("SPIN_DROP_FRAMES")}, 
    //    {SPIN_BUFFER_FRAMES,          std::string("SPIN_BUFFER_FRAMES")}, 
    //    {SPIN_UNSPECIFIED_GRAB_MODE,  std::string("SPIN_UNSPECIFIED_GRAB_MODE")}, 
    //};

    //std::string getGrabModeString_spin(spinGrabMode grabMode)
    //{
    //    if (grabModeToStringMap_spin.count(grabMode) != 0)
    //    {
    //        return grabModeToStringMap_spin[grabMode];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin grabMode");
    //    }
    //}


    //static std::map<spinBusSpeed, std::string> busSpeedToStringMap_spin
    //{

    //    {SPIN_BUSSPEED_S100,           std::string("SPIN_BUSSPEED_S100")}, 
    //    {SPIN_BUSSPEED_S200,           std::string("SPIN_BUSSPEED_S200")}, 
    //    {SPIN_BUSSPEED_S400,           std::string("SPIN_BUSSPEED_S400")}, 
    //    {SPIN_BUSSPEED_S480,           std::string("SPIN_BUSSPEED_S480")}, 
    //    {SPIN_BUSSPEED_S800,           std::string("SPIN_BUSSPEED_S800")}, 
    //    {SPIN_BUSSPEED_S1600,          std::string("SPIN_BUSSPEED_S1600")}, 
    //    {SPIN_BUSSPEED_S3200,          std::string("SPIN_BUSSPEED_S3200")}, 
    //    {SPIN_BUSSPEED_S5000,          std::string("SPIN_BUSSPEED_S5000")}, 
    //    {SPIN_BUSSPEED_10BASE_T,       std::string("SPIN_BUSSPEED_10BASE_T")}, 
    //    {SPIN_BUSSPEED_100BASE_T,      std::string("SPIN_BUSSPEED_100BASE_T")}, 
    //    {SPIN_BUSSPEED_1000BASE_T,     std::string("SPIN_BUSSPEED_1000BASE_T")}, 
    //    {SPIN_BUSSPEED_10000BASE_T,    std::string("SPIN_BUSSPEED_10000BASE_T")}, 
    //    {SPIN_BUSSPEED_S_FASTEST,      std::string("SPIN_BUSSPEED_S_FASTEST")}, 
    //    {SPIN_BUSSPEED_ANY,            std::string("SPIN_BUSSPEED_ANY")}, 
    //    {SPIN_BUSSPEED_SPEED_UNKNOWN,  std::string("SPIN_BUSSPEED_SPEED_UNKNOWN")} 
    //};

    //std::string getBusSpeedString_spin(spinBusSpeed busSpeed)
    //{
    //    if (busSpeedToStringMap_spin.count(busSpeed) !=0)
    //    {
    //        return busSpeedToStringMap_spin[busSpeed];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin busSpeed");
    //    }
    //}


    //static std::map<spinBandwidthAllocation, std::string> bandwidthAllocationToStringMap_spin
    //{
    //    {SPIN_BANDWIDTH_ALLOCATION_OFF,         std::string("SPIN_BANDWIDTH_ALLOCATION_OFF")}, 
    //    {SPIN_BANDWIDTH_ALLOCATION_ON,          std::string("SPIN_BANDWIDTH_ALLOCATION_ON")}, 
    //    {SPIN_BANDWIDTH_ALLOCATION_UNSUPPORTED, std::string("SPIN_BANDWIDTH_ALLOCATION_UNSUPPORTED")}, 
    //    {SPIN_BANDWIDTH_ALLOCATION_UNSPECIFIED, std::string("SPIN_BANDWIDTH_ALLOCATION_UNSPECIFIED")}, 
    //};

    //std::string getBandwidthAllocationString_spin(spinBandwidthAllocation bandwidthAllocation) 
    //{
    //    if (bandwidthAllocationToStringMap_spin.count(bandwidthAllocation)!=0)
    //    {
    //        return bandwidthAllocationToStringMap_spin[bandwidthAllocation];
    //    }
    //    else
    //    {
    //        return std::string("unknown spin bandwidth allocation");
    //    }
    //}


} // namespece bias

#endif // #ifdef WITH_SPIN
