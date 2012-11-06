#ifdef WITH_FC2
#include "utils_fc2.hpp"
#include <iostream>
#include <bitset>

namespace bias {

    std::string getInterfaceTypeString_fc2(fc2InterfaceType ifaceType)
    {
        switch (ifaceType) 
        { 
    
            case FC2_INTERFACE_IEEE1394:
                return std::string("FC2_INTERFACE_IEEE1394");
                break;
    
            case FC2_INTERFACE_USB_2:
                return std::string("FC2_INTERFACE_USB_2");
                break;
    
            case FC2_INTERFACE_USB_3:
                return std::string("FC2_INTERFACE_USB_3");
                break;
                
            case FC2_INTERFACE_GIGE:
                return std::string("FC2_INTERFACE_GIGE");
                break;
    
            case FC2_INTERFACE_UNKNOWN:
                return std::string("FC2_INTERFACE_UNKNOWN");
                break;
                
            default:
                return std::string("unknown fc2 interface value");
                break;
        }
    }


    std::string getPixelFormatString_fc2(fc2PixelFormat format)
    {
        switch (format) 
        {
            case FC2_PIXEL_FORMAT_MONO8:
                return std::string("FC2_PIXEL_FORMAT_MONO8");
                break;

            case FC2_PIXEL_FORMAT_411YUV8:		
                return std::string("FC2_PIXEL_FORMAT_411YUV8");
                break;

            case FC2_PIXEL_FORMAT_422YUV8:		
                return std::string("FC2_PIXEL_FORMAT_422YUV8");
                break;

            case FC2_PIXEL_FORMAT_444YUV8:
                return std::string("FC2_PIXEL_FORMAT_444YUV8");
                break;

            case FC2_PIXEL_FORMAT_RGB8:
                return std::string("FC2_PIXEL_FORMAT_RGB8");
                break;

            case FC2_PIXEL_FORMAT_MONO16:
                return std::string("FC2_PIXEL_FORMAT_MONO16");
                break;

            case FC2_PIXEL_FORMAT_RGB16:
                return std::string("FC2_PIXEL_FORMAT_RGB16");
                break;

            case FC2_PIXEL_FORMAT_S_MONO16:
                return std::string("FC2_PIXEL_FORMAT_S_MONO16");
                break;

            case FC2_PIXEL_FORMAT_S_RGB16:
                return std::string("FC2_PIXEL_FORMAT_S_RGB16");
                break;

            case FC2_PIXEL_FORMAT_RAW8:
                return std::string("FC2_PIXEL_FORMAT_RAW8");
                break;

            case FC2_PIXEL_FORMAT_RAW16:
                return std::string("FC2_PIXEL_FORMAT_RAW16");
                break;

            case FC2_PIXEL_FORMAT_MONO12:
                return std::string("FC2_PIXEL_FORMAT_MONO12");
                break;

            case FC2_PIXEL_FORMAT_RAW12:
                return std::string("FC2_PIXEL_FORMAT_RAW12");
                break;

            case FC2_PIXEL_FORMAT_BGR:
                return std::string("FC2_PIXEL_FORMAT_BGR");
                break;

            case FC2_PIXEL_FORMAT_BGRU:
                return std::string("FC2_PIXEL_FORMAT_BGRU");
                break;

            case FC2_PIXEL_FORMAT_RGBU:
                return std::string("FC2_PIXEL_FORMAT_RGBU");
                break;

            case FC2_PIXEL_FORMAT_BGR16:
                return std::string("FC2_PIXEL_FORMAT_BGR16");
                break;

            case FC2_PIXEL_FORMAT_BGRU16:
                return std::string("FC2_PIXEL_FORMAT_BGRU16");
                break;

            case FC2_PIXEL_FORMAT_422YUV8_JPEG:
                return std::string("FC2_PIXEL_FORMAT_422YUV8_JPEG");
                break;

            case FC2_NUM_PIXEL_FORMATS:
                return std::string("FC2_NUM_PIXEL_FORMATS");
                break;

            case FC2_UNSPECIFIED_PIXEL_FORMAT:
                return std::string("FC2_UNSPECIFIED_PIXEL_FORMAT");
                break;

            default:
                return std::string("unknown fc2 pixel format");
                break;
        }
    }

