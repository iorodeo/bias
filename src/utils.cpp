#include "utils.hpp"
#include <sstream>

namespace bias
{
    // Functions which return lists of enumeration types
    // ------------------------------------------------------------------------
    
    VideoModeList getListOfVideoModes() 
    {
        VideoModeList list;
        for (int i=0; i<int(NUMBER_OF_VIDEOMODE); i++) 
        {
            list.push_back(VideoMode(i));
        }
        return list;
    }

    FrameRateList getListOfFrameRates()
    {
        FrameRateList list;
        for (int i=0; i<int(NUMBER_OF_FRAMERATE); i++) 
        {
            list.push_back(FrameRate(i));
        }
        return list;
    }

    ImageModeList getListOfImageModes()
    {
        ImageModeList list;
        for (int i=0; i< int(NUMBER_OF_IMAGEMODE); i++)
        {
            list.push_back(ImageMode(i));
        }
        return list;
    }

    PropertyTypeList getListOfPropertyTypes()
    {
        PropertyTypeList list;
        for (int i=0; i< int(NUMBER_OF_PROPERTY_TYPE); i++)
        {
            list.push_back(PropertyType(i));
        }
        return list;
    }

    // Functions for converting enumerations to strings
    // ------------------------------------------------------------------------

    std::string getVideoModeString(VideoMode videoMode)
    {
        switch (videoMode)
        {
            case VIDEOMODE_160x120YUV444: 
                return std::string("VIDEOMODE_160x120YUV444");
                break;

            case VIDEOMODE_320x240YUV422: 
                return std::string("VIDEOMODE_320x240YUV422");
                break;

            case VIDEOMODE_640x480YUV411: 
                return std::string("VIDEOMODE_640x480YUV411");
                break;

            case VIDEOMODE_640x480YUV422: 
                return std::string("VIDEOMODE_640x480YUV422");
                break;

            case VIDEOMODE_640x480RGB: 
                return std::string("VIDEOMODE_640x480RGB");
                break;

            case VIDEOMODE_640x480Y8: 
                return std::string("VIDEOMODE_640x480Y8");
                break;

            case VIDEOMODE_640x480Y16: 
                return std::string("VIDEOMODE_640x480Y16");
                break;

            case VIDEOMODE_800x600YUV422: 
                return std::string("VIDEOMODE_800x600YUV422");
                break;

            case VIDEOMODE_800x600RGB: 
                return std::string("VIDEOMODE_800x600RGB");
                break;

            case VIDEOMODE_800x600Y8: 
                return std::string("VIDEOMODE_800x600Y8");
                break;

            case VIDEOMODE_800x600Y16: 
                return std::string("VIDEOMODE_800x600Y16");
                break;

            case VIDEOMODE_1024x768YUV422: 
                return std::string("VIDEOMODE_1024x768YUV422");
                break;

            case VIDEOMODE_1024x768RGB: 
                return std::string("VIDEOMODE_1024x768RGB");
                break;

            case VIDEOMODE_1024x768Y8: 
                return std::string("VIDEOMODE_1024x768Y8");
                break;

            case VIDEOMODE_1024x768Y16: 
                return std::string("VIDEOMODE_1024x768Y16");
                break;

            case VIDEOMODE_1280x960YUV422: 
                return std::string("VIDEOMODE_1280x960YUV422");
                break;

            case VIDEOMODE_1280x960RGB: 
                return std::string("VIDEOMODE_1280x960RGB");
                break;

            case VIDEOMODE_1280x960Y8: 
                return std::string("VIDEOMODE_1280x960Y8");
                break;

            case VIDEOMODE_1280x960Y16: 
                return std::string("VIDEOMODE_1280x960Y16");
                break;

            case VIDEOMODE_1600x1200YUV422: 
                return std::string("VIDEOMODE_1600x1200YUV422");
                break;

            case VIDEOMODE_1600x1200RGB: 
                return std::string("VIDEOMODE_1600x1200RGB");
                break;

            case VIDEOMODE_1600x1200Y8: 
                return std::string("VIDEOMODE_1600x1200Y8");
                break;

            case VIDEOMODE_1600x1200Y16: 
                return std::string("VIDEOMODE_1600x1200Y16");
                break;

            case VIDEOMODE_FORMAT7: 
                return std::string("VIDEOMODE_FORMAT7");
                break;

            case VIDEOMODE_UNSPECIFIED:
                return std::string("VIDEOMODE_UNSPECIFIED");
                break;
                
            default:
                {
                    std::stringstream ssMsg;
                    ssMsg << "uknown videoMode: " << videoMode;
                    return ssMsg.str();
                }
                break;
        }
        return std::string("error: getVideoModeString()");
    }

