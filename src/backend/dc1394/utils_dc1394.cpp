#include "utils_dc1394.hpp"
#include "exception.hpp"
#include <opencv2/core/core.hpp>
#include <map>
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias
{

    // Conversion from BIAS types to libdc1394 types
    // ------------------------------------------------------------------------
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
        if (videoModeMap_to_dc1394.count(vidMode) != 0)
        {
            return videoModeMap_to_dc1394[vidMode];
        }
        else
        {
            if ((vidMode == VIDEOMODE_FORMAT7) && (imgMode < NUMBER_OF_IMAGEMODE))
            {
                switch (imgMode)
                {
                    case IMAGEMODE_0:
                        return DC1394_VIDEO_MODE_FORMAT7_0;

                    case IMAGEMODE_1:
                        return DC1394_VIDEO_MODE_FORMAT7_1;

                    case IMAGEMODE_2:
                        return DC1394_VIDEO_MODE_FORMAT7_2;

                    case IMAGEMODE_3:
                        return DC1394_VIDEO_MODE_FORMAT7_3;

                    case IMAGEMODE_4:
                        return DC1394_VIDEO_MODE_FORMAT7_4;

                    case IMAGEMODE_5:
                        return DC1394_VIDEO_MODE_FORMAT7_5;

                    case IMAGEMODE_6:
                        return DC1394_VIDEO_MODE_FORMAT7_6;

                    case IMAGEMODE_7:
                        return DC1394_VIDEO_MODE_FORMAT7_7;

                }
            }
            else
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to convert framerate to libdc1394 frame rate";
                throw RuntimeError(ERROR_DC1394_CONVERT_FRAMERATE, ssError.str());
            }
        }

        dc1394video_mode_t vidMode_dc1394;
        return vidMode_dc1394;
    }

    // Conversion from libdc1394 types to BIAS types
    // ------------------------------------------------------------------------
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
        if (videoModeMap_from_dc1394.count(vidMode_dc1394) != 0)
        {
            return videoModeMap_from_dc1394[vidMode_dc1394];
        }
        else
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert libdc1394 framerate to framerate";
            throw RuntimeError(ERROR_DC1394_CONVERT_FRAMERATE, ssError.str());
        }
    }


    // Print functions for libdc1394 configurations, settings and info
    //-------------------------------------------------------------------------
    //
    void printVideoModes_dc1394(dc1394video_modes_t modes)
    {  
        for (int i=0; i<modes.num; i++)
        {
            std::cout << "i: " << i << ", mode: "; 
            std::cout << getVideoModeString_dc1394(modes.modes[i]) << std::endl;
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

    std::string getVideoModeString_dc1394(dc1394video_mode_t vidMode)
    {
        if (videoModeToStringMap_dc1394.count(vidMode) != 0)
        {
            return videoModeToStringMap_dc1394[vidMode];
        }
        else
        {
            return std::string("unknown dc1394 video mode");
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

    std::string getColorCodingString_dc1394(dc1394color_coding_t colorCoding)
    {
        if (colorCodingToStringMap_dc1394.count(colorCoding) != 0)
        {
            return colorCodingToStringMap_dc1394[colorCoding];
        }
        else
        {
            return std::string("unknown dc1394 color coding");
        }
    }

    
} // namespace bias

  