    std::string getBayerTileFormatString_fc2(fc2BayerTileFormat bayerFormat)
    {
        switch (bayerFormat) 
        {
            case FC2_BT_NONE: 
                return std::string("FC2_BT_NONE");
                break;

            case FC2_BT_RGGB: 
                return std::string("FC2_BT_RGGB");
                break;

            case FC2_BT_GRBG: 
                return std::string("FC2_BT_GRBG");
                break;

            case FC2_BT_GBRG: 
                return std::string("FC2_BT_GBRG");
                break;

            case FC2_BT_BGGR: 
                return std::string("FC2_BT_BGGR");
                break;

            default:
                return std::string("unknown fc2 bayer format");
                break;
        }
    }

    std::string getVideoModeString_fc2(fc2VideoMode videoMode)
    {
        switch (videoMode) 
        {
            case FC2_VIDEOMODE_160x120YUV444:
                return std::string("FC2_VIDEOMODE_160x120YUV444");
                break;

            case FC2_VIDEOMODE_320x240YUV422:
                return std::string("FC2_VIDEOMODE_320x240YUV422");
                break;

            case FC2_VIDEOMODE_640x480YUV411:
                return std::string("FC2_VIDEOMODE_640x480YUV411");
                break;

            case FC2_VIDEOMODE_640x480YUV422:
                return std::string("FC2_VIDEOMODE_640x480YUV422");
                break;

            case FC2_VIDEOMODE_640x480RGB:
                return std::string("FC2_VIDEOMODE_640x480RGB");
                break;

            case FC2_VIDEOMODE_640x480Y8:
                return std::string("FC2_VIDEOMODE_640x480Y8");
                break;

            case FC2_VIDEOMODE_640x480Y16:
                return std::string("FC2_VIDEOMODE_640x480Y16");
                break;

            case FC2_VIDEOMODE_800x600YUV422:
                return std::string("FC2_VIDEOMODE_800x600YUV422");
                break;

            case FC2_VIDEOMODE_800x600RGB:
                return std::string("FC2_VIDEOMODE_800x600RGB");
                break;

            case FC2_VIDEOMODE_800x600Y8:
                return std::string("FC2_VIDEOMODE_800x600Y8");
                break;

            case FC2_VIDEOMODE_800x600Y16:
                return std::string("FC2_VIDEOMODE_800x600Y16");
                break;

            case FC2_VIDEOMODE_1024x768YUV422:
                return std::string("FC2_VIDEOMODE_1024x768YUV422");
                break;

            case FC2_VIDEOMODE_1024x768RGB:
                return std::string("FC2_VIDEOMODE_1024x768RGB");
                break;

            case FC2_VIDEOMODE_1024x768Y8:
                return std::string("FC2_VIDEOMODE_1024x768Y8");
                break;

            case FC2_VIDEOMODE_1024x768Y16:
                return std::string("FC2_VIDEOMODE_1024x768Y16");
                break;

            case FC2_VIDEOMODE_1280x960YUV422:
                return std::string("FC2_VIDEOMODE_1280x960YUV422");
                break;

            case FC2_VIDEOMODE_1280x960RGB:
                return std::string("FC2_VIDEOMODE_1280x960RGB");
                break;

            case FC2_VIDEOMODE_1280x960Y8:
                return std::string("FC2_VIDEOMODE_1280x960Y8");
                break;

            case FC2_VIDEOMODE_1280x960Y16:
                return std::string("FC2_VIDEOMODE_1280x960Y16");
                break;

            case FC2_VIDEOMODE_1600x1200YUV422:
                return std::string("FC2_VIDEOMODE_1600x1200YUV422");
                break;

            case FC2_VIDEOMODE_1600x1200RGB:
                return std::string("FC2_VIDEOMODE_1600x1200RGB");
                break;

            case FC2_VIDEOMODE_1600x1200Y8:
                return std::string("FC2_VIDEOMODE_1600x1200Y8");
                break;

            case FC2_VIDEOMODE_1600x1200Y16:
                return std::string("FC2_VIDEOMODE_1600x1200Y16");
                break;

            case FC2_VIDEOMODE_FORMAT7:
                return std::string("FC2_VIDEOMODE_FORMAT7");
                break;

            default:
                return std::string("unknown fc2 video mode");
                break;
        }
    }