    std::string getFrameRateString(FrameRate frameRate)
    {
        switch (frameRate)
        {
            case FRAMERATE_1_875: 
                return std::string("FRAMERATE_1_875");
                break;

            case FRAMERATE_3_75: 
                return std::string("FRAMERATE_3_75");
                break;

            case FRAMERATE_7_5: 
                return std::string("FRAMERATE_7_5");
                break;

            case FRAMERATE_15: 
                return std::string("FRAMERATE_15");
                break;

            case FRAMERATE_30: 
                return std::string("FRAMERATE_30");
                break;

            case FRAMERATE_60: 
                return std::string("FRAMERATE_60");
                break;

            case FRAMERATE_120: 
                return std::string("FRAMERATE_120");
                break;

            case FRAMERATE_240: 
                return std::string("FRAMERATE_240");
                break;

            case FRAMERATE_FORMAT7: 
                return std::string("FRAMERATE_FORMAT7");
                break;

            case FRAMERATE_UNSPECIFIED:
                return std::string("FRAMERATE_UNSPECIFIED");
                break;

            default:
                {
                    std::stringstream ssMsg;
                    ssMsg << "uknown frameRate: " << frameRate;
                    return ssMsg.str();
                }
                break;
        }
        return std::string("error: getFrameRateString()");
    }

    std::string getPropertyTypeString(PropertyType propertyType)
    {
        switch(propertyType)
        {
            case PROPERTY_TYPE_BRIGHTNESS: 
                return std::string("PROPERTY_TYPE_BRIGHTNESS");
                break;

            case PROPERTY_TYPE_AUTO_EXPOSURE:
                return std::string("PROPERTY_TYPE_AUTO_EXPOSURE");
                break;

            case PROPERTY_TYPE_SHARPNESS:
                return std::string("PROPERTY_TYPE_SHARPNESS");
                break;

            case PROPERTY_TYPE_WHITE_BALANCE:
                return std::string("PROPERTY_TYPE_WHITE_BALANCE");
                break;

            case PROPERTY_TYPE_HUE:
                return std::string("PROPERTY_TYPE_HUE");
                break;

            case PROPERTY_TYPE_SATURATION:
                return std::string("PROPERTY_TYPE_SATURATION");
                break;

            case PROPERTY_TYPE_GAMMA:
                return std::string("PROPERTY_TYPE_GAMMA");
                break;

            case PROPERTY_TYPE_IRIS:
                return std::string("PROPERTY_TYPE_IRIS");
                break;

            case PROPERTY_TYPE_FOCUS:
                return std::string("PROPERTY_TYPE_FOCUS");
                break;

            case PROPERTY_TYPE_ZOOM:
                return std::string("PROPERTY_TYPE_ZOOM");
                break;

            case PROPERTY_TYPE_PAN:
                return std::string("PROPERTY_TYPE_PAN");
                break;

            case PROPERTY_TYPE_TILT:
                return std::string("PROPERTY_TYPE_TILT");
                break;

            case PROPERTY_TYPE_SHUTTER:
                return std::string("PROPERTY_TYPE_SHUTTER");
                break;

            case PROPERTY_TYPE_GAIN:
                return std::string("PROPERTY_TYPE_GAIN");
                break;

            case PROPERTY_TYPE_TRIGGER_MODE:
                return std::string("PROPERTY_TYPE_TRIGGER_MODE");
                break;

            case PROPERTY_TYPE_TRIGGER_DELAY:
                return std::string("PROPERTY_TYPE_TRIGGER_DELAY");
                break;

            case PROPERTY_TYPE_FRAME_RATE:
                return std::string("PROPERTY_TYPE_FRAME_RATE");
                break;

            case PROPERTY_TYPE_TEMPERATURE:
                return std::string("PROPERTY_TYPE_TEMPERATURE");
                break;
            default:
                {
                    std::stringstream ssMsg;
                    ssMsg << ": unknown PropertyType " << propertyType;
                    return ssMsg.str();
                }
                break;
        }
        return std::string("error: getPropertyTypeStrint()");
    }


} // namespase bias
