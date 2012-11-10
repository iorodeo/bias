#include "utils.hpp"
#include <sstream>

namespace bias
{
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
                    ssMsg << "uknown video mode: " << videoMode;
                    return ssMsg.str();
                }
                break;
        }
    }



} // namespase bias