    std::string getFrameRateString_fc2(fc2FrameRate frameRate)
    {
        switch (frameRate) 
        {
           
            case FC2_FRAMERATE_1_875:
                return std::string("FC2_FRAMERATE_1_875");
                break;

            case FC2_FRAMERATE_3_75:
                return std::string("FC2_FRAMERATE_3_75");
                break;

            case FC2_FRAMERATE_7_5:
                return std::string("FC2_FRAMERATE_7_5");
                break;

            case FC2_FRAMERATE_15:
                return std::string("FC2_FRAMERATE_15");
                break;

            case FC2_FRAMERATE_30:
                return std::string("FC2_FRAMERATE_30");
                break;

            case FC2_FRAMERATE_60:
                return std::string("FC2_FRAMERATE_60");
                break;

            case FC2_FRAMERATE_120:
                return std::string("FC2_FRAMERATE_120");
                break;

            case FC2_FRAMERATE_240:
                return std::string("FC2_FRAMERATE_240");
                break;

            case FC2_FRAMERATE_FORMAT7:
                return std::string("FC2_FRAMERATE_FORMAT7");
                break;

            default:
                return std::string("unknown fc2 framerate");
                break;

        }
    }

    std::string getModeString_fc2(fc2Mode mode)
    {
        switch( mode )
        {
            case FC2_MODE_0:
                return std::string("FC2_MODE_0");
                break;

            case FC2_MODE_1:
                return std::string("FC2_MODE_1");
                break;

            case FC2_MODE_2:
                return std::string("FC2_MODE_2");
                break;

            case FC2_MODE_3:
                return std::string("FC2_MODE_3");
                break;

            case FC2_MODE_4:
                return std::string("FC2_MODE_4");
                break;

            case FC2_MODE_5:
                return std::string("FC2_MODE_5");
                break;

            case FC2_MODE_6:
                return std::string("FC2_MODE_6");
                break;

            case FC2_MODE_7:
                return std::string("FC2_MODE_7");
                break;

            case FC2_MODE_8:
                return std::string("FC2_MODE_8");
                break;

            case FC2_MODE_9:
                return std::string("FC2_MODE_9");
                break;

            case FC2_MODE_10:
                return std::string("FC2_MODE_10");
                break;

            case FC2_MODE_11:
                return std::string("FC2_MODE_11");
                break;

            case FC2_MODE_12:
                return std::string("FC2_MODE_12");
                break;

            case FC2_MODE_13:
                return std::string("FC2_MODE_13");
                break;

            case FC2_MODE_14:
                return std::string("FC2_MODE_14");
                break;

            case FC2_MODE_15:
                return std::string("FC2_MODE_15");
                break;

            case FC2_MODE_16:
                return std::string("FC2_MODE_16");
                break;

            case FC2_MODE_17:
                return std::string("FC2_MODE_17");
                break;

            case FC2_MODE_18:
                return std::string("FC2_MODE_18");
                break;

            case FC2_MODE_19:
                return std::string("FC2_MODE_19");
                break;

            case FC2_MODE_20:
                return std::string("FC2_MODE_20");
                break;

            case FC2_MODE_21:
                return std::string("FC2_MODE_21");
                break;

            case FC2_MODE_22:
                return std::string("FC2_MODE_22");
                break;

            case FC2_MODE_23:
                return std::string("FC2_MODE_23");
                break;

            case FC2_MODE_24:
                return std::string("FC2_MODE_24");
                break;

            case FC2_MODE_25:
                return std::string("FC2_MODE_25");
                break;

            case FC2_MODE_26:
                return std::string("FC2_MODE_26");
                break;

            case FC2_MODE_27:
                return std::string("FC2_MODE_27");
                break;

            case FC2_MODE_28:
                return std::string("FC2_MODE_28");
                break;

            case FC2_MODE_29:
                return std::string("FC2_MODE_29");
                break;

            case FC2_MODE_30:
                return std::string("FC2_MODE_30");
                break;

            case FC2_MODE_31:
                return std::string("FC2_MODE_31");
                break;

            default:
                return std::string("uknown fc2 mode");

        }

    }

