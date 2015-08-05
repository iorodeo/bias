#include "utils.hpp"
#include "exception.hpp"
#include <map>
#include <sstream>
#include <iostream>

namespace bias
{
    // Functions which return lists of enumeration types
    // ------------------------------------------------------------------------
    
    VideoModeList getListOfVideoModes() 
    {
        VideoModeList list;
        for (int i=0; i < int(NUMBER_OF_VIDEOMODE); i++) 
        {
            list.push_back(VideoMode(i));
        }
        return list;
    }

    FrameRateList getListOfFrameRates()
    {
        FrameRateList list;
        for (int i=0; i < int(NUMBER_OF_FRAMERATE); i++) 
        {
            list.push_back(FrameRate(i));
        }
        return list;
    }

    ImageModeList getListOfImageModes()
    {
        ImageModeList list;
        for (int i=0; i < int(NUMBER_OF_IMAGEMODE); i++)
        {
            list.push_back(ImageMode(i));
        }
        return list;
    }

    ImageModeList getListOfImageModes(unsigned int number)
    {
        ImageModeList list;
        for (int i=0; i < number; i++)
        {
            list.push_back(ImageMode(i));
        }
        return list;
    }

    PropertyTypeList getListOfPropertyTypes()
    {
        PropertyTypeList list;
        for (int i=0; i < int(NUMBER_OF_PROPERTY_TYPE); i++)
        {
            list.push_back(PropertyType(i));
        }
        return list;
    }

    PixelFormatList getListOfPixelFormats()
    {
        PixelFormatList list;
        for (int i=0; i< int(NUMBER_OF_PIXEL_FORMAT); i++)
        {
            list.push_back(PixelFormat(i));
        }
        return list;
    }

    TriggerTypeList getListOfTriggerTypes() 
    {
        TriggerTypeList list;
        for (int i=0; i< int(NUMBER_OF_TRIGGER_TYPE); i++)
        {
            list.push_back(TriggerType(i));
        }
        return list;
    }

    // Functions for converting enumerations to strings
    // ------------------------------------------------------------------------

    static std::map<VideoMode, std::string> createVideoModeToStringMap()
    {
        std::map<VideoMode, std::string> map;
        map[VIDEOMODE_160x120YUV444]    =   std::string("160x120YUV444");
        map[VIDEOMODE_320x240YUV422]    =   std::string("320x240YUV422");
        map[VIDEOMODE_640x480YUV411]    =   std::string("640x480YUV411");
        map[VIDEOMODE_640x480YUV422]    =   std::string("640x480YUV422");
        map[VIDEOMODE_640x480RGB]       =   std::string("640x480RGB");
        map[VIDEOMODE_640x480Y8]        =   std::string("640x480Y8");
        map[VIDEOMODE_640x480Y16]       =   std::string("640x480Y16");
        map[VIDEOMODE_800x600YUV422]    =   std::string("800x600YUV422");
        map[VIDEOMODE_800x600RGB]       =   std::string("800x600RGB");
        map[VIDEOMODE_800x600Y8]        =   std::string("800x600Y8");
        map[VIDEOMODE_800x600Y16]       =   std::string("800x600Y16");
        map[VIDEOMODE_1024x768YUV422]   =   std::string("1024x768YUV422");
        map[VIDEOMODE_1024x768RGB]      =   std::string("1024x768RGB");
        map[VIDEOMODE_1024x768Y8]       =   std::string("1024x768Y8");
        map[VIDEOMODE_1024x768Y16]      =   std::string("1024x768Y16");
        map[VIDEOMODE_1280x960YUV422]   =   std::string("1280x960YUV422");
        map[VIDEOMODE_1280x960RGB]      =   std::string("1280x960RGB");
        map[VIDEOMODE_1280x960Y8]       =   std::string("1280x960Y8");
        map[VIDEOMODE_1280x960Y16]      =   std::string("1280x960Y16");
        map[VIDEOMODE_1600x1200YUV422]  =   std::string("1600x1200YUV422");
        map[VIDEOMODE_1600x1200RGB]     =   std::string("1600x1200RGB");
        map[VIDEOMODE_1600x1200Y8]      =   std::string("1600x1200Y8");
        map[VIDEOMODE_1600x1200Y16]     =   std::string("1600x1200Y16");
        map[VIDEOMODE_FORMAT7]          =   std::string("Format7");
        map[VIDEOMODE_UNSPECIFIED]      =   std::string("Unspecified");
        return map;
    };

    static std::map<VideoMode, std::string> videoModeToStringMap = 
        createVideoModeToStringMap();

    std::string getVideoModeString(VideoMode vidMode)
    {
        if (videoModeToStringMap.count(vidMode) != 0) 
        {
            return videoModeToStringMap[vidMode];
        }
        else 
        { 
            std::stringstream ssMsg;
            ssMsg << "uknown videoMode: " << vidMode;
            return ssMsg.str();
        }
    }

