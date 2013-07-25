#ifdef WITH_FC2
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <opencv2/core/core.hpp>
#include <map>
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias {

    // Image conversion - for mapping from FlyCapture2 to opencv 
    // -----------------------------------------------------------------------
    
    fc2PixelFormat getSuitablePixelFormat(fc2PixelFormat pixFormat)
    {
        fc2PixelFormat convPixFormat = FC2_PIXEL_FORMAT_MONO8;

        switch (pixFormat)
        {
            case FC2_PIXEL_FORMAT_RAW8:
                convPixFormat = FC2_PIXEL_FORMAT_RAW8;
                break;

            case FC2_PIXEL_FORMAT_MONO8:
                convPixFormat = FC2_PIXEL_FORMAT_MONO8;
                break;

            case FC2_PIXEL_FORMAT_RAW12:
            case FC2_PIXEL_FORMAT_RAW16:
            case FC2_PIXEL_FORMAT_MONO12:
            case FC2_PIXEL_FORMAT_MONO16:
            case FC2_PIXEL_FORMAT_S_MONO16:
                convPixFormat = FC2_PIXEL_FORMAT_MONO16;
                break;

            case FC2_PIXEL_FORMAT_RGB16:
            case FC2_PIXEL_FORMAT_S_RGB16:
            case FC2_PIXEL_FORMAT_BGR:
            case FC2_PIXEL_FORMAT_BGRU:
            case FC2_PIXEL_FORMAT_RGB:
            case FC2_PIXEL_FORMAT_RGBU:
            case FC2_PIXEL_FORMAT_BGR16:
	        case FC2_PIXEL_FORMAT_BGRU16:
            case FC2_PIXEL_FORMAT_422YUV8_JPEG:
            case FC2_PIXEL_FORMAT_411YUV8:
            case FC2_PIXEL_FORMAT_422YUV8:
            case FC2_PIXEL_FORMAT_444YUV8:
                convPixFormat = FC2_PIXEL_FORMAT_BGR;
                break;

            default:
                break;
        }

        return convPixFormat;

    }


    int getCompatibleOpencvFormat(fc2PixelFormat pixFormat)
    {
        int opencvFormat = CV_8UC1;

        switch (pixFormat)
        {
            case FC2_PIXEL_FORMAT_RAW8:
            case FC2_PIXEL_FORMAT_MONO8:
                opencvFormat = CV_8UC1;
                break;

            case FC2_PIXEL_FORMAT_MONO16:
                opencvFormat = CV_16UC1;
                break;

            case FC2_PIXEL_FORMAT_BGR:
                opencvFormat = CV_8UC3;
                break;

            default:
                std::stringstream ssError;
                ssError << "no compatible opencv format" << std::endl;
                throw RuntimeError(
                        ERROR_FC2_NO_COMPATIBLE_OPENCV_FORMAT, 
                        ssError.str()
                        );
                break;

        }
        return opencvFormat;
    }
    
    // Conversion from BIAS types to FlyCapture2 types
    // ------------------------------------------------------------------------

    static std::map<PropertyType, fc2PropertyType> createPropertyTypeMap_to_fc2()
    {
        std::map<PropertyType, fc2PropertyType> map;
        map[PROPERTY_TYPE_BRIGHTNESS]     =   FC2_BRIGHTNESS;
        map[PROPERTY_TYPE_AUTO_EXPOSURE]  =   FC2_AUTO_EXPOSURE;
        map[PROPERTY_TYPE_SHARPNESS]      =   FC2_SHARPNESS;
        map[PROPERTY_TYPE_WHITE_BALANCE]  =   FC2_WHITE_BALANCE;
        map[PROPERTY_TYPE_HUE]            =   FC2_HUE;
        map[PROPERTY_TYPE_SATURATION]     =   FC2_SATURATION;
        map[PROPERTY_TYPE_GAMMA]          =   FC2_GAMMA;
        map[PROPERTY_TYPE_IRIS]           =   FC2_IRIS;
        map[PROPERTY_TYPE_FOCUS]          =   FC2_FOCUS;
        map[PROPERTY_TYPE_ZOOM]           =   FC2_ZOOM;
        map[PROPERTY_TYPE_PAN]            =   FC2_PAN;
        map[PROPERTY_TYPE_TILT]           =   FC2_TILT;
        map[PROPERTY_TYPE_SHUTTER]        =   FC2_SHUTTER;
        map[PROPERTY_TYPE_GAIN]           =   FC2_GAIN;
        map[PROPERTY_TYPE_TRIGGER_MODE]   =   FC2_TRIGGER_MODE;
        map[PROPERTY_TYPE_TRIGGER_DELAY]  =   FC2_TRIGGER_DELAY;
        map[PROPERTY_TYPE_FRAME_RATE]     =   FC2_FRAME_RATE;
        map[PROPERTY_TYPE_TEMPERATURE]    =   FC2_TEMPERATURE;
        return map;
    };

    static std::map<PropertyType, fc2PropertyType> propertyTypeMap_to_fc2 = 
        createPropertyTypeMap_to_fc2();

    fc2PropertyType convertPropertyType_to_fc2(PropertyType propType)
    {
        if (propertyTypeMap_to_fc2.count(propType) != 0)
        {
            return propertyTypeMap_to_fc2[propType];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert property to FlyCaptuer2 property";
            throw RuntimeError(ERROR_FC2_CONVERT_PROPERTY_TYPE, ssError.str());
        }
    }

    static std::map<FrameRate, fc2FrameRate> createFrameRateMap_to_fc2()
    {
        std::map<FrameRate, fc2FrameRate> map;
        map[FRAMERATE_1_875]    = FC2_FRAMERATE_1_875;
        map[FRAMERATE_3_75]     = FC2_FRAMERATE_3_75;
        map[FRAMERATE_7_5]      = FC2_FRAMERATE_7_5;
        map[FRAMERATE_15]       = FC2_FRAMERATE_15;
        map[FRAMERATE_30]       = FC2_FRAMERATE_30;
        map[FRAMERATE_60]       = FC2_FRAMERATE_60;
        map[FRAMERATE_120]      = FC2_FRAMERATE_120;
        map[FRAMERATE_240]      = FC2_FRAMERATE_240;
        map[FRAMERATE_FORMAT7]  = FC2_FRAMERATE_FORMAT7;
        return map;
    };

    static std::map<FrameRate, fc2FrameRate> frameRateMap_to_fc2 = 
        createFrameRateMap_to_fc2();


    fc2FrameRate convertFrameRate_to_fc2(FrameRate frmRate)
    {
        if (frameRateMap_to_fc2.count(frmRate) != 0)
        {
            return frameRateMap_to_fc2[frmRate];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert framerate to fc2 frame rate";
            throw RuntimeError(ERROR_FC2_CONVERT_FRAMERATE, ssError.str());
        }
    }

    static std::map<VideoMode, fc2VideoMode> createVideoModeMap_to_fc2()
    {
        std::map<VideoMode, fc2VideoMode> map;
        map[VIDEOMODE_160x120YUV444]   =   FC2_VIDEOMODE_160x120YUV444;
        map[VIDEOMODE_320x240YUV422]   =   FC2_VIDEOMODE_320x240YUV422;
        map[VIDEOMODE_640x480YUV411]   =   FC2_VIDEOMODE_640x480YUV411;
        map[VIDEOMODE_640x480YUV422]   =   FC2_VIDEOMODE_640x480YUV422;
        map[VIDEOMODE_640x480RGB]      =   FC2_VIDEOMODE_640x480RGB;
        map[VIDEOMODE_640x480Y8]       =   FC2_VIDEOMODE_640x480Y8;
        map[VIDEOMODE_640x480Y16]      =   FC2_VIDEOMODE_640x480Y16;
        map[VIDEOMODE_800x600YUV422]   =   FC2_VIDEOMODE_800x600YUV422;
        map[VIDEOMODE_800x600RGB]      =   FC2_VIDEOMODE_800x600RGB;
        map[VIDEOMODE_800x600Y8]       =   FC2_VIDEOMODE_800x600Y8;
        map[VIDEOMODE_800x600Y16]      =   FC2_VIDEOMODE_800x600Y16;
        map[VIDEOMODE_1024x768YUV422]  =   FC2_VIDEOMODE_1024x768YUV422;
        map[VIDEOMODE_1024x768RGB]     =   FC2_VIDEOMODE_1024x768RGB;
        map[VIDEOMODE_1024x768Y8]      =   FC2_VIDEOMODE_1024x768Y8;
        map[VIDEOMODE_1024x768Y16]     =   FC2_VIDEOMODE_1024x768Y16;
        map[VIDEOMODE_1280x960YUV422]  =   FC2_VIDEOMODE_1280x960YUV422;
        map[VIDEOMODE_1280x960RGB]     =   FC2_VIDEOMODE_1280x960RGB;
        map[VIDEOMODE_1280x960Y8]      =   FC2_VIDEOMODE_1280x960Y8;
        map[VIDEOMODE_1280x960Y16]     =   FC2_VIDEOMODE_1280x960Y16;
        map[VIDEOMODE_1600x1200YUV422] =   FC2_VIDEOMODE_1600x1200YUV422;
        map[VIDEOMODE_1600x1200RGB]    =   FC2_VIDEOMODE_1600x1200RGB;
        map[VIDEOMODE_1600x1200Y8]     =   FC2_VIDEOMODE_1600x1200Y8;
        map[VIDEOMODE_1600x1200Y16]    =   FC2_VIDEOMODE_1600x1200Y16;
        map[VIDEOMODE_FORMAT7]         =   FC2_VIDEOMODE_FORMAT7;
        return map;
    }

    static std::map<VideoMode, fc2VideoMode> videoModeMap_to_fc2 = 
        createVideoModeMap_to_fc2();

    fc2VideoMode convertVideoMode_to_fc2(VideoMode vidMode)
    {
        if (videoModeMap_to_fc2.count(vidMode) != 0)
        {
            return videoModeMap_to_fc2[vidMode];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert video mode to FlyCapture2 video mode";
            throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
        }
    }

    static std::map<ImageMode, fc2Mode> createImageModeMap_to_fc2()
    {
        std::map<ImageMode, fc2Mode> map;
        map[IMAGEMODE_0]   =  FC2_MODE_0;
        map[IMAGEMODE_1]   =  FC2_MODE_1;
        map[IMAGEMODE_2]   =  FC2_MODE_2;
        map[IMAGEMODE_3]   =  FC2_MODE_3;
        map[IMAGEMODE_4]   =  FC2_MODE_4;
        map[IMAGEMODE_5]   =  FC2_MODE_5;
        map[IMAGEMODE_6]   =  FC2_MODE_6;
        map[IMAGEMODE_7]   =  FC2_MODE_7;
        map[IMAGEMODE_8]   =  FC2_MODE_8;
        map[IMAGEMODE_9]   =  FC2_MODE_9;
        map[IMAGEMODE_10]  =  FC2_MODE_10;
        map[IMAGEMODE_11]  =  FC2_MODE_11;
        map[IMAGEMODE_12]  =  FC2_MODE_12;
        map[IMAGEMODE_13]  =  FC2_MODE_13;
        map[IMAGEMODE_14]  =  FC2_MODE_14;
        map[IMAGEMODE_15]  =  FC2_MODE_15;
        map[IMAGEMODE_16]  =  FC2_MODE_16;
        map[IMAGEMODE_17]  =  FC2_MODE_17;
        map[IMAGEMODE_18]  =  FC2_MODE_18;
        map[IMAGEMODE_19]  =  FC2_MODE_19;
        map[IMAGEMODE_20]  =  FC2_MODE_20;
        map[IMAGEMODE_21]  =  FC2_MODE_21;
        map[IMAGEMODE_22]  =  FC2_MODE_22;
        map[IMAGEMODE_23]  =  FC2_MODE_23;
        map[IMAGEMODE_24]  =  FC2_MODE_24;
        map[IMAGEMODE_25]  =  FC2_MODE_25;
        map[IMAGEMODE_26]  =  FC2_MODE_26;
        map[IMAGEMODE_27]  =  FC2_MODE_27;
        map[IMAGEMODE_28]  =  FC2_MODE_28;
        map[IMAGEMODE_29]  =  FC2_MODE_29;
        map[IMAGEMODE_30]  =  FC2_MODE_30;
        map[IMAGEMODE_31]  =  FC2_MODE_31;
        return map;
    }

    static std::map<ImageMode, fc2Mode> imageModeMap_to_fc2 = 
        createImageModeMap_to_fc2();

    fc2Mode convertImageMode_to_fc2(ImageMode imgMode)
    {
        if (imageModeMap_to_fc2.count(imgMode) != 0) 
        {
            return imageModeMap_to_fc2[imgMode];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert image mode to FlyCapture2 ";
            ssError << "imaging mode";
            throw RuntimeError(ERROR_FC2_CONVERT_IMAGEMODE, ssError.str());
        }
    }

    fc2Property convertProperty_to_fc2(Property prop)
    {
        fc2Property prop_fc2;
        prop_fc2.type = convertPropertyType_to_fc2(prop.type);
        prop_fc2.present = (prop.present) ? 1 : 0;
        prop_fc2.absControl = (prop.absoluteControl) ? 1 : 0;
        prop_fc2.onePush = (prop.onePush) ? 1 : 0;
        prop_fc2.onOff = (prop.on) ? 1 : 0;
        prop_fc2.autoManualMode = (prop.autoActive) ? 1 : 0;
        prop_fc2.valueA = prop.value;
        prop_fc2.absValue = prop.absoluteValue;
        for (int i=0; i<8; i++) 
        {
            prop_fc2.reserved[i] = 0;
        }
        return prop_fc2;
    }

    static std::map<PixelFormat, fc2PixelFormat> createPixelFormatMap_to_fc2()
    {
        std::map<PixelFormat, fc2PixelFormat> map;
        map[PIXEL_FORMAT_MONO8]         =   FC2_PIXEL_FORMAT_MONO8;
        map[PIXEL_FORMAT_411YUV8]       =   FC2_PIXEL_FORMAT_411YUV8;
        map[PIXEL_FORMAT_422YUV8]       =   FC2_PIXEL_FORMAT_422YUV8;
        map[PIXEL_FORMAT_444YUV8]       =   FC2_PIXEL_FORMAT_444YUV8;
        map[PIXEL_FORMAT_RGB8]          =   FC2_PIXEL_FORMAT_RGB8;
        map[PIXEL_FORMAT_MONO16]        =   FC2_PIXEL_FORMAT_MONO16;
        map[PIXEL_FORMAT_RGB16]         =   FC2_PIXEL_FORMAT_RGB16;
        map[PIXEL_FORMAT_S_MONO16]      =   FC2_PIXEL_FORMAT_S_MONO16;
        map[PIXEL_FORMAT_S_RGB16]       =   FC2_PIXEL_FORMAT_S_RGB16;
        map[PIXEL_FORMAT_RAW8]          =   FC2_PIXEL_FORMAT_RAW8;
        map[PIXEL_FORMAT_RAW16]         =   FC2_PIXEL_FORMAT_RAW16;
        map[PIXEL_FORMAT_MONO12]        =   FC2_PIXEL_FORMAT_MONO12;
        map[PIXEL_FORMAT_RAW12]         =   FC2_PIXEL_FORMAT_RAW12;
        map[PIXEL_FORMAT_BGR]           =   FC2_PIXEL_FORMAT_BGR;
        map[PIXEL_FORMAT_BGRU]          =   FC2_PIXEL_FORMAT_BGRU;
        map[PIXEL_FORMAT_RGB]           =   FC2_PIXEL_FORMAT_RGB;
        map[PIXEL_FORMAT_RGBU]          =   FC2_PIXEL_FORMAT_RGBU;
        map[PIXEL_FORMAT_BGR16]         =   FC2_PIXEL_FORMAT_BGR16;
	    map[PIXEL_FORMAT_BGRU16]        =   FC2_PIXEL_FORMAT_BGRU16;
        map[PIXEL_FORMAT_422YUV8_JPEG]  =   FC2_PIXEL_FORMAT_422YUV8_JPEG;
        return map;
    };

    static std::map<PixelFormat, fc2PixelFormat> pixelFormatMap_to_fc2 = 
        createPixelFormatMap_to_fc2();

    fc2PixelFormat convertPixelFormat_to_fc2(PixelFormat pixFormat)
    {
        if (pixelFormatMap_to_fc2.count(pixFormat) != 0)
        {
            return pixelFormatMap_to_fc2[pixFormat];
        }
        else
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert pixel format to FlyCapture2 ";
            ssError << "imaging mode";
            throw RuntimeError(ERROR_FC2_CONVERT_PIXEL_FORMAT, ssError.str());
        }
    }


    fc2Format7ImageSettings convertFormat7Settings_to_fc2(Format7Settings settings)
    {
        fc2Format7ImageSettings settings_fc2;
        settings_fc2.mode = convertImageMode_to_fc2(settings.mode);
        settings_fc2.offsetX = settings.offsetX;
        settings_fc2.offsetY = settings.offsetY;
        settings_fc2.width = settings.width;
        settings_fc2.height = settings.height;
        settings_fc2.pixelFormat = convertPixelFormat_to_fc2(settings.pixelFormat);
        return settings_fc2;
    }

    // Conversion from FlyCapture2 types to BIAS types
    // ------------------------------------------------------------------------
     
    static std::map<fc2PropertyType, PropertyType> createPropertyTypeMap_from_fc2()
    {
        std::map<fc2PropertyType, PropertyType> map;
        map[FC2_BRIGHTNESS]     =   PROPERTY_TYPE_BRIGHTNESS;
        map[FC2_AUTO_EXPOSURE]  =   PROPERTY_TYPE_AUTO_EXPOSURE;
        map[FC2_SHARPNESS]      =   PROPERTY_TYPE_SHARPNESS;
        map[FC2_WHITE_BALANCE]  =   PROPERTY_TYPE_WHITE_BALANCE;
        map[FC2_HUE]            =   PROPERTY_TYPE_HUE;
        map[FC2_SATURATION]     =   PROPERTY_TYPE_SATURATION;
        map[FC2_GAMMA]          =   PROPERTY_TYPE_GAMMA;
        map[FC2_IRIS]           =   PROPERTY_TYPE_IRIS;
        map[FC2_FOCUS]          =   PROPERTY_TYPE_FOCUS;
        map[FC2_ZOOM]           =   PROPERTY_TYPE_ZOOM;
        map[FC2_PAN]            =   PROPERTY_TYPE_PAN;
        map[FC2_TILT]           =   PROPERTY_TYPE_TILT;
        map[FC2_SHUTTER]        =   PROPERTY_TYPE_SHUTTER;
        map[FC2_GAIN]           =   PROPERTY_TYPE_GAIN;
        map[FC2_TRIGGER_MODE]   =   PROPERTY_TYPE_TRIGGER_MODE;
        map[FC2_TRIGGER_DELAY]  =   PROPERTY_TYPE_TRIGGER_DELAY;
        map[FC2_FRAME_RATE]     =   PROPERTY_TYPE_FRAME_RATE;
        map[FC2_TEMPERATURE]    =   PROPERTY_TYPE_TEMPERATURE;
        return map;

    }

    static std::map<fc2PropertyType, PropertyType> propertyTypeMap_from_fc2 = 
        createPropertyTypeMap_from_fc2();

    PropertyType convertPropertyType_from_fc2(fc2PropertyType propType_fc2)
    {
        if (propertyTypeMap_from_fc2.count(propType_fc2) != 0) 
        {
            return propertyTypeMap_from_fc2[propType_fc2];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert FlyCapture2 PropertyType";
            throw RuntimeError(ERROR_FC2_CONVERT_PROPERTY_TYPE, ssError.str());
        }
    }

    static std::map<fc2VideoMode, VideoMode> createVideoModeMap_from_fc2()
    {
        std::map<fc2VideoMode, VideoMode> map;
        map[FC2_VIDEOMODE_160x120YUV444]     =   VIDEOMODE_160x120YUV444;
        map[FC2_VIDEOMODE_320x240YUV422]     =   VIDEOMODE_320x240YUV422;
        map[FC2_VIDEOMODE_640x480YUV411]     =   VIDEOMODE_640x480YUV411;
        map[FC2_VIDEOMODE_640x480YUV422]     =   VIDEOMODE_640x480YUV422;
        map[FC2_VIDEOMODE_640x480RGB]        =   VIDEOMODE_640x480RGB;
        map[FC2_VIDEOMODE_640x480Y8]         =   VIDEOMODE_640x480Y8;
        map[FC2_VIDEOMODE_640x480Y16]        =   VIDEOMODE_640x480Y16;
        map[FC2_VIDEOMODE_800x600YUV422]     =   VIDEOMODE_800x600YUV422;
        map[FC2_VIDEOMODE_800x600RGB]        =   VIDEOMODE_800x600RGB;
        map[FC2_VIDEOMODE_800x600Y8]         =   VIDEOMODE_800x600Y8;
        map[FC2_VIDEOMODE_800x600Y16]        =   VIDEOMODE_800x600Y16;
        map[FC2_VIDEOMODE_1024x768YUV422]    =   VIDEOMODE_1024x768YUV422;
        map[FC2_VIDEOMODE_1024x768RGB]       =   VIDEOMODE_1024x768RGB;
        map[FC2_VIDEOMODE_1024x768Y8]        =   VIDEOMODE_1024x768Y8;
        map[FC2_VIDEOMODE_1024x768Y16]       =   VIDEOMODE_1024x768Y16;
        map[FC2_VIDEOMODE_1280x960YUV422]    =   VIDEOMODE_1280x960YUV422;
        map[FC2_VIDEOMODE_1280x960RGB]       =   VIDEOMODE_1280x960RGB;
        map[FC2_VIDEOMODE_1280x960Y8]        =   VIDEOMODE_1280x960Y8;
        map[FC2_VIDEOMODE_1280x960Y16]       =   VIDEOMODE_1280x960Y16;
        map[FC2_VIDEOMODE_1600x1200YUV422]   =   VIDEOMODE_1600x1200YUV422;
        map[FC2_VIDEOMODE_1600x1200RGB]      =   VIDEOMODE_1600x1200RGB;
        map[FC2_VIDEOMODE_1600x1200Y8]       =   VIDEOMODE_1600x1200Y8;
        map[FC2_VIDEOMODE_1600x1200Y16]      =   VIDEOMODE_1600x1200Y16;
        map[FC2_VIDEOMODE_FORMAT7]           =   VIDEOMODE_FORMAT7;
        return map;
    }

    static std::map<fc2VideoMode, VideoMode> videoModeMap_from_fc2 = 
        createVideoModeMap_from_fc2();

    VideoMode convertVideoMode_from_fc2(fc2VideoMode vidMode_fc2)
    {
        if (videoModeMap_from_fc2.count(vidMode_fc2) != 0)
        {
            return videoModeMap_from_fc2[vidMode_fc2];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert FlyCapture2 VideoMode";
            throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
        }
    }

    static std::map<fc2FrameRate, FrameRate> createFrameRateMap_from_fc2()
    {
        std::map<fc2FrameRate, FrameRate> map;
        map[FC2_FRAMERATE_1_875]    =   FRAMERATE_1_875;
        map[FC2_FRAMERATE_3_75]     =   FRAMERATE_3_75;
        map[FC2_FRAMERATE_7_5]      =   FRAMERATE_7_5;
        map[FC2_FRAMERATE_15]       =   FRAMERATE_15;
        map[FC2_FRAMERATE_30]       =   FRAMERATE_30;
        map[FC2_FRAMERATE_60]       =   FRAMERATE_60;
        map[FC2_FRAMERATE_120]      =   FRAMERATE_120;
        map[FC2_FRAMERATE_240]      =   FRAMERATE_240;
        map[FC2_FRAMERATE_FORMAT7]  =   FRAMERATE_FORMAT7;
        return map;
    }

    static std::map<fc2FrameRate, FrameRate> frameRateMap_from_fc2 = 
        createFrameRateMap_from_fc2();

    FrameRate convertFrameRate_from_fc2(fc2FrameRate frmRate_fc2)
    {
        if (frameRateMap_from_fc2.count(frmRate_fc2) != 0)
        {
            return frameRateMap_from_fc2[frmRate_fc2];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert FlyCapture2 FrameRate";
            throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
        }
    }

    static std::map<fc2Mode, ImageMode> createImageModeMap_from_fc2()
    {
        std::map<fc2Mode, ImageMode> map;
        map[FC2_MODE_0]   =   IMAGEMODE_0;
        map[FC2_MODE_1]   =   IMAGEMODE_1;
        map[FC2_MODE_2]   =   IMAGEMODE_2;
        map[FC2_MODE_3]   =   IMAGEMODE_3;
        map[FC2_MODE_4]   =   IMAGEMODE_4;
        map[FC2_MODE_5]   =   IMAGEMODE_5;
        map[FC2_MODE_6]   =   IMAGEMODE_6;
        map[FC2_MODE_7]   =   IMAGEMODE_7;
        map[FC2_MODE_8]   =   IMAGEMODE_8;
        map[FC2_MODE_9]   =   IMAGEMODE_9;
        map[FC2_MODE_10]  =   IMAGEMODE_10;
        map[FC2_MODE_11]  =   IMAGEMODE_11;
        map[FC2_MODE_12]  =   IMAGEMODE_12;
        map[FC2_MODE_13]  =   IMAGEMODE_13;
        map[FC2_MODE_14]  =   IMAGEMODE_14;
        map[FC2_MODE_15]  =   IMAGEMODE_15;
        map[FC2_MODE_16]  =   IMAGEMODE_16;
        map[FC2_MODE_17]  =   IMAGEMODE_17;
        map[FC2_MODE_18]  =   IMAGEMODE_18;
        map[FC2_MODE_19]  =   IMAGEMODE_19;
        map[FC2_MODE_20]  =   IMAGEMODE_20;
        map[FC2_MODE_21]  =   IMAGEMODE_21;
        map[FC2_MODE_22]  =   IMAGEMODE_22;
        map[FC2_MODE_23]  =   IMAGEMODE_23;
        map[FC2_MODE_24]  =   IMAGEMODE_24;
        map[FC2_MODE_25]  =   IMAGEMODE_25;
        map[FC2_MODE_26]  =   IMAGEMODE_26;
        map[FC2_MODE_27]  =   IMAGEMODE_27;
        map[FC2_MODE_28]  =   IMAGEMODE_28;
        map[FC2_MODE_29]  =   IMAGEMODE_29;
        map[FC2_MODE_30]  =   IMAGEMODE_30;
        map[FC2_MODE_31]  =   IMAGEMODE_31;
        return map;
    }

    static std::map<fc2Mode, ImageMode> imageModeMap_from_fc2 = 
        createImageModeMap_from_fc2();

    ImageMode convertImageMode_from_fc2(fc2Mode imgMode_fc2)
    {
        if (imageModeMap_from_fc2.count(imgMode_fc2) != 0)
        {
            return imageModeMap_from_fc2[imgMode_fc2];
        }
        else
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert FlyCapture2 Mode";
            throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
        }
    }

    Property convertProperty_from_fc2(fc2Property prop_fc2)
    {
        Property prop;
        prop.type = convertPropertyType_from_fc2(prop_fc2.type);
        prop.present = bool(prop_fc2.present);
        prop.absoluteControl = bool(prop_fc2.absControl);
        prop.onePush = bool(prop_fc2.onePush);
        prop.on = bool(prop_fc2.onOff);
        prop.autoActive = bool(prop_fc2.autoManualMode);
        prop.value = prop_fc2.valueA;
        prop.absoluteValue = prop_fc2.absValue;
        return prop;
    }
    
    PropertyInfo convertPropertyInfo_from_fc2(fc2PropertyInfo propInfo_fc2) 
    {
        PropertyInfo propInfo;
        propInfo.type = convertPropertyType_from_fc2(propInfo_fc2.type);
        propInfo.present = bool(propInfo_fc2.present);
        propInfo.autoCapable = bool(propInfo_fc2.autoSupported);
        propInfo.manualCapable = bool(propInfo_fc2.manualSupported);
        propInfo.absoluteCapable = bool(propInfo_fc2.absValSupported);
        propInfo.onePushCapable = bool(propInfo_fc2.onePushSupported);
        propInfo.onOffCapable = bool(propInfo_fc2.onOffSupported);
        propInfo.minValue = propInfo_fc2.min;
        propInfo.maxValue = propInfo_fc2.max;
        propInfo.minAbsoluteValue = propInfo_fc2.absMin;
        propInfo.maxAbsoluteValue = propInfo_fc2.absMax;
        propInfo.units = std::string(propInfo_fc2.pUnits);
        propInfo.unitsAbbr = std::string(propInfo_fc2.pUnitAbbr);
        propInfo.haveUnits =  bool(propInfo.units.length());
        return propInfo;
    }

    static std::map<fc2PixelFormat, PixelFormat> createPixelFormatMap_from_fc2()
    {
        std::map<fc2PixelFormat, PixelFormat> map;
        map[FC2_PIXEL_FORMAT_MONO8]         =   PIXEL_FORMAT_MONO8;
        map[FC2_PIXEL_FORMAT_411YUV8]       =   PIXEL_FORMAT_411YUV8;
        map[FC2_PIXEL_FORMAT_422YUV8]       =   PIXEL_FORMAT_422YUV8;
        map[FC2_PIXEL_FORMAT_444YUV8]       =   PIXEL_FORMAT_444YUV8;
        map[FC2_PIXEL_FORMAT_RGB8]          =   PIXEL_FORMAT_RGB8;
        map[FC2_PIXEL_FORMAT_MONO16]        =   PIXEL_FORMAT_MONO16;
        map[FC2_PIXEL_FORMAT_RGB16]         =   PIXEL_FORMAT_RGB16;
        map[FC2_PIXEL_FORMAT_S_MONO16]      =   PIXEL_FORMAT_S_MONO16;
        map[FC2_PIXEL_FORMAT_S_RGB16]       =   PIXEL_FORMAT_S_RGB16;
        map[FC2_PIXEL_FORMAT_RAW8]          =   PIXEL_FORMAT_RAW8;
        map[FC2_PIXEL_FORMAT_RAW16]         =   PIXEL_FORMAT_RAW16;
        map[FC2_PIXEL_FORMAT_MONO12]        =   PIXEL_FORMAT_MONO12;
        map[FC2_PIXEL_FORMAT_RAW12]         =   PIXEL_FORMAT_RAW12;
        map[FC2_PIXEL_FORMAT_BGR]           =   PIXEL_FORMAT_BGR;
        map[FC2_PIXEL_FORMAT_BGRU]          =   PIXEL_FORMAT_BGRU;
        map[FC2_PIXEL_FORMAT_RGB]           =   PIXEL_FORMAT_RGB;
        map[FC2_PIXEL_FORMAT_RGBU]          =   PIXEL_FORMAT_RGBU;
        map[FC2_PIXEL_FORMAT_BGR16]         =   PIXEL_FORMAT_BGR16;
	    map[FC2_PIXEL_FORMAT_BGRU16]        =   PIXEL_FORMAT_BGRU16;
        map[FC2_PIXEL_FORMAT_422YUV8_JPEG]  =   PIXEL_FORMAT_422YUV8_JPEG;
        return map;
    };

    static std::map<fc2PixelFormat, PixelFormat> pixelFormatMap_from_fc2 = 
        createPixelFormatMap_from_fc2();

    PixelFormat convertPixelFormat_from_fc2(fc2PixelFormat pixFormat_fc2)
    {
        if (pixelFormatMap_from_fc2.count(pixFormat_fc2) != 0)
        {
            return pixelFormatMap_from_fc2[pixFormat_fc2];
        }
        else
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert pixel format from FlyCaptuer2";
            throw RuntimeError(ERROR_FC2_CONVERT_PROPERTY_TYPE, ssError.str());
        }
    }


    Format7Settings convertFormat7Settings_from_fc2(fc2Format7ImageSettings settings_fc2)
    {
        Format7Settings settings;
        settings.mode = convertImageMode_from_fc2(settings_fc2.mode);
        settings.offsetX = settings_fc2.offsetX;
        settings.offsetY = settings_fc2.offsetY;
        settings.width = settings_fc2.width;
        settings.height = settings_fc2.height;
        settings.pixelFormat = convertPixelFormat_from_fc2(settings_fc2.pixelFormat);
        return settings;
    }

    // Functions for printing information
    // --------------------------------------------------------------
    void printImageInfo_fc2(fc2Image &image)
    {
        std::cout << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << "FlyCapture2 Image Info" << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " rows:             " << image.rows << std::endl;
        std::cout << " cols:             " << image.cols << std::endl;
        std::cout << " stride:           " << image.stride << std::endl;
        std::cout << " dataSize:         " << image.dataSize << std::endl;
        std::cout << " receivedDataSize: " << image.receivedDataSize << std::endl;
        std::cout << " format            " << std::endl;
        std::cout << "   Code:           " << "0x" << std::hex << image.format << std::dec << std::endl;
        std::cout << "   String:         " << getPixelFormatString_fc2(image.format) << std::endl;
        std::cout << " bayerFormat       " << std::endl;
        std::cout << "   code:           " << "0x" << std::hex << image.bayerFormat << std::dec << std::endl;
        std::cout << "   string:         " << getBayerTileFormatString_fc2(image.bayerFormat) << std::endl;
        std::cout << std::endl;
    }

    void printFormat7ImageSettings_fc2(fc2Format7ImageSettings &imageSettings)
    {
        std::cout << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "FlyCatpure2 Format 7 Image settings " << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " mode:         " << getModeString_fc2(imageSettings.mode) << std::endl;
        std::cout << " offsetX:      " << imageSettings.offsetX << std::endl;
        std::cout << " offsetY:      " << imageSettings.offsetY << std::endl;
        std::cout << " width:        " << imageSettings.width << std::endl; 
        std::cout << " height:       " << imageSettings.height << std::endl;
        std::cout << " pixelFormat:  " << getPixelFormatString_fc2(imageSettings.pixelFormat) << std::endl;
        std::cout << " reserved:     " << std::endl;
        for (int i=0; i<8; i++) 
        {
            std::cout << "   [" << i << "] = " << std::hex << "0x" << imageSettings.reserved[i];
            std::cout << std::dec << std::endl;
        }
        std::cout << std::endl;
    }

    void printFormat7PacketSettings_fc2(unsigned int packetSize, float percentage)
    {
        std::cout << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "FlyCapture2 Format 7 Packet settings" << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " percentage:   " << percentage << std::endl;
        std::cout << " packetSize:   " << packetSize << std::endl;
        std::cout << std::endl;
    }

    void printFormat7Configuration_fc2(
            fc2Format7ImageSettings &imageSettings,
            unsigned int packetSize,
            float percentage 
            )
    {
        printFormat7ImageSettings_fc2(imageSettings);
        printFormat7PacketSettings_fc2(packetSize, percentage);
    }

    void printFormat7Info_fc2(fc2Format7Info &format7Info)
    {
        std::cout << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << "FlyCapture2 Format 7 Info" << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " mode:                      " << getModeString_fc2(format7Info.mode) << std::endl;
        std::cout << " maxWidth:                  " << format7Info.maxWidth << std::endl;
        std::cout << " maxHeight:                 " << format7Info.maxHeight << std::endl;
        std::cout << " offsetHStepSize:           " << format7Info.offsetHStepSize << std::endl;
        std::cout << " offsetVStepSize:           " << format7Info.offsetVStepSize << std::endl;
        std::cout << " imageHStepSize:            " << format7Info.imageHStepSize << std::endl;
        std::cout << " imageVStepSize:            " << format7Info.imageVStepSize << std:: endl;
        std::cout << " pixelFormatBitField:       " << format7Info.pixelFormatBitField << std::endl;
        std::cout << " pixelFormatBitField:       " << std::bitset<32>(format7Info.pixelFormatBitField) << std::endl;
        std::cout << " vendorPixelFormatBitField: " << format7Info.vendorPixelFormatBitField << std::endl;
        std::cout << " vendorPixelFormatBitField: " << std::bitset<32>(format7Info.vendorPixelFormatBitField) << std::endl;
        std::cout << " packetSize:                " << format7Info.packetSize << std::endl;
        std::cout << " minPacketSize:             " << format7Info.minPacketSize << std::endl;
        std::cout << " maxPacketSize:             " << format7Info.maxPacketSize << std::endl;
        std::cout << std::endl;
    }

    void printFormat7PacketInfo_fc2(fc2Format7PacketInfo &packetInfo)
    {
        std::cout << std::endl;
        std::cout << "--------------------------------" << std::endl;
        std::cout << "FlyCapture2 Format 7 Packet Info" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " recommendedBytesPerPacket:  " << packetInfo.recommendedBytesPerPacket << std::endl;
        std::cout << " maxBytesPerPacket:          " << packetInfo.maxBytesPerPacket << std::endl;
        std::cout << " unitBytesPerPacket:         " << packetInfo.unitBytesPerPacket << std::endl;
        std::cout << " reseverd                    " << std::endl;
        for (int i=0; i<8; i++) 
        {
            std::cout << "   [" << i << "] = " << packetInfo.reserved[i] << std::endl;
        }
        std::cout << std::endl;
    }

    void printPropertyInfo_fc2(fc2PropertyInfo &propInfo)
    {
        std::cout << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << "FlyCapture2 Property Info" << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " type:                " << getPropertyTypeString_fc2(propInfo.type) << std::endl;
        std::cout << std::boolalpha;
        std::cout << " present:             " << bool(propInfo.present) << std::endl;
        std::cout << " autoSupported:       " << bool(propInfo.autoSupported) << std::endl;
        std::cout << " manualSupported:     " << bool(propInfo.manualSupported) << std::endl;
        std::cout << " onOffSupported:      " << bool(propInfo.onOffSupported) << std::endl;
        std::cout << " onePushSupported:    " << bool(propInfo.onePushSupported) << std::endl;
        std::cout << " absValSupported:     " << bool(propInfo.absValSupported) << std::endl;
        std::cout << " readOutSupported:    " << bool(propInfo.absValSupported) << std::endl;
        std::cout << std::noboolalpha;
        std::cout << " min (unsigned int):  " << propInfo.min << std::endl;
        std::cout << " max (unsigned int):  " << propInfo.max << std::endl;
        std::cout << " absMin:              " << propInfo.absMin << std::endl;
        std::cout << " absMax:              " << propInfo.absMax << std::endl;
        std::cout << " pUnits:              " << propInfo.pUnits << std::endl;
        std::cout << " pUnitAbbr:           " << propInfo.pUnitAbbr << std::endl;
        std::cout << " reserved             " << std::endl;
        for (int i=0; i<8; i++)
        {
            std::cout << "   [" << i << "] = " << propInfo.reserved[i] << std::endl; 
        }
        std::cout << std::endl; 
    }

    void printProperty_fc2(fc2Property &prop)
    {
        std::cout << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << "FlyCapture2 Property" << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " type:            " << getPropertyTypeString_fc2(prop.type) << std::endl;
        std::cout << std::boolalpha << std::endl;
        std::cout << " present:         " << bool(prop.present) << std::endl; 
        std::cout << " absControl:      " << bool(prop.absControl) << std::endl;
        std::cout << " onePush:         " << bool(prop.onePush) << std::endl;
        std::cout << " onOff:           " << bool(prop.onOff) << std::endl;
        std::cout << " autoManualMode:  " << bool(prop.autoManualMode) << std::endl;
        std::cout << std::noboolalpha;
        std::cout << " valueA:          " << prop.valueA << std::endl;
        std::cout << " valueB:          " << prop.valueB << std::endl;
        std::cout << " absValue:        " << prop.absValue << std::endl;
        std::cout << " reserved         " << std::endl;
        for (int i=0; i<8; i++)
        {
            std::cout << "   [" << i << "] = " << prop.reserved[i] << std::endl; 
        }
        std::cout << std::endl; 
    }

    void printTriggerMode_fc2(fc2TriggerMode &trigMode)
    {
        std::cout << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << "FlyCapture2 TriggerMode" << std::endl;
        std::cout << "-----------------------" << std::endl;
        std::cout << std::endl << std::boolalpha;
        std::cout << " onOff:     " << bool(trigMode.onOff) << std::endl;
        std::cout << std::noboolalpha;
        std::cout << " polarity:  " << trigMode.polarity << std::endl;
        std::cout << " source:    " << trigMode.source << std::endl;
        std::cout << " mode:      " << trigMode.mode << std::endl;
        std::cout << " parameter: " << trigMode.parameter << std::endl;
        std::cout << " reserved   " << std::endl;
        for (int i=0; i<8; i++)
        {
            std::cout << "   [" << i << "] = " << trigMode.reserved[i] << std::endl; 
        }
        std::cout << std::endl; 
    }

    void printTriggerModeInfo_fc2(fc2TriggerModeInfo &trigModeInfo)
    {
        std::cout << std::endl;
        std::cout << "---------------------------" << std::endl;
        std::cout << "FlyCapture2 TriggerModeInfo" << std::endl;
        std::cout << "---------------------------" << std::endl;
        std::cout << std::endl << std::boolalpha;
        std::cout << " present:                  " << bool(trigModeInfo.present) << std::endl;
        std::cout << " readOutSupported:         " << bool(trigModeInfo.readOutSupported) << std::endl;
        std::cout << " onOffSupported:           " << bool(trigModeInfo.onOffSupported) << std::endl;
        std::cout << " polaritySupported:        " << bool(trigModeInfo.polaritySupported) << std::endl;
        std::cout << " valueReadable:            " << bool(trigModeInfo.valueReadable) << std::endl;
        std::cout << " sourceMask:               " << std::bitset<32>(trigModeInfo.sourceMask) << std::endl;
        std::cout << " softwareTriggerSupported: " << bool(trigModeInfo.softwareTriggerSupported) << std::endl;
        std::cout << " modeMask:                 " << std::bitset<32>(trigModeInfo.modeMask) << std::endl;
        std::cout << " reserved   " << std::endl;
        for (int i=0; i<8; i++)
        {
            std::cout << "   [" << i << "] = " << trigModeInfo.reserved[i] << std::endl; 
        }
        std::cout << std::noboolalpha << std::endl;
    }

    // Functions for converting FlyCapture2 enumerations to strings   
    // Mostly autogenerated. 
    // ------------------------------------------------------------------------

    static std::map<fc2InterfaceType, std::string> createInterfaceTypeToStringMap_fc2()
    {
        std::map<fc2InterfaceType, std::string> map;
        map[FC2_INTERFACE_IEEE1394]  =   std::string("FC2_INTERFACE_IEEE1394");
        map[FC2_INTERFACE_USB_2]     =   std::string("FC2_INTERFACE_USB_2");
        map[FC2_INTERFACE_USB_3]     =   std::string("FC2_INTERFACE_USB_3");
        map[FC2_INTERFACE_GIGE]      =   std::string("FC2_INTERFACE_GIGE");
        map[FC2_INTERFACE_UNKNOWN]   =   std::string("FC2_INTERFACE_UNKNOWN");
        return map;
    }

    static std::map<fc2InterfaceType, std::string> interfaceTypeToStringMap_fc2 = 
        createInterfaceTypeToStringMap_fc2();

    std::string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType)
    {
        if (interfaceTypeToStringMap_fc2.count(ifaceType) != 0)
        {
            return interfaceTypeToStringMap_fc2[ifaceType];
        }
        else
        {
            return std::string("unknown fc2 interface value");
        }
    }

    static std::map<fc2PixelFormat, std::string> createPixelFormatToStringMap_fc2()
    {
        std::map<fc2PixelFormat, std::string> map;
        map[FC2_PIXEL_FORMAT_MONO8] = std::string("FC2_PIXEL_FORMAT_MONO8");
        map[FC2_PIXEL_FORMAT_411YUV8]      =   std::string("FC2_PIXEL_FORMAT_411YUV8");
        map[FC2_PIXEL_FORMAT_422YUV8]      =   std::string("FC2_PIXEL_FORMAT_422YUV8");
        map[FC2_PIXEL_FORMAT_444YUV8]      =   std::string("FC2_PIXEL_FORMAT_444YUV8");
        map[FC2_PIXEL_FORMAT_RGB8]         =   std::string("FC2_PIXEL_FORMAT_RGB8");
        map[FC2_PIXEL_FORMAT_MONO16]       =   std::string("FC2_PIXEL_FORMAT_MONO16");
        map[FC2_PIXEL_FORMAT_RGB16]        =   std::string("FC2_PIXEL_FORMAT_RGB16");
        map[FC2_PIXEL_FORMAT_S_MONO16]     =   std::string("FC2_PIXEL_FORMAT_S_MONO16");
        map[FC2_PIXEL_FORMAT_S_RGB16]      =   std::string("FC2_PIXEL_FORMAT_S_RGB16");
        map[FC2_PIXEL_FORMAT_RAW8]         =   std::string("FC2_PIXEL_FORMAT_RAW8");
        map[FC2_PIXEL_FORMAT_RAW16]        =   std::string("FC2_PIXEL_FORMAT_RAW16");
        map[FC2_PIXEL_FORMAT_MONO12]       =   std::string("FC2_PIXEL_FORMAT_MONO12");
        map[FC2_PIXEL_FORMAT_RAW12]        =   std::string("FC2_PIXEL_FORMAT_RAW12");
        map[FC2_PIXEL_FORMAT_BGR]          =   std::string("FC2_PIXEL_FORMAT_BGR");
        map[FC2_PIXEL_FORMAT_BGRU]         =   std::string("FC2_PIXEL_FORMAT_BGRU");
        map[FC2_PIXEL_FORMAT_RGBU]         =   std::string("FC2_PIXEL_FORMAT_RGBU");
        map[FC2_PIXEL_FORMAT_BGR16]        =   std::string("FC2_PIXEL_FORMAT_BGR16");
        map[FC2_PIXEL_FORMAT_BGRU16]       =   std::string("FC2_PIXEL_FORMAT_BGRU16");
        map[FC2_PIXEL_FORMAT_422YUV8_JPEG] =   std::string("FC2_PIXEL_FORMAT_422YUV8_JPEG");
        map[FC2_NUM_PIXEL_FORMATS]         =   std::string("FC2_NUM_PIXEL_FORMATS");
        map[FC2_UNSPECIFIED_PIXEL_FORMAT]  =   std::string("FC2_UNSPECIFIED_PIXEL_FORMAT");
        return map;
    }

    static std::map<fc2PixelFormat, std::string> pixelFormatToStringMap_fc2 = 
        createPixelFormatToStringMap_fc2();

    std::string getPixelFormatString_fc2(fc2PixelFormat format)
    {
        if (pixelFormatToStringMap_fc2.count(format) != 0)
        {
            return pixelFormatToStringMap_fc2[format];
        }
        else
        {
            return std::string("unknown fc2 pixel format");
        }
    }

    static std::map<fc2BayerTileFormat, std::string> createBayerTileFromatToStringMap_fc2()
    {
        std::map<fc2BayerTileFormat, std::string> map;
        map[FC2_BT_NONE] = std::string("FC2_BT_NONE");
        map[FC2_BT_RGGB] = std::string("FC2_BT_RGGB");
        map[FC2_BT_GRBG] = std::string("FC2_BT_GRBG");
        map[FC2_BT_GBRG] = std::string("FC2_BT_GBRG");
        map[FC2_BT_BGGR] = std::string("FC2_BT_BGGR");
        return map;
    }

    static std::map<fc2BayerTileFormat, std::string> bayerTileFormatToStringMap_fc2 = 
        createBayerTileFromatToStringMap_fc2();

    std::string getBayerTileFormatString_fc2(fc2BayerTileFormat bayerFormat)
    {
        if (bayerTileFormatToStringMap_fc2.count(bayerFormat) != 0)
        {
            return bayerTileFormatToStringMap_fc2[bayerFormat];
        }
        else 
        {
            return std::string("unknown fc2 bayer format");
        }
    }

    static std::map<fc2VideoMode, std::string> createVideoModeToStringMap_fc2()
    {
        std::map<fc2VideoMode, std::string> map;

        map[FC2_VIDEOMODE_160x120YUV444]   =  std::string("FC2_VIDEOMODE_160x120YUV444");
        map[FC2_VIDEOMODE_320x240YUV422]   =  std::string("FC2_VIDEOMODE_320x240YUV422");
        map[FC2_VIDEOMODE_640x480YUV411]   =  std::string("FC2_VIDEOMODE_640x480YUV411");
        map[FC2_VIDEOMODE_640x480YUV422]   =  std::string("FC2_VIDEOMODE_640x480YUV422");
        map[FC2_VIDEOMODE_640x480RGB]      =  std::string("FC2_VIDEOMODE_640x480RGB");
        map[FC2_VIDEOMODE_640x480Y8]       =  std::string("FC2_VIDEOMODE_640x480Y8");
        map[FC2_VIDEOMODE_640x480Y16]      =  std::string("FC2_VIDEOMODE_640x480Y16");
        map[FC2_VIDEOMODE_800x600YUV422]   =  std::string("FC2_VIDEOMODE_800x600YUV422");
        map[FC2_VIDEOMODE_800x600RGB]      =  std::string("FC2_VIDEOMODE_800x600RGB");
        map[FC2_VIDEOMODE_800x600Y8]       =  std::string("FC2_VIDEOMODE_800x600Y8");
        map[FC2_VIDEOMODE_800x600Y16]      =  std::string("FC2_VIDEOMODE_800x600Y16");
        map[FC2_VIDEOMODE_1024x768YUV422]  =  std::string("FC2_VIDEOMODE_1024x768YUV422");
        map[FC2_VIDEOMODE_1024x768RGB]     =  std::string("FC2_VIDEOMODE_1024x768RGB");
        map[FC2_VIDEOMODE_1024x768Y8]      =  std::string("FC2_VIDEOMODE_1024x768Y8");
        map[FC2_VIDEOMODE_1024x768Y16]     =  std::string("FC2_VIDEOMODE_1024x768Y16");
        map[FC2_VIDEOMODE_1280x960YUV422]  =  std::string("FC2_VIDEOMODE_1280x960YUV422");
        map[FC2_VIDEOMODE_1280x960RGB]     =  std::string("FC2_VIDEOMODE_1280x960RGB");
        map[FC2_VIDEOMODE_1280x960Y8]      =  std::string("FC2_VIDEOMODE_1280x960Y8");
        map[FC2_VIDEOMODE_1280x960Y16]     =  std::string("FC2_VIDEOMODE_1280x960Y16");
        map[FC2_VIDEOMODE_1600x1200YUV422] =  std::string("FC2_VIDEOMODE_1600x1200YUV422");
        map[FC2_VIDEOMODE_1600x1200RGB]    =  std::string("FC2_VIDEOMODE_1600x1200RGB");
        map[FC2_VIDEOMODE_1600x1200Y8]     =  std::string("FC2_VIDEOMODE_1600x1200Y8");
        map[FC2_VIDEOMODE_1600x1200Y16]    =  std::string("FC2_VIDEOMODE_1600x1200Y16");
        map[FC2_VIDEOMODE_FORMAT7]         =  std::string("FC2_VIDEOMODE_FORMAT7");
        return map;
    }

    static std::map<fc2VideoMode, std::string> videoModeToStringMap_fc2 = 
        createVideoModeToStringMap_fc2();

    std::string getVideoModeString_fc2(fc2VideoMode vidMode)
    {
        if (videoModeToStringMap_fc2.count(vidMode) != 0)
        {
            return videoModeToStringMap_fc2[vidMode];
        }
        else
        {
            return std::string("unknown fc2 video mode");
        }
    }

    static std::map<fc2FrameRate, std::string> createFrameRateToStringMap_fc2()
    {
        std::map<fc2FrameRate, std::string> map;
        map[FC2_FRAMERATE_1_875]    =  std::string("FC2_FRAMERATE_1_875");
        map[FC2_FRAMERATE_3_75]     =  std::string("FC2_FRAMERATE_3_75");
        map[FC2_FRAMERATE_7_5]      =  std::string("FC2_FRAMERATE_7_5");
        map[FC2_FRAMERATE_15]       =  std::string("FC2_FRAMERATE_15");
        map[FC2_FRAMERATE_30]       =  std::string("FC2_FRAMERATE_30");
        map[FC2_FRAMERATE_60]       =  std::string("FC2_FRAMERATE_60");
        map[FC2_FRAMERATE_120]      =  std::string("FC2_FRAMERATE_120");
        map[FC2_FRAMERATE_240]      =  std::string("FC2_FRAMERATE_240");
        map[FC2_FRAMERATE_FORMAT7]  =  std::string("FC2_FRAMERATE_FORMAT7");
        return map;
    }

    static std::map<fc2FrameRate, std::string> frameRateToStringMap_fc2 = 
        createFrameRateToStringMap_fc2();

    std::string getFrameRateString_fc2(fc2FrameRate frmRate)
    {
        if (frameRateToStringMap_fc2.count(frmRate) != 0) 
        {
            return frameRateToStringMap_fc2[frmRate];
        }
        else
        {
            return std::string("unknown fc2 framerate");
        }
    }

    static std::map<fc2Mode, std::string> createImageModeToStringMap_fc2()
    {
        std::map<fc2Mode, std::string> map;
        map[FC2_MODE_0]   =   std::string("FC2_MODE_0");
        map[FC2_MODE_1]   =   std::string("FC2_MODE_1");
        map[FC2_MODE_2]   =   std::string("FC2_MODE_2");
        map[FC2_MODE_3]   =   std::string("FC2_MODE_3");
        map[FC2_MODE_4]   =   std::string("FC2_MODE_4");
        map[FC2_MODE_5]   =   std::string("FC2_MODE_5");
        map[FC2_MODE_6]   =   std::string("FC2_MODE_6");
        map[FC2_MODE_7]   =   std::string("FC2_MODE_7");
        map[FC2_MODE_8]   =   std::string("FC2_MODE_8");
        map[FC2_MODE_9]   =   std::string("FC2_MODE_9");
        map[FC2_MODE_10]  =   std::string("FC2_MODE_10");
        map[FC2_MODE_11]  =   std::string("FC2_MODE_11");
        map[FC2_MODE_12]  =   std::string("FC2_MODE_12");
        map[FC2_MODE_13]  =   std::string("FC2_MODE_13");
        map[FC2_MODE_14]  =   std::string("FC2_MODE_14");
        map[FC2_MODE_15]  =   std::string("FC2_MODE_15");
        map[FC2_MODE_16]  =   std::string("FC2_MODE_16");
        map[FC2_MODE_17]  =   std::string("FC2_MODE_17");
        map[FC2_MODE_18]  =   std::string("FC2_MODE_18");
        map[FC2_MODE_19]  =   std::string("FC2_MODE_19");
        map[FC2_MODE_20]  =   std::string("FC2_MODE_20");
        map[FC2_MODE_21]  =   std::string("FC2_MODE_21");
        map[FC2_MODE_22]  =   std::string("FC2_MODE_22");
        map[FC2_MODE_23]  =   std::string("FC2_MODE_23");
        map[FC2_MODE_24]  =   std::string("FC2_MODE_24");
        map[FC2_MODE_25]  =   std::string("FC2_MODE_25");
        map[FC2_MODE_26]  =   std::string("FC2_MODE_26");
        map[FC2_MODE_27]  =   std::string("FC2_MODE_27");
        map[FC2_MODE_28]  =   std::string("FC2_MODE_28");
        map[FC2_MODE_29]  =   std::string("FC2_MODE_29");
        map[FC2_MODE_30]  =   std::string("FC2_MODE_30");
        map[FC2_MODE_31]  =   std::string("FC2_MODE_31");
        return map;
    }

    static std::map<fc2Mode, std::string> imageModeToStringMap_fc2 = 
        createImageModeToStringMap_fc2();

    std::string getModeString_fc2(fc2Mode mode)
    {

        if (imageModeToStringMap_fc2.count(mode) != 0)
        {
            return imageModeToStringMap_fc2[mode];
        }
        else {
            return std::string("uknown fc2 mode");
        }
    }

    static std::map<fc2PropertyType, std::string> createPropertyTypeToStringMap_fc2()
    {
        std::map<fc2PropertyType, std::string> map;
        map[FC2_BRIGHTNESS]     =   std::string("FC2_BRIGHTNESS");
        map[FC2_AUTO_EXPOSURE]  =   std::string("FC2_AUTO_EXPOSURE");
        map[FC2_SHARPNESS]      =   std::string("FC2_SHARPNESS");
        map[FC2_WHITE_BALANCE]  =   std::string("FC2_WHITE_BALANCE");
        map[FC2_HUE]            =   std::string("FC2_HUE");
        map[FC2_SATURATION]     =   std::string("FC2_SATURATION");
        map[FC2_GAMMA]          =   std::string("FC2_GAMMA");
        map[FC2_IRIS]           =   std::string("FC2_IRIS");
        map[FC2_FOCUS]          =   std::string("FC2_FOCUS");
        map[FC2_ZOOM]           =   std::string("FC2_ZOOM");
        map[FC2_PAN]            =   std::string("FC2_PAN");
        map[FC2_TILT]           =   std::string("FC2_TILT");
        map[FC2_SHUTTER]        =   std::string("FC2_SHUTTER");
        map[FC2_GAIN]           =   std::string("FC2_GAIN");
        map[FC2_TRIGGER_MODE]   =   std::string("FC2_TRIGGER_MODE");
        map[FC2_TRIGGER_DELAY]  =   std::string("FC2_TRIGGER_DELAY");
        map[FC2_FRAME_RATE]     =   std::string("FC2_FRAME_RATE");
        map[FC2_TEMPERATURE]    =   std::string("FC2_TEMPERATURE");
        return map;
    }

    static std::map<fc2PropertyType, std::string> propertyTypeToStringMap_fc2 =  
        createPropertyTypeToStringMap_fc2();

    std::string getPropertyTypeString_fc2(fc2PropertyType propType)
    {
        if (propertyTypeToStringMap_fc2.count(propType) != 0)
        {
            return propertyTypeToStringMap_fc2[propType];
        }
        else
        {
            return std::string("unknown fc2 property type");
        }
    }

} // namespece bias

#endif // #ifdef WITH_FC2