    void printImageInfo_fc2(fc2Image &image)
    {
        std::cout << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << "FlyCapture2 Image Info" << std::endl;
        std::cout << "----------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " Rows:             " << image.rows << std::endl;
        std::cout << " Cols:             " << image.cols << std::endl;
        std::cout << " Stride:           " << image.stride << std::endl;
        std::cout << " DataSize:         " << image.dataSize << std::endl;
        std::cout << " ReceivedDataSize: " << image.receivedDataSize << std::endl;
        std::cout << " Format            " << std::endl;
        std::cout << "   Code:           " << "0x" << std::hex << image.format << std::dec << std::endl;
        std::cout << "   String:         " << getPixelFormatString_fc2(image.format) << std::endl;
        std::cout << " BayerFormat       " << std::endl;
        std::cout << "   Code:           " << "0x" << std::hex << image.bayerFormat << std::dec << std::endl;
        std::cout << "   String:         " << getBayerTileFormatString_fc2(image.bayerFormat) << std::endl;
        std::cout << std::endl;
    }

    void printFormat7ImageSettings_fc2(fc2Format7ImageSettings &imageSettings)
    {
        std::cout << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "FlyCatpure2 Format 7 Image settings " << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " Mode:         " << getModeString_fc2(imageSettings.mode) << std::endl;
        std::cout << " OffsetX:      " << imageSettings.offsetX << std::endl;
        std::cout << " OffsetY:      " << imageSettings.offsetY << std::endl;
        std::cout << " Width:        " << imageSettings.width << std::endl; 
        std::cout << " Height:       " << imageSettings.height << std::endl;
        std::cout << " Pixel Format: " << getPixelFormatString_fc2(imageSettings.pixelFormat) << std::endl;
        std::cout << " Reserved:     " << std::endl;
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
        std::cout << " Percentage:   " << percentage << std::endl;
        std::cout << " Packet Size:  " << packetSize << std::endl;
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
        std::cout << " Mode:                      " << getModeString_fc2(format7Info.mode) << std::endl;
        std::cout << " Max Width:                 " << format7Info.maxWidth << std::endl;
        std::cout << " Max Height:                " << format7Info.maxHeight << std::endl;
        std::cout << " OffsetHStepSize:           " << format7Info.offsetHStepSize << std::endl;
        std::cout << " OffsetVStepSize:           " << format7Info.offsetVStepSize << std::endl;
        std::cout << " ImageHStepSize:            " << format7Info.imageHStepSize << std::endl;
        std::cout << " ImageVStepSize:            " << format7Info.imageVStepSize << std:: endl;
        std::cout << " PixelFormatBitField:       " << format7Info.pixelFormatBitField << std::endl;
        std::cout << " PixelFormatBitField:       " << std::bitset<32>(format7Info.pixelFormatBitField) << std::endl;
        std::cout << " VendorPixelFormatBitField: " << format7Info.vendorPixelFormatBitField << std::endl;
        std::cout << " VendorPixelFormatBitField: " << std::bitset<32>(format7Info.vendorPixelFormatBitField) << std::endl;
        std::cout << " Packet Size:               " << format7Info.packetSize << std::endl;
        std::cout << " MinPacketSize:             " << format7Info.minPacketSize << std::endl;
        std::cout << " MaxPacketSize:             " << format7Info.maxPacketSize << std::endl;
        std::cout << std::endl;
    }
}
#endif