    static std::map<FrameRate, std::string> createFrameRateToStringMap()
    {
        std::map<FrameRate, std::string> map;

        map[FRAMERATE_1_875]        =   std::string("1.875");
        map[FRAMERATE_3_75]         =   std::string("3.75");
        map[FRAMERATE_7_5]          =   std::string("7.5");
        map[FRAMERATE_15]           =   std::string("15");
        map[FRAMERATE_30]           =   std::string("30");
        map[FRAMERATE_60]           =   std::string("60");
        map[FRAMERATE_120]          =   std::string("120");
        map[FRAMERATE_240]          =   std::string("240");
        map[FRAMERATE_FORMAT7]      =   std::string("Format7");
        map[FRAMERATE_UNSPECIFIED ] =   std::string("Unspecified");
        return map;
    };

    static std::map<FrameRate, std::string> frameRateToStringMap = 
        createFrameRateToStringMap();

    std::string getFrameRateString(FrameRate frmRate)
    {
        if (frameRateToStringMap.count(frmRate) != 0) 
        {
            return frameRateToStringMap[frmRate];
        }
        else
        { 
            std::stringstream ssMsg;
            ssMsg << "uknown frmRate: " << frmRate;
            return ssMsg.str();
        }
    }


    static std::map<PropertyType, std::string> createPropertyTypeToStringMap()
    {
        std::map<PropertyType, std::string> map;
        map[PROPERTY_TYPE_BRIGHTNESS]     =  std::string("Brightness");
        map[PROPERTY_TYPE_AUTO_EXPOSURE]  =  std::string("Auto Exposure");
        map[PROPERTY_TYPE_SHARPNESS]      =  std::string("Sharpness");
        map[PROPERTY_TYPE_WHITE_BALANCE]  =  std::string("White Balance");
        map[PROPERTY_TYPE_HUE]            =  std::string("Hue");
        map[PROPERTY_TYPE_SATURATION]     =  std::string("Saturation");
        map[PROPERTY_TYPE_GAMMA]          =  std::string("Gamma");
        map[PROPERTY_TYPE_IRIS]           =  std::string("Iris");
        map[PROPERTY_TYPE_FOCUS]          =  std::string("Focus");
        map[PROPERTY_TYPE_ZOOM]           =  std::string("Zoom");
        map[PROPERTY_TYPE_PAN]            =  std::string("Pan");
        map[PROPERTY_TYPE_TILT]           =  std::string("Tilt");
        map[PROPERTY_TYPE_SHUTTER]        =  std::string("Shutter");
        map[PROPERTY_TYPE_GAIN]           =  std::string("Gain");
        map[PROPERTY_TYPE_TRIGGER_MODE]   =  std::string("Trigger Mode");
        map[PROPERTY_TYPE_TRIGGER_DELAY]  =  std::string("Trigger Delay");
        map[PROPERTY_TYPE_FRAME_RATE]     =  std::string("Frame Rate");
        map[PROPERTY_TYPE_TEMPERATURE]    =  std::string("Temperature");
        map[PROPERTY_TYPE_UNSPECIFIED]    =  std::string("Unspecified");
        return map;
    }

    static std::map<PropertyType, std::string> propertyTypeToStringMap = 
        createPropertyTypeToStringMap();

    std::string getPropertyTypeString(PropertyType propType)
    {
        if (propertyTypeToStringMap.count(propType) != 0)
        {
            return propertyTypeToStringMap[propType];
        } 
        else
        { 
            std::stringstream ssMsg;
            ssMsg << ": unknown PropertyType " << propType;
            return ssMsg.str();
        }
    }

    static std::map<PixelFormat, std::string> createPixelFormatToStringMap()
    {
        std::map<PixelFormat, std::string> map;
        map[PIXEL_FORMAT_MONO8]         =   std::string("MONO8");
        map[PIXEL_FORMAT_411YUV8]       =   std::string("411YUV8");
        map[PIXEL_FORMAT_422YUV8]       =   std::string("422YUV8");
        map[PIXEL_FORMAT_444YUV8]       =   std::string("444YUV8");
        map[PIXEL_FORMAT_RGB8]          =   std::string("RGB8");
        map[PIXEL_FORMAT_MONO16]        =   std::string("MONO16");
        map[PIXEL_FORMAT_RGB16]         =   std::string("RGB16");
        map[PIXEL_FORMAT_S_MONO16]      =   std::string("S_MONO16");
        map[PIXEL_FORMAT_S_RGB16]       =   std::string("S_RGB16");
        map[PIXEL_FORMAT_RAW8]          =   std::string("RAW8");
        map[PIXEL_FORMAT_RAW16]         =   std::string("RAW16");
        map[PIXEL_FORMAT_MONO12]        =   std::string("MONO12");
        map[PIXEL_FORMAT_RAW12]         =   std::string("RAW12");
        map[PIXEL_FORMAT_BGR8]          =   std::string("BGR8");
        map[PIXEL_FORMAT_BGRU]          =   std::string("BGRU");
        map[PIXEL_FORMAT_RGBU]          =   std::string("RGBU");
        map[PIXEL_FORMAT_BGR16]         =   std::string("BGR16");
        map[PIXEL_FORMAT_BGRU16]        =   std::string("BGRU16");
        map[PIXEL_FORMAT_422YUV8_JPEG]  =   std::string("422YUV8_JPEG");
        map[PIXEL_FORMAT_UNSPECIFIED]   =   std::string("Unspecified");
        return map;
    };

