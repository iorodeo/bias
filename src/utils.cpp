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

    std::string getVideoModeString(VideoMode vidMode)
    {
        switch (vidMode)
        {
            case VIDEOMODE_160x120YUV444: 
                return std::string("160x120YUV444");
                break;

            case VIDEOMODE_320x240YUV422: 
                return std::string("320x240YUV422");
                break;

            case VIDEOMODE_640x480YUV411: 
                return std::string("640x480YUV411");
                break;

            case VIDEOMODE_640x480YUV422: 
                return std::string("640x480YUV422");
                break;

            case VIDEOMODE_640x480RGB: 
                return std::string("640x480RGB");
                break;

            case VIDEOMODE_640x480Y8: 
                return std::string("640x480Y8");
                break;

            case VIDEOMODE_640x480Y16: 
                return std::string("640x480Y16");
                break;

            case VIDEOMODE_800x600YUV422: 
                return std::string("800x600YUV422");
                break;

            case VIDEOMODE_800x600RGB: 
                return std::string("800x600RGB");
                break;

            case VIDEOMODE_800x600Y8: 
                return std::string("800x600Y8");
                break;

            case VIDEOMODE_800x600Y16: 
                return std::string("800x600Y16");
                break;

            case VIDEOMODE_1024x768YUV422: 
                return std::string("1024x768YUV422");
                break;

            case VIDEOMODE_1024x768RGB: 
                return std::string("1024x768RGB");
                break;

            case VIDEOMODE_1024x768Y8: 
                return std::string("1024x768Y8");
                break;

            case VIDEOMODE_1024x768Y16: 
                return std::string("1024x768Y16");
                break;

            case VIDEOMODE_1280x960YUV422: 
                return std::string("1280x960YUV422");
                break;

            case VIDEOMODE_1280x960RGB: 
                return std::string("1280x960RGB");
                break;

            case VIDEOMODE_1280x960Y8: 
                return std::string("1280x960Y8");
                break;

            case VIDEOMODE_1280x960Y16: 
                return std::string("1280x960Y16");
                break;

            case VIDEOMODE_1600x1200YUV422: 
                return std::string("1600x1200YUV422");
                break;

            case VIDEOMODE_1600x1200RGB: 
                return std::string("1600x1200RGB");
                break;

            case VIDEOMODE_1600x1200Y8: 
                return std::string("1600x1200Y8");
                break;

            case VIDEOMODE_1600x1200Y16: 
                return std::string("1600x1200Y16");
                break;

            case VIDEOMODE_FORMAT7: 
                return std::string("Format7");
                break;

            case VIDEOMODE_UNSPECIFIED:
                return std::string("Unspecified");
                break;
                
            default:
                {
                    std::stringstream ssMsg;
                    ssMsg << "uknown videoMode: " << vidMode;
                    return ssMsg.str();
                }
                break;
        }
        return std::string("error: getVideoModeString()");
    }

    std::string getFrameRateString(FrameRate frmRate)
    {
        switch (frmRate)
        {
            case FRAMERATE_1_875: 
                return std::string("1.875");
                break;

            case FRAMERATE_3_75: 
                return std::string("3.75");
                break;

            case FRAMERATE_7_5: 
                return std::string("7.5");
                break;

            case FRAMERATE_15: 
                return std::string("15");
                break;

            case FRAMERATE_30: 
                return std::string("30");
                break;

            case FRAMERATE_60: 
                return std::string("60");
                break;

            case FRAMERATE_120: 
                return std::string("120");
                break;

            case FRAMERATE_240: 
                return std::string("240");
                break;

            case FRAMERATE_FORMAT7: 
                return std::string("Format7");
                break;

            case FRAMERATE_UNSPECIFIED:
                return std::string("Unspecified");
                break;

            default:
                {
                    std::stringstream ssMsg;
                    ssMsg << "uknown frmRate: " << frmRate;
                    return ssMsg.str();
                }
                break;
        }
        return std::string("error: getFrameRateString()");
    }

    std::string getPropertyTypeString(PropertyType propType)
    {
        switch(propType)
        {
            case PROPERTY_TYPE_BRIGHTNESS: 
                return std::string("Brightness");
                break;

            case PROPERTY_TYPE_AUTO_EXPOSURE:
                return std::string("Auto Exposure");
                break;

            case PROPERTY_TYPE_SHARPNESS:
                return std::string("Sharpness");
                break;

            case PROPERTY_TYPE_WHITE_BALANCE:
                return std::string("White Balance");
                break;

            case PROPERTY_TYPE_HUE:
                return std::string("Hue");
                break;

            case PROPERTY_TYPE_SATURATION:
                return std::string("Saturation");
                break;

            case PROPERTY_TYPE_GAMMA:
                return std::string("Gamma");
                break;

            case PROPERTY_TYPE_IRIS:
                return std::string("Iris");
                break;

            case PROPERTY_TYPE_FOCUS:
                return std::string("Focus");
                break;

            case PROPERTY_TYPE_ZOOM:
                return std::string("Zoom");
                break;

            case PROPERTY_TYPE_PAN:
                return std::string("Pan");
                break;

            case PROPERTY_TYPE_TILT:
                return std::string("Tilt");
                break;

            case PROPERTY_TYPE_SHUTTER:
                return std::string("Shutter");
                break;

            case PROPERTY_TYPE_GAIN:
                return std::string("Gain");
                break;

            case PROPERTY_TYPE_TRIGGER_MODE:
                return std::string("Trigger Mode");
                break;

            case PROPERTY_TYPE_TRIGGER_DELAY:
                return std::string("Trigger Delay");
                break;

            case PROPERTY_TYPE_FRAME_RATE:
                return std::string("Frame Rate");
                break;

            case PROPERTY_TYPE_TEMPERATURE:
                return std::string("Temperature");
                break;
            case PROPERTY_TYPE_UNSPECIFIED:
                return std::string("Unspecified");
            default:
                {
                    std::stringstream ssMsg;
                    ssMsg << ": unknown PropertyType " << propType;
                    return ssMsg.str();
                }
                break;
        }
        return std::string("error: getPropertyTypeStrint()");
    }


} // namespase bias
