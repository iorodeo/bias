#include "utils_dc1394.hpp"
#include "exception.hpp"
#include "utils.hpp"
#include <opencv2/core/core.hpp>
#include <map>
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias
{

    // Conversion from BIAS types to libdc1394 types
    // ------------------------------------------------------------------------
    static std::map<PropertyType, dc1394feature_t> createPropertyTypeMap_to_dc1394()
    {
        std::map<PropertyType, dc1394feature_t> map;
        map[PROPERTY_TYPE_BRIGHTNESS]     =  DC1394_FEATURE_BRIGHTNESS;
        map[PROPERTY_TYPE_AUTO_EXPOSURE]  =  DC1394_FEATURE_EXPOSURE;
        map[PROPERTY_TYPE_SHARPNESS]      =  DC1394_FEATURE_SHARPNESS;
        map[PROPERTY_TYPE_WHITE_BALANCE]  =  DC1394_FEATURE_WHITE_BALANCE;
        map[PROPERTY_TYPE_HUE]            =  DC1394_FEATURE_HUE;
        map[PROPERTY_TYPE_SATURATION]     =  DC1394_FEATURE_SATURATION;
        map[PROPERTY_TYPE_GAMMA]          =  DC1394_FEATURE_GAMMA;
        map[PROPERTY_TYPE_IRIS]           =  DC1394_FEATURE_IRIS;
        map[PROPERTY_TYPE_FOCUS]          =  DC1394_FEATURE_FOCUS;
        map[PROPERTY_TYPE_ZOOM]           =  DC1394_FEATURE_ZOOM;
        map[PROPERTY_TYPE_PAN]            =  DC1394_FEATURE_PAN;
        map[PROPERTY_TYPE_TILT]           =  DC1394_FEATURE_TILT;
        map[PROPERTY_TYPE_SHUTTER]        =  DC1394_FEATURE_SHUTTER;
        map[PROPERTY_TYPE_GAIN]           =  DC1394_FEATURE_GAIN;
        map[PROPERTY_TYPE_TRIGGER_MODE]   =  DC1394_FEATURE_TRIGGER;
        map[PROPERTY_TYPE_TRIGGER_DELAY]  =  DC1394_FEATURE_TRIGGER_DELAY;
        map[PROPERTY_TYPE_FRAME_RATE]     =  DC1394_FEATURE_FRAME_RATE;
        map[PROPERTY_TYPE_TEMPERATURE]    =  DC1394_FEATURE_TEMPERATURE ;
        return map;
    }

    static std::map<PropertyType, dc1394feature_t> propertyTypeMap_to_dc1394 = 
        createPropertyTypeMap_to_dc1394();

    dc1394feature_t convertPropertyType_to_dc1394(PropertyType propType)
    {
        dc1394feature_t feature;
        if (propertyTypeMap_to_dc1394.count(propType) == 0)
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert propertyType to libdc1394 feature";
            throw RuntimeError(ERROR_DC1394_CONVERT_PROPERTY_TYPE, ssError.str());
        }
        return propertyTypeMap_to_dc1394[propType];
    };

    static std::map<VideoMode, dc1394video_mode_t> createVideoModeMap_to_dc1394()
    {
        std::map<VideoMode, dc1394video_mode_t> map;
        map[VIDEOMODE_160x120YUV444]   =   DC1394_VIDEO_MODE_160x120_YUV444;
        map[VIDEOMODE_320x240YUV422]   =   DC1394_VIDEO_MODE_320x240_YUV422;
        map[VIDEOMODE_640x480YUV411]   =   DC1394_VIDEO_MODE_640x480_YUV411;
        map[VIDEOMODE_640x480YUV422]   =   DC1394_VIDEO_MODE_640x480_YUV422;
        map[VIDEOMODE_640x480RGB]      =   DC1394_VIDEO_MODE_640x480_RGB8;
        map[VIDEOMODE_640x480Y8]       =   DC1394_VIDEO_MODE_640x480_MONO8;
        map[VIDEOMODE_640x480Y16]      =   DC1394_VIDEO_MODE_640x480_MONO16;
        map[VIDEOMODE_800x600YUV422]   =   DC1394_VIDEO_MODE_800x600_YUV422;
        map[VIDEOMODE_800x600RGB]      =   DC1394_VIDEO_MODE_800x600_RGB8;
        map[VIDEOMODE_800x600Y8]       =   DC1394_VIDEO_MODE_800x600_MONO8;
        map[VIDEOMODE_800x600Y16]      =   DC1394_VIDEO_MODE_800x600_MONO16;
        map[VIDEOMODE_1024x768YUV422]  =   DC1394_VIDEO_MODE_1024x768_YUV422;
        map[VIDEOMODE_1024x768RGB]     =   DC1394_VIDEO_MODE_1024x768_RGB8;
        map[VIDEOMODE_1024x768Y8]      =   DC1394_VIDEO_MODE_1024x768_MONO8;
        map[VIDEOMODE_1024x768Y16]     =   DC1394_VIDEO_MODE_1024x768_MONO16;
        map[VIDEOMODE_1280x960YUV422]  =   DC1394_VIDEO_MODE_1280x960_YUV422;
        map[VIDEOMODE_1280x960RGB]     =   DC1394_VIDEO_MODE_1280x960_RGB8;
        map[VIDEOMODE_1280x960Y8]      =   DC1394_VIDEO_MODE_1280x960_MONO8;
        map[VIDEOMODE_1280x960Y16]     =   DC1394_VIDEO_MODE_1280x960_MONO16;
        map[VIDEOMODE_1600x1200YUV422] =   DC1394_VIDEO_MODE_1600x1200_YUV422;
        map[VIDEOMODE_1600x1200RGB]    =   DC1394_VIDEO_MODE_1600x1200_RGB8;
        map[VIDEOMODE_1600x1200Y8]     =   DC1394_VIDEO_MODE_1600x1200_MONO8;
        map[VIDEOMODE_1600x1200Y16]    =   DC1394_VIDEO_MODE_1600x1200_MONO16;
        return map;
    }

    static std::map<VideoMode, dc1394video_mode_t> videoModeMap_to_dc1394 = 
        createVideoModeMap_to_dc1394();

    
    dc1394video_mode_t convertVideoMode_to_dc1394(VideoMode vidMode, ImageMode imgMode)
    {
        dc1394video_mode_t vidMode_dc1394;
        if (vidMode == VIDEOMODE_FORMAT7) 
        {
            if (imgMode < NUMBER_OF_IMAGEMODE)
            {   
                switch (imgMode)
                {
                    case IMAGEMODE_0:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_0;

                    case IMAGEMODE_1:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_1;

                    case IMAGEMODE_2:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_2;

                    case IMAGEMODE_3:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_3;

                    case IMAGEMODE_4:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_4;

                    case IMAGEMODE_5:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_5;

                    case IMAGEMODE_6:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_6;

                    case IMAGEMODE_7:
                        vidMode_dc1394 = DC1394_VIDEO_MODE_FORMAT7_7;
                }
            }
            else
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to convert videomode to libdc1394 videomode";
                throw RuntimeError(ERROR_DC1394_CONVERT_VIDEOMODE, ssError.str());
            }
        } 
        else
        {
            if (videoModeMap_to_dc1394.count(vidMode) != 0)
            { 
                vidMode_dc1394 = videoModeMap_to_dc1394[vidMode];
            }
            else
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to convert videomode to libdc1394 videomode";
                throw RuntimeError(ERROR_DC1394_CONVERT_VIDEOMODE, ssError.str());
            }
        }
        return vidMode_dc1394;
    }

    static std::map<FrameRate, dc1394framerate_t> createFrameRateMap_to_dc1394()
    {
        std::map<FrameRate, dc1394framerate_t> map;
        map[FRAMERATE_1_875]    = DC1394_FRAMERATE_1_875;
        map[FRAMERATE_3_75]     = DC1394_FRAMERATE_3_75;
        map[FRAMERATE_7_5]      = DC1394_FRAMERATE_7_5;
        map[FRAMERATE_15]       = DC1394_FRAMERATE_15;
        map[FRAMERATE_30]       = DC1394_FRAMERATE_30;
        map[FRAMERATE_60]       = DC1394_FRAMERATE_60;
        map[FRAMERATE_120]      = DC1394_FRAMERATE_120;
        map[FRAMERATE_240]      = DC1394_FRAMERATE_240;
        map[FRAMERATE_FORMAT7]  = dc1394framerate_t(DC1394_FRAMERATE_MAX + 1); 
        // Note, Format7 does not exist at value in libdc1394 so wer are using max value + 1.
        return map;
    }
    static std::map<FrameRate,dc1394framerate_t> frameRateMap_to_dc1394 = 
        createFrameRateMap_to_dc1394();

    dc1394framerate_t convertFrameRate_to_dc1394(FrameRate frmRate)
    {
        if (frameRateMap_to_dc1394.count(frmRate) == 0)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert framerate to dc1394 framerate";
            throw RuntimeError(ERROR_DC1394_CONVERT_FRAMERATE, ssError.str());
        }
        return frameRateMap_to_dc1394[frmRate];
    }


    // Conversion from libdc1394 types to BIAS types
    // ------------------------------------------------------------------------
    //
    static std::map<dc1394feature_t, PropertyType> createPropertyTypeMap_from_dc1394()
    {
        std::map<dc1394feature_t, PropertyType> map;
        map[DC1394_FEATURE_BRIGHTNESS]     =  PROPERTY_TYPE_BRIGHTNESS;
        map[DC1394_FEATURE_EXPOSURE]       =  PROPERTY_TYPE_AUTO_EXPOSURE;
        map[DC1394_FEATURE_SHARPNESS]      =  PROPERTY_TYPE_SHARPNESS;
        map[DC1394_FEATURE_WHITE_BALANCE]  =  PROPERTY_TYPE_WHITE_BALANCE;
        map[DC1394_FEATURE_HUE]            =  PROPERTY_TYPE_HUE;
        map[DC1394_FEATURE_SATURATION]     =  PROPERTY_TYPE_SATURATION;
        map[DC1394_FEATURE_GAMMA]          =  PROPERTY_TYPE_GAMMA;
        map[DC1394_FEATURE_IRIS]           =  PROPERTY_TYPE_IRIS;
        map[DC1394_FEATURE_FOCUS]          =  PROPERTY_TYPE_FOCUS;
        map[DC1394_FEATURE_ZOOM]           =  PROPERTY_TYPE_ZOOM;
        map[DC1394_FEATURE_PAN]            =  PROPERTY_TYPE_PAN;
        map[DC1394_FEATURE_TILT]           =  PROPERTY_TYPE_TILT;
        map[DC1394_FEATURE_SHUTTER]        =  PROPERTY_TYPE_SHUTTER;
        map[DC1394_FEATURE_GAIN]           =  PROPERTY_TYPE_GAIN;
        map[DC1394_FEATURE_TRIGGER]        =  PROPERTY_TYPE_TRIGGER_MODE;
        map[DC1394_FEATURE_TRIGGER_DELAY]  =  PROPERTY_TYPE_TRIGGER_DELAY;
        map[DC1394_FEATURE_FRAME_RATE]     =  PROPERTY_TYPE_FRAME_RATE;
        map[DC1394_FEATURE_TEMPERATURE]    =  PROPERTY_TYPE_TEMPERATURE;
        return map;
    }

    static std::map<dc1394feature_t, PropertyType> propertyTypeMap_from_dc1394 = 
        createPropertyTypeMap_from_dc1394();

    PropertyType convertPropertyType_from_dc1394(dc1394feature_t feature)
    {
        PropertyType propertyType;
        if (propertyTypeMap_from_dc1394.count(feature) == 0)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert libdc1394 feature to propertyType";
            throw RuntimeError(ERROR_DC1394_CONVERT_PROPERTY_TYPE, ssError.str());
        }
        return propertyTypeMap_from_dc1394[feature];
    };


    static std::map<dc1394video_mode_t, VideoMode> createVideoModeMap_from_dc1394()
    {
        std::map<dc1394video_mode_t, VideoMode> map;
        map[DC1394_VIDEO_MODE_160x120_YUV444]    =   VIDEOMODE_160x120YUV444;
        map[DC1394_VIDEO_MODE_320x240_YUV422]    =   VIDEOMODE_320x240YUV422;
        map[DC1394_VIDEO_MODE_640x480_YUV411]    =   VIDEOMODE_640x480YUV411;
        map[DC1394_VIDEO_MODE_640x480_YUV422]    =   VIDEOMODE_640x480YUV422;
        map[DC1394_VIDEO_MODE_640x480_RGB8]      =   VIDEOMODE_640x480RGB;
        map[DC1394_VIDEO_MODE_640x480_MONO8]     =   VIDEOMODE_640x480Y8;
        map[DC1394_VIDEO_MODE_640x480_MONO16]    =   VIDEOMODE_640x480Y16;
        map[DC1394_VIDEO_MODE_800x600_YUV422]    =   VIDEOMODE_800x600YUV422;
        map[DC1394_VIDEO_MODE_800x600_RGB8]      =   VIDEOMODE_800x600RGB;
        map[DC1394_VIDEO_MODE_800x600_MONO8]     =   VIDEOMODE_800x600Y8;
        map[DC1394_VIDEO_MODE_800x600_MONO16]    =   VIDEOMODE_800x600Y16;
        map[DC1394_VIDEO_MODE_1024x768_YUV422]   =   VIDEOMODE_1024x768YUV422;
        map[DC1394_VIDEO_MODE_1024x768_RGB8]     =   VIDEOMODE_1024x768RGB;
        map[DC1394_VIDEO_MODE_1024x768_MONO8]    =   VIDEOMODE_1024x768Y8;
        map[DC1394_VIDEO_MODE_1024x768_MONO16]   =   VIDEOMODE_1024x768Y16;
        map[DC1394_VIDEO_MODE_1280x960_YUV422]   =   VIDEOMODE_1280x960YUV422;
        map[DC1394_VIDEO_MODE_1280x960_RGB8]     =   VIDEOMODE_1280x960RGB;
        map[DC1394_VIDEO_MODE_1280x960_MONO8]    =   VIDEOMODE_1280x960Y8;
        map[DC1394_VIDEO_MODE_1280x960_MONO16]   =   VIDEOMODE_1280x960Y16;
        map[DC1394_VIDEO_MODE_1600x1200_YUV422]  =   VIDEOMODE_1600x1200YUV422;
        map[DC1394_VIDEO_MODE_1600x1200_RGB8]    =   VIDEOMODE_1600x1200RGB;
        map[DC1394_VIDEO_MODE_1600x1200_MONO8]   =   VIDEOMODE_1600x1200Y8;
        map[DC1394_VIDEO_MODE_1600x1200_MONO16]  =   VIDEOMODE_1600x1200Y16;
        map[DC1394_VIDEO_MODE_FORMAT7_0]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_1]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_2]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_3]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_4]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_5]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_6]         =   VIDEOMODE_FORMAT7;
        map[DC1394_VIDEO_MODE_FORMAT7_7]         =   VIDEOMODE_FORMAT7;
        return map;
    }

    static std::map<dc1394video_mode_t, VideoMode> videoModeMap_from_dc1394 = 
        createVideoModeMap_from_dc1394();

    VideoMode convertVideoMode_from_dc1394(dc1394video_mode_t vidMode_dc1394)
    {
        if (videoModeMap_from_dc1394.count(vidMode_dc1394) == 0)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert libdc1394 videomode";
            throw RuntimeError(ERROR_DC1394_CONVERT_VIDEOMODE, ssError.str());
        }
        return videoModeMap_from_dc1394[vidMode_dc1394];
    }

    static std::map<dc1394framerate_t, FrameRate> createFrameRateMap_from_dc1394()
    {
        std::map<dc1394framerate_t, FrameRate> map;
        map[DC1394_FRAMERATE_1_875]    = FRAMERATE_1_875;
        map[DC1394_FRAMERATE_3_75]     = FRAMERATE_3_75;
        map[DC1394_FRAMERATE_7_5]      = FRAMERATE_7_5;
        map[DC1394_FRAMERATE_15]       = FRAMERATE_15;
        map[DC1394_FRAMERATE_30]       = FRAMERATE_30;
        map[DC1394_FRAMERATE_60]       = FRAMERATE_60;
        map[DC1394_FRAMERATE_120]      = FRAMERATE_120;
        map[DC1394_FRAMERATE_240]      = FRAMERATE_240;
        map[dc1394framerate_t(DC1394_FRAMERATE_MAX + 1)] = FRAMERATE_FORMAT7; 
        return map;
    }

    static std::map<dc1394framerate_t, FrameRate> frameRateMap_from_dc1394 = 
        createFrameRateMap_from_dc1394();

    FrameRate convertFrameRate_from_dc1394(dc1394framerate_t frmRate_dc1394)
    {
        if (frameRateMap_from_dc1394.count(frmRate_dc1394) == 0)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert libdc1394 framerate";
            throw RuntimeError(ERROR_DC1394_CONVERT_FRAMERATE, ssError.str());
        }
        return frameRateMap_from_dc1394[frmRate_dc1394];
    }

    ImageMode convertImageMode_from_dc1394(dc1394video_mode_t vidMode)
    {
        ImageMode imageMode = IMAGEMODE_UNSPECIFIED;

        switch (vidMode)
        { 
            case DC1394_VIDEO_MODE_FORMAT7_0:
                imageMode = IMAGEMODE_0;
                break; 

            case DC1394_VIDEO_MODE_FORMAT7_1:
                imageMode = IMAGEMODE_1;
                break; 

            case DC1394_VIDEO_MODE_FORMAT7_2:
                imageMode = IMAGEMODE_2;
                break; 

            case DC1394_VIDEO_MODE_FORMAT7_3:
                imageMode = IMAGEMODE_3;
                break; 

            case DC1394_VIDEO_MODE_FORMAT7_4:
                imageMode = IMAGEMODE_4;
                break;

            case DC1394_VIDEO_MODE_FORMAT7_5:
                imageMode = IMAGEMODE_5;
                break;

            case DC1394_VIDEO_MODE_FORMAT7_6:
                imageMode = IMAGEMODE_6;

            case DC1394_VIDEO_MODE_FORMAT7_7:
                imageMode = IMAGEMODE_7;
                break;

            default:
                break;
            }
        return imageMode;
    }

    // Print functions for libdc1394 configurations, settings and info
    //-------------------------------------------------------------------------
    //
    void printVideoModes_dc1394(dc1394video_modes_t modes_dc1394)
    {  
        for (int i=0; i<modes_dc1394.num; i++)
        {
            std::cout << "i: " << i << ", mode: "; 
            std::cout << getVideoModeString_dc1394(modes_dc1394.modes[i]) << std::endl;
        }
    }

    // libdc1394 enumeration to string converstions
    // ------------------------------------------------------------------------
    static std::map<dc1394video_mode_t, std::string> createVideoModeToStringMap_dc1394()
    {
        std::map<dc1394video_mode_t, std::string> map;
        map[DC1394_VIDEO_MODE_160x120_YUV444]    = std::string("DC1394_VIDEO_MODE_160x120_YUV444");
        map[DC1394_VIDEO_MODE_320x240_YUV422]    = std::string("DC1394_VIDEO_MODE_320x240_YUV422");  
        map[DC1394_VIDEO_MODE_640x480_YUV411]    = std::string("DC1394_VIDEO_MODE_640x480_YUV411");  
        map[DC1394_VIDEO_MODE_640x480_YUV422]    = std::string("DC1394_VIDEO_MODE_640x480_YUV422");  
        map[DC1394_VIDEO_MODE_640x480_RGB8]      = std::string("DC1394_VIDEO_MODE_640x480_RGB8");  
        map[DC1394_VIDEO_MODE_640x480_MONO8]     = std::string("DC1394_VIDEO_MODE_640x480_MONO8");  
        map[DC1394_VIDEO_MODE_640x480_MONO16]    = std::string("DC1394_VIDEO_MODE_640x480_MONO16");  
        map[DC1394_VIDEO_MODE_800x600_YUV422]    = std::string("DC1394_VIDEO_MODE_800x600_YUV422");  
        map[DC1394_VIDEO_MODE_800x600_RGB8]      = std::string("DC1394_VIDEO_MODE_800x600_RGB8");  
        map[DC1394_VIDEO_MODE_800x600_MONO8]     = std::string("DC1394_VIDEO_MODE_800x600_MONO8");  
        map[DC1394_VIDEO_MODE_800x600_MONO16]    = std::string("DC1394_VIDEO_MODE_800x600_MONO16");  
        map[DC1394_VIDEO_MODE_1024x768_YUV422]   = std::string("DC1394_VIDEO_MODE_1024x768_YUV422");  
        map[DC1394_VIDEO_MODE_1024x768_RGB8]     = std::string("DC1394_VIDEO_MODE_1024x768_RGB8");  
        map[DC1394_VIDEO_MODE_1024x768_MONO8]    = std::string("DC1394_VIDEO_MODE_1024x768_MONO8");  
        map[DC1394_VIDEO_MODE_1024x768_MONO16]   = std::string("DC1394_VIDEO_MODE_1024x768_MONO16");  
        map[DC1394_VIDEO_MODE_1280x960_YUV422]   = std::string("DC1394_VIDEO_MODE_1280x960_YUV422");  
        map[DC1394_VIDEO_MODE_1280x960_RGB8]     = std::string("DC1394_VIDEO_MODE_1280x960_RGB8");  
        map[DC1394_VIDEO_MODE_1280x960_MONO8]    = std::string("DC1394_VIDEO_MODE_1280x960_MONO8");  
        map[DC1394_VIDEO_MODE_1280x960_MONO16]   = std::string("DC1394_VIDEO_MODE_1280x960_MONO16");  
        map[DC1394_VIDEO_MODE_1600x1200_YUV422]  = std::string("DC1394_VIDEO_MODE_1600x1200_YUV422");  
        map[DC1394_VIDEO_MODE_1600x1200_RGB8]    = std::string("DC1394_VIDEO_MODE_1600x1200_RGB8");   
        map[DC1394_VIDEO_MODE_1600x1200_MONO8]   = std::string("DC1394_VIDEO_MODE_1600x1200_MONO8");   
        map[DC1394_VIDEO_MODE_1600x1200_MONO16]  = std::string("DC1394_VIDEO_MODE_1600x1200_MONO16");   
        map[DC1394_VIDEO_MODE_FORMAT7_0]         = std::string("DC1394_VIDEO_MODE_FORMAT7_0");   
        map[DC1394_VIDEO_MODE_FORMAT7_1]         = std::string("DC1394_VIDEO_MODE_FORMAT7_1");   
        map[DC1394_VIDEO_MODE_FORMAT7_2]         = std::string("DC1394_VIDEO_MODE_FORMAT7_2");   
        map[DC1394_VIDEO_MODE_FORMAT7_3]         = std::string("DC1394_VIDEO_MODE_FORMAT7_3");   
        map[DC1394_VIDEO_MODE_FORMAT7_4]         = std::string("DC1394_VIDEO_MODE_FORMAT7_4");   
        map[DC1394_VIDEO_MODE_FORMAT7_5]         = std::string("DC1394_VIDEO_MODE_FORMAT7_5");   
        map[DC1394_VIDEO_MODE_FORMAT7_6]         = std::string("DC1394_VIDEO_MODE_FORMAT7_6");   
        map[DC1394_VIDEO_MODE_FORMAT7_7]         = std::string("DC1394_VIDEO_MODE_FORMAT7_7");   
        return map;
    }

    static std::map<dc1394video_mode_t, std::string> videoModeToStringMap_dc1394 = 
        createVideoModeToStringMap_dc1394();


    std::string getVideoModeString_dc1394(dc1394video_mode_t vidMode_dc1394)
    {
        if (videoModeToStringMap_dc1394.count(vidMode_dc1394) != 0)
        {
            return videoModeToStringMap_dc1394[vidMode_dc1394];
        }
        else
        {
            return std::string("unknown dc1394 video mode");
        }
    }


    static std::map<dc1394framerate_t, std::string> createFrameRateToStringMap_dc1394()
    {
        std::map<dc1394framerate_t, std::string> map;
        map[DC1394_FRAMERATE_1_875]    = std::string("DC1394_FRAMERATE_1_875");
        map[DC1394_FRAMERATE_3_75]     = std::string("DC1394_FRAMERATE_3_75");
        map[DC1394_FRAMERATE_7_5]      = std::string("DC1394_FRAMERATE_7_5");
        map[DC1394_FRAMERATE_15]       = std::string("DC1394_FRAMERATE_15");
        map[DC1394_FRAMERATE_30]       = std::string("DC1394_FRAMERATE_30");
        map[DC1394_FRAMERATE_60]       = std::string("DC1394_FRAMERATE_60");
        map[DC1394_FRAMERATE_120]      = std::string("DC1394_FRAMERATE_120");
        map[DC1394_FRAMERATE_240]      = std::string("DC1394_FRAMERATE_240");
        map[dc1394framerate_t(DC1394_FRAMERATE_MAX + 1)] = std::string("DC1394_FRAMERATE_FORMAT7"); 
        return map;
    }
    static std::map<dc1394framerate_t, std::string> frameRateToStringMap_dc1394 = 
        createFrameRateToStringMap_dc1394();


    std::string getFrameRateString_dc1394(dc1394framerate_t frmRate_dc1394)
    {
        if (frameRateToStringMap_dc1394.count(frmRate_dc1394) != 0)
        {
            return frameRateToStringMap_dc1394[frmRate_dc1394];
        }
        else
        {
            return std::string("unknown dc1394 frame rate");
        }
    }



    static std::map<dc1394color_coding_t, std::string> createColorCodingToStringMap_dc1394()
    {
        std::map<dc1394color_coding_t, std::string> map;
        map[DC1394_COLOR_CODING_MONO8]    = std::string("DC1394_COLOR_CODING_MONO8");
        map[DC1394_COLOR_CODING_YUV411]   = std::string("DC1394_COLOR_CODING_YUV411");
        map[DC1394_COLOR_CODING_YUV422]   = std::string("DC1394_COLOR_CODING_YUV422");
        map[DC1394_COLOR_CODING_YUV444]   = std::string("DC1394_COLOR_CODING_YUV444");
        map[DC1394_COLOR_CODING_RGB8]     = std::string("DC1394_COLOR_CODING_RGB8");
        map[DC1394_COLOR_CODING_MONO16]   = std::string("DC1394_COLOR_CODING_MONO16");
        map[DC1394_COLOR_CODING_RGB16]    = std::string("DC1394_COLOR_CODING_RGB16");
        map[DC1394_COLOR_CODING_MONO16S]  = std::string("DC1394_COLOR_CODING_MONO16S");
        map[DC1394_COLOR_CODING_RGB16S]   = std::string("DC1394_COLOR_CODING_RGB16S");
        map[DC1394_COLOR_CODING_RAW8]     = std::string("DC1394_COLOR_CODING_RAW8");
        map[DC1394_COLOR_CODING_RAW16]    = std::string("DC1394_COLOR_CODING_RAW16");
        return map;
    }
    static std::map<dc1394color_coding_t, std::string> colorCodingToStringMap_dc1394 = 
        createColorCodingToStringMap_dc1394();


    std::string getColorCodingString_dc1394(dc1394color_coding_t colorCoding_dc1394)
    {
        if (colorCodingToStringMap_dc1394.count(colorCoding_dc1394) != 0)
        {
            return colorCodingToStringMap_dc1394[colorCoding_dc1394];
        }
        else
        {
            return std::string("unknown dc1394 color coding");
        }
    }


    static std::map<dc1394feature_t, std::string> createFeatureToStringMap_dc1394()
    {
        std::map<dc1394feature_t, std::string> map;
        map[DC1394_FEATURE_BRIGHTNESS]     =  std::string("DC1394_FEATURE_BRIGHTNESS");
        map[DC1394_FEATURE_EXPOSURE]       =  std::string("DC1394_FEATURE_EXPOSURE");
        map[DC1394_FEATURE_SHARPNESS]      =  std::string("DC1394_FEATURE_SHARPNESS");
        map[DC1394_FEATURE_WHITE_BALANCE]  =  std::string("DC1394_FEATURE_WHITE_BALANCE");
        map[DC1394_FEATURE_HUE]            =  std::string("DC1394_FEATURE_HUE");
        map[DC1394_FEATURE_SATURATION]     =  std::string("DC1394_FEATURE_SATURATION");
        map[DC1394_FEATURE_GAMMA]          =  std::string("DC1394_FEATURE_GAMMA");
        map[DC1394_FEATURE_IRIS]           =  std::string("DC1394_FEATURE_IRIS");
        map[DC1394_FEATURE_FOCUS]          =  std::string("DC1394_FEATURE_FOCUS");
        map[DC1394_FEATURE_ZOOM]           =  std::string("DC1394_FEATURE_ZOOM");
        map[DC1394_FEATURE_PAN]            =  std::string("DC1394_FEATURE_PAN");
        map[DC1394_FEATURE_TILT]           =  std::string("DC1394_FEATURE_TILT");
        map[DC1394_FEATURE_SHUTTER]        =  std::string("DC1394_FEATURE_SHUTTER");
        map[DC1394_FEATURE_GAIN]           =  std::string("DC1394_FEATURE_GAIN");
        map[DC1394_FEATURE_TRIGGER]        =  std::string("DC1394_FEATURE_TRIGGER");
        map[DC1394_FEATURE_TRIGGER_DELAY]  =  std::string("DC1394_FEATURE_TRIGGER_DELAY");
        map[DC1394_FEATURE_FRAME_RATE]     =  std::string("DC1394_FEATURE_FRAME_RATE");
        map[DC1394_FEATURE_TEMPERATURE]    =  std::string("DC1394_FEATURE_TEMPERATURE");
        return map;
    }
    static std::map<dc1394feature_t, std::string> featureToStringMap_dc1394 = 
        createFeatureToStringMap_dc1394();


    std::string getFeatureString_dc1394(dc1394feature_t feature_dc1394)
    {
        if (featureToStringMap_dc1394.count(feature_dc1394) != 0)
        {
            return featureToStringMap_dc1394[feature_dc1394];
        }
        else
        {
            return std::string("unknown dc1394 feature");
        }
    }


    std::string getFeatureInfoString_dc1394(const dc1394feature_info_t &featureInfo_dc1394)
    {
        std::stringstream ss;
        ss << "id:                 " << getFeatureString_dc1394(featureInfo_dc1394.id) << std::endl;
        ss << "available:          " << getBoolString_dc1394(featureInfo_dc1394.available) << std::endl;
        ss << "absolute_capable:   " << getBoolString_dc1394(featureInfo_dc1394.absolute_capable) << std::endl;
        ss << "readout_capable:    " << getBoolString_dc1394(featureInfo_dc1394.readout_capable) << std::endl;
        ss << "on_off_capable:     " << getBoolString_dc1394(featureInfo_dc1394.on_off_capable) << std::endl;
        ss << "polarity_capable:   " << getBoolString_dc1394(featureInfo_dc1394.polarity_capable) << std::endl;
        ss << "is_on:              " << getSwitchString_dc1394(featureInfo_dc1394.is_on) << std::endl;
        ss << "feature_mode:       " << getFeatureModeString_dc1394(featureInfo_dc1394.current_mode) << std::endl;
        ss << "feature_modes:      " << getFeatureModesString_dc1394(featureInfo_dc1394.modes) << std::endl;
        return ss.str();
    }


    std::string getBoolString_dc1394(dc1394bool_t value)
    {
        std::string valueStr;
        switch (value)
        {
            case DC1394_TRUE:
                valueStr =  std::string("true");
                break;

            case DC1394_FALSE:
                valueStr = std::string("false");
                break;

            default:
                valueStr = std::string("unknown");
                break;
        }
        return valueStr;
    }


    std::string getSwitchString_dc1394(dc1394switch_t value)
    {
        std::string valueStr;
        switch (value)
        {
            case DC1394_ON: 
                valueStr = std::string("on");
                break;

            case  DC1394_OFF:
                valueStr = std::string("off");
                break;

            default:
                valueStr = std::string("unknown");
                break;
        }
        return valueStr;
    }



    std::string getFeatureModeString_dc1394(dc1394feature_mode_t value)
    {
        std::string valueStr;
        switch(value)
        {
            case DC1394_FEATURE_MODE_MANUAL:
                valueStr = std::string("manual");
                break;

            case DC1394_FEATURE_MODE_AUTO:
                valueStr = std::string("auto");
                break;

            default:
                valueStr = std::string("uknown");
                break;
        }
        return valueStr;
    }


    std::string getFeatureModesString_dc1394(dc1394feature_modes_t featureModes_dc1394)
    {
        std::stringstream ss;
        ss << "[";
        for (int i=0; i<featureModes_dc1394.num; i++)
        {
            ss << getFeatureModeString_dc1394(featureModes_dc1394.modes[i]);
            if (i < featureModes_dc1394.num - 1)
            {
                ss << ", ";
            }
        }
        ss << "]";
        return ss.str();
    }

    
} // namespace bias

  