    static std::map<PixelFormat, std::string> pixelFormatToStringMap = 
        createPixelFormatToStringMap();

    std::string getPixelFormatString(PixelFormat pixFormat)
    {
        if (pixelFormatToStringMap.count(pixFormat) != 0) 
        {
            return pixelFormatToStringMap[pixFormat];
        }
        else 
        { 
            std::stringstream ssMsg;
            ssMsg << ": unknown PixelFormat " << pixFormat; 
            return ssMsg.str();
        }
    }

    std::string getImageInfoString(ImageInfo imgInfo)
    {
        std::stringstream ss;
        ss << "cols:         " << imgInfo.cols << std::endl;
        ss << "rows:         " << imgInfo.rows << std::endl;
        ss << "stride:       " << imgInfo.stride << std::endl;
        ss << "dataSize:     " << imgInfo.dataSize << std::endl;
        ss << "pixelFormat:  " << getPixelFormatString(imgInfo.pixelFormat);
        ss << std::endl;
        return ss.str();
    }

    std::string getTriggerTypeString(TriggerType trigType)
    {
        std::string trigTypeString;
        if (trigType == TRIGGER_INTERNAL)
        {
            trigTypeString = std::string("Internal");
        }
        else if (trigType == TRIGGER_EXTERNAL)
        {
            trigTypeString = std::string("External");
        }
        else
        {
            trigTypeString = std::string("Unspecified");
        }
        return trigTypeString;
    }

    static std::map<ImageMode, std::string> createImageModeToStringMap()
    {
        std::map<ImageMode, std::string> map;
        map[IMAGEMODE_0] = std::string("0");
        map[IMAGEMODE_1] = std::string("1");
        map[IMAGEMODE_2] = std::string("2");
        map[IMAGEMODE_3] = std::string("3");
        map[IMAGEMODE_4] = std::string("4");
        map[IMAGEMODE_5] = std::string("5");
        map[IMAGEMODE_6] = std::string("6");
        map[IMAGEMODE_7] = std::string("7");
        map[IMAGEMODE_8] = std::string("8");
        map[IMAGEMODE_9] = std::string("9");
        map[IMAGEMODE_10] = std::string("10");
        map[IMAGEMODE_11] = std::string("11");
        map[IMAGEMODE_12] = std::string("12");
        map[IMAGEMODE_13] = std::string("13");
        map[IMAGEMODE_14] = std::string("14");
        map[IMAGEMODE_15] = std::string("15");
        map[IMAGEMODE_16] = std::string("16");
        map[IMAGEMODE_17] = std::string("17");
        map[IMAGEMODE_18] = std::string("18");
        map[IMAGEMODE_19] = std::string("19");
        map[IMAGEMODE_20] = std::string("20");
        map[IMAGEMODE_21] = std::string("21");
        map[IMAGEMODE_22] = std::string("22");
        map[IMAGEMODE_23] = std::string("23");
        map[IMAGEMODE_24] = std::string("24");
        map[IMAGEMODE_25] = std::string("25");
        map[IMAGEMODE_26] = std::string("26");
        map[IMAGEMODE_27] = std::string("27");
        map[IMAGEMODE_28] = std::string("28");
        map[IMAGEMODE_29] = std::string("29");
        map[IMAGEMODE_30] = std::string("30");
        map[IMAGEMODE_31] = std::string("31");
        map[IMAGEMODE_UNSPECIFIED] = std::string("Unspecified");
        return map;
    };

    static std::map<ImageMode, std::string> imageModeToStringMap = 
        createImageModeToStringMap();

    std::string getImageModeString(ImageMode mode)
    {
        std::string imageModeString;

        if (mode < NUMBER_OF_IMAGEMODE) 
        {
            imageModeString = imageModeToStringMap[mode];
        }
        else 
        {
            imageModeString = imageModeToStringMap[IMAGEMODE_UNSPECIFIED];
        }
        return imageModeString;
    }

    // ------------------------------------------------------------------------
    
    static std::map<FrameRate, float> createFrameRateToFloatMap()
    {
        std::map<FrameRate, float> map;
        map[FRAMERATE_1_875] =   1.875;
        map[FRAMERATE_3_75]  =   3.75;
        map[FRAMERATE_7_5]   =   7.5;
        map[FRAMERATE_15]    =   15.0;
        map[FRAMERATE_30]    =   30.0;
        map[FRAMERATE_60]    =   60.0;
        map[FRAMERATE_120]   =   120.0;
        map[FRAMERATE_240]   =   240.0;
        return map;
    }

    static std::map<FrameRate, float> frameRateToFloatMap = 
        createFrameRateToFloatMap();

    float getFrameRateAsFloat(FrameRate frmRate)
    {
        std::cout << frmRate << std::endl;
        if (frameRateToFloatMap.count(frmRate)!=0)
        {
            return frameRateToFloatMap[frmRate];
        }
        else
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert FrameRate to float";
            throw RuntimeError(ERROR_INVALID_FRAMERATE, ssError.str());
        }
    }

} // namespase bias
