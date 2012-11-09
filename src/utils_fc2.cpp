#ifdef WITH_FC2
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias {
    
    // Conversion from BIAS enumeration types to FlyCapture2 enumerations
    // ------------------------------------------------------------------------

    fc2PropertyType getPropertyType_fc2(PropertyType propertyType)
    {
        switch (propertyType)
        {
            case PROPERTY_BRIGHTNESS: 
                return FC2_BRIGHTNESS;
                break;

            case PROPERTY_AUTO_EXPOSURE:
                return FC2_AUTO_EXPOSURE;
                break;

            case PROPERTY_SHARPNESS:
                return FC2_SHARPNESS;
                break;

            case PROPERTY_WHITE_BALANCE:
                return FC2_WHITE_BALANCE;
                break;

            case PROPERTY_HUE:
                return FC2_HUE;
                break;

            case PROPERTY_SATURATION:
                return FC2_SATURATION;
                break;

            case PROPERTY_GAMMA:
                return FC2_GAMMA;
                break;

            case PROPERTY_IRIS:
                return FC2_IRIS;
                break;

            case PROPERTY_FOCUS:
                return FC2_FOCUS;
                break;

            case PROPERTY_ZOOM:
                return FC2_ZOOM;
                break;

            case PROPERTY_PAN:
                return FC2_PAN;
                break;

            case PROPERTY_TILT:
                return FC2_TILT;
                break;

            case PROPERTY_SHUTTER:
                return FC2_SHUTTER;
                break;

            case PROPERTY_GAIN:
                return FC2_GAIN;
                break;

            case PROPERTY_TRIGGER_MODE:
                return FC2_TRIGGER_MODE;
                break;

            case PROPERTY_TRIGGER_DELAY:
                return FC2_TRIGGER_DELAY;
                break;

            case PROPERTY_FRAME_RATE:
                return FC2_FRAME_RATE;
                break;

            case PROPERTY_TEMPERATURE:
                return FC2_TEMPERATURE;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert property to FlyCaptuer2 property";
                    throw RuntimeError(ERROR_FC2_CONVERT_PROPERTY_TYPE, ssError.str());
                } 
                break;

        }
        return FC2_UNSPECIFIED_PROPERTY_TYPE;
    }

    fc2FrameRate getFrameRate_fc2(FrameRate frameRate)
    {
        switch (frameRate)
        {
            case FRAMERATE_1_875: 
                return FC2_FRAMERATE_1_875;
                break;

            case FRAMERATE_3_75: 
                return FC2_FRAMERATE_3_75;
                break;

            case FRAMERATE_7_5: 
                return FC2_FRAMERATE_7_5;
                break;

            case FRAMERATE_15: 
                return FC2_FRAMERATE_15;
                break;

            case FRAMERATE_30: 
                return FC2_FRAMERATE_30;
                break;

            case FRAMERATE_60: 
                return FC2_FRAMERATE_60;
                break;

            case FRAMERATE_120: 
                return FC2_FRAMERATE_120;
                break;

            case FRAMERATE_240: 
                return FC2_FRAMERATE_240;
                break;

            case FRAMERATE_FORMAT7: 
                return FC2_FRAMERATE_FORMAT7;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert framerate to fc2 frame rate";
                    throw RuntimeError(ERROR_FC2_CONVERT_FRAMERATE, ssError.str());
                }
                break;

        }
        return FC2_NUM_FRAMERATES;
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
            std::cout << "   [i] = " << packetInfo.reserved[i] << std::endl;
        }
        std::cout << std::endl;
    }


    // Functions for converting FlyCapture2 enumerations to strings   
    // Mostly autogenerated. 
    // ------------------------------------------------------------------------

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
}
#endif
