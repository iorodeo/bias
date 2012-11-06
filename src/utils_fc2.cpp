#ifdef WITH_FC2
#include "utils_fc2.hpp"
#include <iostream>

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

    void printImageInfo_fc2(fc2Image &image)
    {

        std::cout << std::endl;
        std::cout << "--------------------------" << std::endl;
        std::cout << " Image Info               " << std::endl;
        std::cout << "--------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " LIbrary:          " << "FlyCapture2" << std::endl;
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
    
}
#endif
