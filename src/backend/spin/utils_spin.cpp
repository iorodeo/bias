#ifdef WITH_SPIN
#include "utils_spin.hpp"
#include "exception.hpp"
#include <opencv2/core/core.hpp>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias 
{
    const size_t MAX_BUF_LEN = 256;


    spinPixelFormatEnums getImagePixelFormat_spin(spinImage hImage)
    {
        spinPixelFormatEnums pixelFormat = UNKNOWN_PIXELFORMAT;
        spinError err = SPINNAKER_ERR_SUCCESS;
        err = spinImageGetPixelFormat(hImage, &pixelFormat);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage pixel format, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PIXEL_FORMAT, ssError.str());
        }
        return pixelFormat;
    }


    ImageInfo_spin getImageInfo_spin(spinImage hImage)
    {
        ImageInfo_spin info = {0,0,0,0,0,0,nullptr,UNKNOWN_PIXELFORMAT};
        spinError err = SPINNAKER_ERR_SUCCESS;

        err = spinImageGetPaddingX(hImage, &info.xpad);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage x padding, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PADDING_X, ssError.str());
        }

        err = spinImageGetPaddingY(hImage, &info.ypad);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage y padding, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PADDING_Y, ssError.str());
        }

        err = spinImageGetWidth(hImage, &info.cols);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage width, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_WIDTH, ssError.str());
        }

        err = spinImageGetHeight(hImage, &info.rows);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage width, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_HEIGHT, ssError.str());
        }

        err = spinImageGetStride(hImage, &info.stride);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage stride, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_STRIDE, ssError.str());
        }

        err = spinImageGetSize(hImage, &info.dataSize);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get image size, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_SIZE, ssError.str());
        }

        err = spinImageGetData(hImage,&info.dataPtr);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage data pointer, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_DATA_PTR, ssError.str());
        }

        err = spinImageGetPixelFormat(hImage, &info.pixelFormat);
        if (err != SPINNAKER_ERR_SUCCESS) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get spinImage pixel format, error = " << err; 
            throw RuntimeError(ERROR_SPIN_IMAGE_GET_PIXEL_FORMAT, ssError.str());
        }

        return info;
    }



    std::vector<spinPixelFormatEnums> getAllowedPixelFormats_spin()
    {
        std::vector<spinPixelFormatEnums>  formatsVec = 
        {
            PixelFormat_Mono8,
            PixelFormat_YUV411_8_UYYVYY,
            PixelFormat_YUV422_8,
            PixelFormat_YUV8_UYV,
            PixelFormat_RGB8,
            PixelFormat_Mono16,
            PixelFormat_RGB16,
            PixelFormat_Raw8,
            PixelFormat_Raw16,
            PixelFormat_Mono12,
            PixelFormat_BGR8,
            PixelFormat_BGR16
        };
        return formatsVec;
    }


    std::vector<spinPixelFormatEnums> getAllowedColorPixelFormats_spin()
    {
        std::vector<spinPixelFormatEnums>  formatsVec = 
        {
            PixelFormat_YUV411_8_UYYVYY,
            PixelFormat_YUV422_8,
            PixelFormat_YUV8_UYV,
            PixelFormat_RGB8,
            PixelFormat_RGB16,
            PixelFormat_BGR8,
            PixelFormat_BGR16
        };
        return formatsVec;
    }


    std::vector<spinPixelFormatEnums> getAllowedMonoPixelFormats_spin()
    {
        std::vector<spinPixelFormatEnums>  formatsVec = 
        {
            PixelFormat_Mono8,
            PixelFormat_Mono16,
            PixelFormat_Raw8,
            PixelFormat_Raw16,
            PixelFormat_Mono12,
        };
        return formatsVec;
    }


    bool isAllowedPixelFormat_spin(spinPixelFormatEnums pixelFormat_spin)
    {
        std::vector<spinPixelFormatEnums> allowedFormats_spin = getAllowedPixelFormats_spin();
        return std::find(std::begin(allowedFormats_spin), std::end(allowedFormats_spin), pixelFormat_spin) != std::end(allowedFormats_spin);
    }


    std::vector<PropertyType> getSpinSupportedPropertyTypes()
    {
        std::vector<PropertyType> spinSupportedVec = 
        {
            PROPERTY_TYPE_BRIGHTNESS,       
            PROPERTY_TYPE_GAMMA,
            PROPERTY_TYPE_SHUTTER,
            PROPERTY_TYPE_GAIN,
            PROPERTY_TYPE_TRIGGER_MODE,
            PROPERTY_TYPE_TRIGGER_DELAY,
            PROPERTY_TYPE_FRAME_RATE,
            PROPERTY_TYPE_TEMPERATURE,
        };
        return  spinSupportedVec;
    }


    bool isSpinSupportedPropertyType(PropertyType propType)
    { 
        std::vector<PropertyType> supportedTypeVec = getSpinSupportedPropertyTypes();
        return (std::find(supportedTypeVec.begin(), supportedTypeVec.end(), propType) != supportedTypeVec.end()); 
    }



    // Image conversion - for mapping from Spinnaker to opencv 
    // -----------------------------------------------------------------------
    
    spinPixelFormatEnums getSuitablePixelFormat(spinPixelFormatEnums pixFormat)
    {
        spinPixelFormatEnums convPixFormat = UNKNOWN_PIXELFORMAT;

        switch (pixFormat)
        {
            case UNKNOWN_PIXELFORMAT:
                convPixFormat = UNKNOWN_PIXELFORMAT;

            case PixelFormat_Raw8:
            case PixelFormat_Mono8:
                convPixFormat = PixelFormat_Mono8;
                break;

            case PixelFormat_Raw16:
            case PixelFormat_Mono16:
                convPixFormat = PixelFormat_Mono16;
                break;

            case PixelFormat_BGR8:
            case PixelFormat_RGB8:
                convPixFormat = PixelFormat_BGR8;
                break;

            case PixelFormat_BGR16:
            case PixelFormat_RGB16:
                convPixFormat = PixelFormat_BGR16;
                break;

            case PixelFormat_Mono1p:
            case PixelFormat_Mono2p:
            case PixelFormat_Mono4p:
            case PixelFormat_Mono8s:
            case PixelFormat_R8:
            case PixelFormat_G8:
            case PixelFormat_B8:
            case PixelFormat_RGB8Packed:
                convPixFormat = PixelFormat_Mono8;
                break;

            case PixelFormat_Mono10:
            case PixelFormat_Mono10p:
            case PixelFormat_Mono12:
            case PixelFormat_Mono12p:
            case PixelFormat_Mono14:
            case PixelFormat_R10:
            case PixelFormat_R12:
            case PixelFormat_R16:
            case PixelFormat_G10:
            case PixelFormat_G12:
            case PixelFormat_G16:
            case PixelFormat_B10:
            case PixelFormat_B12:
            case PixelFormat_B16:
            case PixelFormat_Mono10Packed:
            case PixelFormat_Mono12Packed:
                convPixFormat = PixelFormat_Mono16;
                break;

            case PixelFormat_RGBa8:
            case PixelFormat_BGRa8:
            case PixelFormat_BayerBG8:
            case PixelFormat_BayerGB8:
            case PixelFormat_BayerGR8:
            case PixelFormat_BayerRG8:
            case PixelFormat_RGB8_Planar:
                convPixFormat = PixelFormat_BGR8;
                break;

            default:
                convPixFormat = PixelFormat_BGR16;
                break;

        }
        return convPixFormat;
    }


    int getCompatibleOpencvFormat(spinPixelFormatEnums pixFormat)
    {
        int opencvFormat = CV_8UC1;

        switch (pixFormat)
        {
            case PixelFormat_Raw8:
            case PixelFormat_Mono8:
                opencvFormat = CV_8UC1;
                break;

            case PixelFormat_Raw16:
            case PixelFormat_Mono16:
                opencvFormat = CV_16UC1;
                break;

            case PixelFormat_BGR8:
                opencvFormat = CV_8UC3;
                break;

            case PixelFormat_BGR16:
                opencvFormat = CV_16UC3;
                break;

            default:
                std::stringstream ssError;
                ssError << "no compatible opencv format" << std::endl;
                throw RuntimeError(ERROR_SPIN_NO_COMPATIBLE_OPENCV_FORMAT, ssError.str());
                break;
        }

        return opencvFormat;
    }


    static std::map<PixelFormat, spinPixelFormatEnums> pixelFormatMap_to_spin = 
    {
        {PIXEL_FORMAT_MONO8,     PixelFormat_Mono8},
        {PIXEL_FORMAT_411YUV8,   PixelFormat_YUV411_8_UYYVYY},
        {PIXEL_FORMAT_422YUV8,   PixelFormat_YUV422_8},
        {PIXEL_FORMAT_444YUV8,   PixelFormat_YUV8_UYV},
        {PIXEL_FORMAT_RGB8,      PixelFormat_RGB8},
        {PIXEL_FORMAT_MONO16,    PixelFormat_Mono16},
        {PIXEL_FORMAT_RGB16,     PixelFormat_RGB16},
        {PIXEL_FORMAT_RAW8,      PixelFormat_Raw8},
        {PIXEL_FORMAT_RAW16,     PixelFormat_Raw16},
        {PIXEL_FORMAT_MONO12,    PixelFormat_Mono12},
        {PIXEL_FORMAT_BGR8,      PixelFormat_BGR8},
        {PIXEL_FORMAT_BGR16,     PixelFormat_BGR16}
    };


    spinPixelFormatEnums convertPixelFormat_to_spin(PixelFormat pixFormat)
    {
        if (pixelFormatMap_to_spin.count(pixFormat) != 0)
        {
            return pixelFormatMap_to_spin[pixFormat];
        }
        else
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert pixel format to Spinnaker ";
            ssError << "imaging mode";
            throw RuntimeError(ERROR_SPIN_CONVERT_PIXEL_FORMAT, ssError.str());
        }
    }


    static std::map<spinPixelFormatEnums, PixelFormat> pixelFormatMap_from_spin = 
    {
        {PixelFormat_Mono8,           PIXEL_FORMAT_MONO8},
        {PixelFormat_YUV411_8_UYYVYY, PIXEL_FORMAT_411YUV8},
        {PixelFormat_YUV422_8,        PIXEL_FORMAT_422YUV8},
        {PixelFormat_YUV8_UYV,        PIXEL_FORMAT_444YUV8},
        {PixelFormat_RGB8,            PIXEL_FORMAT_RGB8},
        {PixelFormat_Mono16,          PIXEL_FORMAT_MONO16},
        {PixelFormat_RGB16,           PIXEL_FORMAT_RGB16},
        {PixelFormat_Raw8,            PIXEL_FORMAT_RAW8},
        {PixelFormat_Raw16,           PIXEL_FORMAT_RAW16},
        {PixelFormat_Mono12,          PIXEL_FORMAT_MONO12},
        {PixelFormat_BGR8,            PIXEL_FORMAT_BGR8},
        {PixelFormat_BGR16,           PIXEL_FORMAT_BGR16}
    };
    

    PixelFormat convertPixelFormat_from_spin(spinPixelFormatEnums pixFormat_spin)
    {
        if (pixelFormatMap_from_spin.count(pixFormat_spin) != 0)
        {
            return pixelFormatMap_from_spin[pixFormat_spin];
        }
        else
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to convert pixel format from Spinnaker";
            throw RuntimeError(ERROR_SPIN_CONVERT_PIXEL_FORMAT, ssError.str());
        }
    }


    static std::map<spinPixelFormatEnums, std::string> pixelFormatToStringMap_spin = {
        {PixelFormat_Mono1p, std::string("Monochrome 1-bit packed")},
        {PixelFormat_Mono2p, std::string("Monochrome 2-bit packed")},
        {PixelFormat_Mono4p, std::string("Monochrome 4-bit packed")},
        {PixelFormat_Mono8, std::string("Monochrome 8-bit")},
        {PixelFormat_Mono8s, std::string("Monochrome 8-bit signed")},
        {PixelFormat_Mono10, std::string("Monochrome 10-bit unpacked")},
        {PixelFormat_Mono10p, std::string("Monochrome 10-bit packed")},
        {PixelFormat_Mono12, std::string("Monochrome 12-bit unpacked")},
        {PixelFormat_Mono12p, std::string("Monochrome 12-bit packed")},
        {PixelFormat_Mono14, std::string("Monochrome 14-bit unpacked")},
        {PixelFormat_Mono16, std::string("Monochrome 16-bit")},
        {PixelFormat_BayerBG8, std::string("Bayer Blue-Green 8-bit")},
        {PixelFormat_BayerBG10, std::string("Bayer Blue-Green 10-bit unpacked")},
        {PixelFormat_BayerBG10p, std::string("Bayer Blue-Green 10-bit packed")},
        {PixelFormat_BayerBG12, std::string("Bayer Blue-Green 12-bit unpacked")},
        {PixelFormat_BayerBG12p, std::string("Bayer Blue-Green 12-bit packed")},
        {PixelFormat_BayerBG16, std::string("Bayer Blue-Green 16-bit")},
        {PixelFormat_BayerGB8, std::string("Bayer Green-Blue 8-bit")},
        {PixelFormat_BayerGB10, std::string("Bayer Green-Blue 10-bit unpacked")},
        {PixelFormat_BayerGB10p, std::string("Bayer Green-Blue 10-bit packed")},
        {PixelFormat_BayerGB12, std::string("Bayer Green-Blue 12-bit unpacked")},
        {PixelFormat_BayerGB12p, std::string("Bayer Green-Blue 12-bit packed")},
        {PixelFormat_BayerGB16, std::string("Bayer Green-Blue 16-bit")},
        {PixelFormat_BayerGR8, std::string("Bayer Green-Red 8-bit")},
        {PixelFormat_BayerGR10, std::string("Bayer Green-Red 10-bit unpacked")},
        {PixelFormat_BayerGR10p, std::string("Bayer Green-Red 10-bit packed")},
        {PixelFormat_BayerGR12, std::string("Bayer Green-Red 12-bit unpacked")},
        {PixelFormat_BayerGR12p, std::string("Bayer Green-Red 12-bit packed")},
        {PixelFormat_BayerGR16, std::string("Bayer Green-Red 16-bit")},
        {PixelFormat_BayerRG8, std::string("Bayer Red-Green 8-bit")},
        {PixelFormat_BayerRG10, std::string("Bayer Red-Green 10-bit unpacked")},
        {PixelFormat_BayerRG10p, std::string("Bayer Red-Green 10-bit packed")},
        {PixelFormat_BayerRG12, std::string("Bayer Red-Green 12-bit unpacked")},
        {PixelFormat_BayerRG12p, std::string("Bayer Red-Green 12-bit packed")},
        {PixelFormat_BayerRG16, std::string("Bayer Red-Green 16-bit")},
        {PixelFormat_RGBa8, std::string("Red-Green-Blue-alpha 8-bit")},
        {PixelFormat_RGBa10, std::string("Red-Green-Blue-alpha 10-bit unpacked")},
        {PixelFormat_RGBa10p, std::string("Red-Green-Blue-alpha 10-bit packed")},
        {PixelFormat_RGBa12, std::string("Red-Green-Blue-alpha 12-bit unpacked")},
        {PixelFormat_RGBa12p, std::string("Red-Green-Blue-alpha 12-bit packed")},
        {PixelFormat_RGBa14, std::string("Red-Green-Blue-alpha 14-bit unpacked")},
        {PixelFormat_RGBa16, std::string("Red-Green-Blue-alpha 16-bit")},
        {PixelFormat_RGB8, std::string("Red-Green-Blue 8-bit")},
        {PixelFormat_RGB8_Planar, std::string("Red-Green-Blue 8-bit planar")},
        {PixelFormat_RGB10, std::string("Red-Green-Blue 10-bit unpacked")},
        {PixelFormat_RGB10_Planar, std::string("Red-Green-Blue 10-bit unpacked planar")},
        {PixelFormat_RGB10p, std::string("Red-Green-Blue 10-bit packed")},
        {PixelFormat_RGB10p32, std::string("Red-Green-Blue 10-bit packed into 32-bit")},
        {PixelFormat_RGB12, std::string("Red-Green-Blue 12-bit unpacked")},
        {PixelFormat_RGB12_Planar, std::string("Red-Green-Blue 12-bit unpacked planar")},
        {PixelFormat_RGB12p, std::string("Red-Green-Blue 12-bit packed  ")},
        {PixelFormat_RGB14, std::string("Red-Green-Blue 14-bit unpacked")},
        {PixelFormat_RGB16, std::string("Red-Green-Blue 16-bit  ")},
        {PixelFormat_RGB16_Planar, std::string("Red-Green-Blue 16-bit planar")},
        {PixelFormat_RGB565p, std::string("Red-Green-Blue 5/6/5-bit packed")},
        {PixelFormat_BGRa8, std::string("Blue-Green-Red-alpha 8-bit")},
        {PixelFormat_BGRa10, std::string("Blue-Green-Red-alpha 10-bit unpacked")},
        {PixelFormat_BGRa10p, std::string("Blue-Green-Red-alpha 10-bit packed")},
        {PixelFormat_BGRa12, std::string("Blue-Green-Red-alpha 12-bit unpacked")},
        {PixelFormat_BGRa12p, std::string("Blue-Green-Red-alpha 12-bit packed")},
        {PixelFormat_BGRa14, std::string("Blue-Green-Red-alpha 14-bit unpacked")},
        {PixelFormat_BGRa16, std::string("Blue-Green-Red-alpha 16-bit")},
        {PixelFormat_BGR8, std::string("Blue-Green-Red 8-bit")},
        {PixelFormat_BGR10, std::string("Blue-Green-Red 10-bit unpacked")},
        {PixelFormat_BGR10p, std::string("Blue-Green-Red 10-bit packed")},
        {PixelFormat_BGR12, std::string("Blue-Green-Red 12-bit unpacked")},
        {PixelFormat_BGR12p, std::string("Blue-Green-Red 12-bit packed")},
        {PixelFormat_BGR14, std::string("Blue-Green-Red 14-bit unpacked")},
        {PixelFormat_BGR16, std::string("Blue-Green-Red 16-bit")},
        {PixelFormat_BGR565p, std::string("Blue-Green-Red 5/6/5-bit packed")},
        {PixelFormat_R8, std::string("Red 8-bit")},
        {PixelFormat_R10, std::string("Red 10-bit")},
        {PixelFormat_R12, std::string("Red 12-bit")},
        {PixelFormat_R16, std::string("Red 16-bit")},
        {PixelFormat_G8, std::string("Green 8-bit")},
        {PixelFormat_G10, std::string("Green 10-bit")},
        {PixelFormat_G12, std::string("Green 12-bit")},
        {PixelFormat_G16, std::string("Green 16-bit")},
        {PixelFormat_B8, std::string("Blue 8-bit")},
        {PixelFormat_B10, std::string("Blue 10-bit")},
        {PixelFormat_B12, std::string("Blue 12-bit")},
        {PixelFormat_B16, std::string("Blue 16-bit")},
        {PixelFormat_Coord3D_ABC8, std::string("3D coordinate A-B-C 8-bit")},
        {PixelFormat_Coord3D_ABC8_Planar, std::string("3D coordinate A-B-C 8-bit planar")},
        {PixelFormat_Coord3D_ABC10p, std::string("3D coordinate A-B-C 10-bit packed")},
        {PixelFormat_Coord3D_ABC10p_Planar, std::string("3D coordinate A-B-C 10-bit packed planar")},
        {PixelFormat_Coord3D_ABC12p, std::string("3D coordinate A-B-C 12-bit packed")},
        {PixelFormat_Coord3D_ABC12p_Planar, std::string("3D coordinate A-B-C 12-bit packed planar")},
        {PixelFormat_Coord3D_ABC16, std::string("3D coordinate A-B-C 16-bit")},
        {PixelFormat_Coord3D_ABC16_Planar, std::string("3D coordinate A-B-C 16-bit planar")},
        {PixelFormat_Coord3D_ABC32f, std::string("3D coordinate A-B-C 32-bit floating point")},
        {PixelFormat_Coord3D_ABC32f_Planar, std::string("3D coordinate A-B-C 32-bit floating point planar")},
        {PixelFormat_Coord3D_AC8, std::string("3D coordinate A-C 8-bit")},
        {PixelFormat_Coord3D_AC8_Planar, std::string("3D coordinate A-C 8-bit planar")},
        {PixelFormat_Coord3D_AC10p, std::string("3D coordinate A-C 10-bit packed")},
        {PixelFormat_Coord3D_AC10p_Planar, std::string("3D coordinate A-C 10-bit packed planar")},
        {PixelFormat_Coord3D_AC12p, std::string("3D coordinate A-C 12-bit packed")},
        {PixelFormat_Coord3D_AC12p_Planar, std::string("3D coordinate A-C 12-bit packed planar")},
        {PixelFormat_Coord3D_AC16, std::string("3D coordinate A-C 16-bit")},
        {PixelFormat_Coord3D_AC16_Planar, std::string("3D coordinate A-C 16-bit planar")},
        {PixelFormat_Coord3D_AC32f, std::string("3D coordinate A-C 32-bit floating point")},
        {PixelFormat_Coord3D_AC32f_Planar, std::string("3D coordinate A-C 32-bit floating point planar")},
        {PixelFormat_Coord3D_A8, std::string("3D coordinate A 8-bit")},
        {PixelFormat_Coord3D_A10p, std::string("3D coordinate A 10-bit packed")},
        {PixelFormat_Coord3D_A12p, std::string("3D coordinate A 12-bit packed")},
        {PixelFormat_Coord3D_A16, std::string("3D coordinate A 16-bit")},
        {PixelFormat_Coord3D_A32f, std::string("3D coordinate A 32-bit floating point")},
        {PixelFormat_Coord3D_B8, std::string("3D coordinate B 8-bit")},
        {PixelFormat_Coord3D_B10p, std::string("3D coordinate B 10-bit packed")},
        {PixelFormat_Coord3D_B12p, std::string("3D coordinate B 12-bit packed")},
        {PixelFormat_Coord3D_B16, std::string("3D coordinate B 16-bit")},
        {PixelFormat_Coord3D_B32f, std::string("3D coordinate B 32-bit floating point")},
        {PixelFormat_Coord3D_C8, std::string("3D coordinate C 8-bit")},
        {PixelFormat_Coord3D_C10p, std::string("3D coordinate C 10-bit packed")},
        {PixelFormat_Coord3D_C12p, std::string("3D coordinate C 12-bit packed")},
        {PixelFormat_Coord3D_C16, std::string("3D coordinate C 16-bit")},
        {PixelFormat_Coord3D_C32f, std::string("3D coordinate C 32-bit floating point")},
        {PixelFormat_Confidence1, std::string("Confidence 1-bit unpacked")},
        {PixelFormat_Confidence1p, std::string("Confidence 1-bit packed")},
        {PixelFormat_Confidence8, std::string("Confidence 8-bit")},
        {PixelFormat_Confidence16, std::string("Confidence 16-bit")},
        {PixelFormat_Confidence32f, std::string("Confidence 32-bit floating point")},
        {PixelFormat_BiColorBGRG8, std::string("Bi-color Blue/Green - Red/Green 8-bit")},
        {PixelFormat_BiColorBGRG10, std::string("Bi-color Blue/Green - Red/Green 10-bit unpacked")},
        {PixelFormat_BiColorBGRG10p, std::string("Bi-color Blue/Green - Red/Green 10-bit packed")},
        {PixelFormat_BiColorBGRG12, std::string("Bi-color Blue/Green - Red/Green 12-bit unpacked")},
        {PixelFormat_BiColorBGRG12p, std::string("Bi-color Blue/Green - Red/Green 12-bit packed")},
        {PixelFormat_BiColorRGBG8, std::string("Bi-color Red/Green - Blue/Green 8-bit")},
        {PixelFormat_BiColorRGBG10, std::string("Bi-color Red/Green - Blue/Green 10-bit unpacked")},
        {PixelFormat_BiColorRGBG10p, std::string("Bi-color Red/Green - Blue/Green 10-bit packed")},
        {PixelFormat_BiColorRGBG12, std::string("Bi-color Red/Green - Blue/Green 12-bit unpacked")},
        {PixelFormat_BiColorRGBG12p, std::string("Bi-color Red/Green - Blue/Green 12-bit packed")},
        {PixelFormat_SCF1WBWG8, std::string("Sparse Color Filter #1 White-Blue-White-Green 8-bit")},
        {PixelFormat_SCF1WBWG10, std::string("Sparse Color Filter #1 White-Blue-White-Green 10-bit unpacked")},
        {PixelFormat_SCF1WBWG10p, std::string("Sparse Color Filter #1 White-Blue-White-Green 10-bit packed")},
        {PixelFormat_SCF1WBWG12, std::string("Sparse Color Filter #1 White-Blue-White-Green 12-bit unpacked")},
        {PixelFormat_SCF1WBWG12p, std::string("Sparse Color Filter #1 White-Blue-White-Green 12-bit packed")},
        {PixelFormat_SCF1WBWG14, std::string("Sparse Color Filter #1 White-Blue-White-Green 14-bit unpacked")},
        {PixelFormat_SCF1WBWG16, std::string("Sparse Color Filter #1 White-Blue-White-Green 16-bit unpacked")},
        {PixelFormat_SCF1WGWB8, std::string("Sparse Color Filter #1 White-Green-White-Blue 8-bit")},
        {PixelFormat_SCF1WGWB10, std::string("Sparse Color Filter #1 White-Green-White-Blue 10-bit unpacked")},
        {PixelFormat_SCF1WGWB10p, std::string("Sparse Color Filter #1 White-Green-White-Blue 10-bit packed")},
        {PixelFormat_SCF1WGWB12, std::string("Sparse Color Filter #1 White-Green-White-Blue 12-bit unpacked")},
        {PixelFormat_SCF1WGWB12p, std::string("Sparse Color Filter #1 White-Green-White-Blue 12-bit packed")},
        {PixelFormat_SCF1WGWB14, std::string("Sparse Color Filter #1 White-Green-White-Blue 14-bit unpacked")},
        {PixelFormat_SCF1WGWB16, std::string("Sparse Color Filter #1 White-Green-White-Blue 16-bit")},
        {PixelFormat_SCF1WGWR8, std::string("Sparse Color Filter #1 White-Green-White-Red 8-bit")},
        {PixelFormat_SCF1WGWR10, std::string("Sparse Color Filter #1 White-Green-White-Red 10-bit unpacked")},
        {PixelFormat_SCF1WGWR10p, std::string("Sparse Color Filter #1 White-Green-White-Red 10-bit packed")},
        {PixelFormat_SCF1WGWR12, std::string("Sparse Color Filter #1 White-Green-White-Red 12-bit unpacked")},
        {PixelFormat_SCF1WGWR12p, std::string("Sparse Color Filter #1 White-Green-White-Red 12-bit packed")},
        {PixelFormat_SCF1WGWR14, std::string("Sparse Color Filter #1 White-Green-White-Red 14-bit unpacked")},
        {PixelFormat_SCF1WGWR16, std::string("Sparse Color Filter #1 White-Green-White-Red 16-bit")},
        {PixelFormat_SCF1WRWG8, std::string("Sparse Color Filter #1 White-Red-White-Green 8-bit")},
        {PixelFormat_SCF1WRWG10, std::string("Sparse Color Filter #1 White-Red-White-Green 10-bit unpacked")},
        {PixelFormat_SCF1WRWG10p, std::string("Sparse Color Filter #1 White-Red-White-Green 10-bit packed")},
        {PixelFormat_SCF1WRWG12, std::string("Sparse Color Filter #1 White-Red-White-Green 12-bit unpacked")},
        {PixelFormat_SCF1WRWG12p, std::string("Sparse Color Filter #1 White-Red-White-Green 12-bit packed")},
        {PixelFormat_SCF1WRWG14, std::string("Sparse Color Filter #1 White-Red-White-Green 14-bit unpacked")},
        {PixelFormat_SCF1WRWG16, std::string("Sparse Color Filter #1 White-Red-White-Green 16-bit")},
        {PixelFormat_YCbCr8, std::string("YCbCr 4:4:4 8-bit")},
        {PixelFormat_YCbCr8_CbYCr, std::string("YCbCr 4:4:4 8-bit")},
        {PixelFormat_YCbCr10_CbYCr, std::string("YCbCr 4:4:4 10-bit unpacked")},
        {PixelFormat_YCbCr10p_CbYCr, std::string("YCbCr 4:4:4 10-bit packed")},
        {PixelFormat_YCbCr12_CbYCr, std::string("YCbCr 4:4:4 12-bit unpacked")},
        {PixelFormat_YCbCr12p_CbYCr, std::string("YCbCr 4:4:4 12-bit packed")},
        {PixelFormat_YCbCr411_8, std::string("YCbCr 4:1:1 8-bit")},
        {PixelFormat_YCbCr411_8_CbYYCrYY, std::string("YCbCr 4:1:1 8-bit")},
        {PixelFormat_YCbCr422_8, std::string("YCbCr 4:2:2 8-bit")},
        {PixelFormat_YCbCr422_8_CbYCrY, std::string("YCbCr 4:2:2 8-bit")},
        {PixelFormat_YCbCr422_10, std::string("YCbCr 4:2:2 10-bit unpacked")},
        {PixelFormat_YCbCr422_10_CbYCrY, std::string("YCbCr 4:2:2 10-bit unpacked")},
        {PixelFormat_YCbCr422_10p, std::string("YCbCr 4:2:2 10-bit packed")},
        {PixelFormat_YCbCr422_10p_CbYCrY, std::string("YCbCr 4:2:2 10-bit packed")},
        {PixelFormat_YCbCr422_12, std::string("YCbCr 4:2:2 12-bit unpacked")},
        {PixelFormat_YCbCr422_12_CbYCrY, std::string("YCbCr 4:2:2 12-bit unpacked")},
        {PixelFormat_YCbCr422_12p, std::string("YCbCr 4:2:2 12-bit packed")},
        {PixelFormat_YCbCr422_12p_CbYCrY, std::string("YCbCr 4:2:2 12-bit packed")},
        {PixelFormat_YCbCr601_8_CbYCr, std::string("YCbCr 4:4:4 8-bit BT.601")},
        {PixelFormat_YCbCr601_10_CbYCr, std::string("YCbCr 4:4:4 10-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_10p_CbYCr, std::string("YCbCr 4:4:4 10-bit packed BT.601")},
        {PixelFormat_YCbCr601_12_CbYCr, std::string("YCbCr 4:4:4 12-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_12p_CbYCr, std::string("YCbCr 4:4:4 12-bit packed BT.601")},
        {PixelFormat_YCbCr601_411_8_CbYYCrYY, std::string("YCbCr 4:1:1 8-bit BT.601")},
        {PixelFormat_YCbCr601_422_8, std::string("YCbCr 4:2:2 8-bit BT.601")},
        {PixelFormat_YCbCr601_422_8_CbYCrY, std::string("YCbCr 4:2:2 8-bit BT.601")},
        {PixelFormat_YCbCr601_422_10, std::string("YCbCr 4:2:2 10-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_10_CbYCrY, std::string("YCbCr 4:2:2 10-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_10p, std::string("YCbCr 4:2:2 10-bit packed BT.601")},
        {PixelFormat_YCbCr601_422_10p_CbYCrY, std::string("YCbCr 4:2:2 10-bit packed BT.601")},
        {PixelFormat_YCbCr601_422_12, std::string("YCbCr 4:2:2 12-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_12_CbYCrY, std::string("YCbCr 4:2:2 12-bit unpacked BT.601")},
        {PixelFormat_YCbCr601_422_12p, std::string("YCbCr 4:2:2 12-bit packed BT.601")},
        {PixelFormat_YCbCr601_422_12p_CbYCrY, std::string("YCbCr 4:2:2 12-bit packed BT.601")},
        {PixelFormat_YCbCr709_8_CbYCr, std::string("YCbCr 4:4:4 8-bit BT.709")},
        {PixelFormat_YCbCr709_10_CbYCr, std::string("YCbCr 4:4:4 10-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_10p_CbYCr, std::string("YCbCr 4:4:4 10-bit packed BT.709")},
        {PixelFormat_YCbCr709_12_CbYCr, std::string("YCbCr 4:4:4 12-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_12p_CbYCr, std::string("YCbCr 4:4:4 12-bit packed BT.709")},
        {PixelFormat_YCbCr709_411_8_CbYYCrYY, std::string("YCbCr 4:1:1 8-bit BT.709")},
        {PixelFormat_YCbCr709_422_8, std::string("YCbCr 4:2:2 8-bit BT.709")},
        {PixelFormat_YCbCr709_422_8_CbYCrY, std::string("YCbCr 4:2:2 8-bit BT.709")},
        {PixelFormat_YCbCr709_422_10, std::string("YCbCr 4:2:2 10-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_10_CbYCrY, std::string("YCbCr 4:2:2 10-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_10p, std::string("YCbCr 4:2:2 10-bit packed BT.709")},
        {PixelFormat_YCbCr709_422_10p_CbYCrY, std::string("YCbCr 4:2:2 10-bit packed BT.709")},
        {PixelFormat_YCbCr709_422_12, std::string("YCbCr 4:2:2 12-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_12_CbYCrY, std::string("YCbCr 4:2:2 12-bit unpacked BT.709")},
        {PixelFormat_YCbCr709_422_12p, std::string("YCbCr 4:2:2 12-bit packed BT.709")},
        {PixelFormat_YCbCr709_422_12p_CbYCrY, std::string("YCbCr 4:2:2 12-bit packed BT.709")},
        {PixelFormat_YUV8_UYV, std::string("YUV 4:4:4 8-bit")},
        {PixelFormat_YUV411_8_UYYVYY, std::string("YUV 4:1:1 8-bit")},
        {PixelFormat_YUV422_8, std::string("YUV 4:2:2 8-bit")},
        {PixelFormat_YUV422_8_UYVY, std::string("YUV 4:2:2 8-bit")},
        {PixelFormat_Mono10Packed, std::string("Pixel format set Mono 10 Packed.")},
        {PixelFormat_Mono12Packed, std::string("Pixel format set Mono 12 Packed.")},
        {PixelFormat_BayerGR12Packed, std::string("Pixel format set BayerGR 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_BayerRG12Packed, std::string("Pixel format set BayerRG 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_BayerGB12Packed, std::string("Pixel format set to BayerGB 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_BayerBG12Packed, std::string("Pixel format set to BayerBG 12 Packed. (GigE Vision 2.0)")},
        {PixelFormat_YUV411Packed, std::string("Pixel format set to YUV411 Packed.")},
        {PixelFormat_YUV422Packed, std::string("Pixel format set to YUV 422 Packed.")},
        {PixelFormat_YUV444Packed, std::string("Pixel format set to YUV 444 Packed.")},
        {PixelFormat_RGB8Packed, std::string("Pixel format set to RGB 8 Packed.")},
        {PixelFormat_Raw16, std::string("Raw 16 bit.")},
        {PixelFormat_Raw8, std::string("Raw bit.")},
        {PixelFormat_BayerGR10Packed, std::string("Bayer GR 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_BayerRG10Packed, std::string("Bayer RG 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_BayerGB10Packed, std::string("Bayer GB 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_BayerBG10Packed, std::string("Bayer BG 10 bit packed (GigE Vision 2.0).")},
        {PixelFormat_R12_Jpeg, std::string("Red 12-bit JPEG.")},
        {PixelFormat_GR12_Jpeg, std::string("Green Red 12-bit JPEG.")},
        {PixelFormat_GB12_Jpeg, std::string("Green Blue 12-bit JPEG.")},
        {PixelFormat_B12_Jpeg, std::string("Blue 12-bit packed JPEG.")},
        {UNKNOWN_PIXELFORMAT, std::string("Unknown Pixel Format")},
    };

    std::string getPixelFormatString_spin(spinPixelFormatEnums format)
    {
        if (pixelFormatToStringMap_spin.count(format) != 0)
        {
            return pixelFormatToStringMap_spin[format];
        }
        else
        {
            return std::string("unknown spin pixel format");
        }
    }

    void printAllPixelFormatStrings_spin()
    {
        std::cout << std::endl;
        for (auto kv : pixelFormatToStringMap_spin)
        {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }
        std::cout << std::endl;
    }

} // namespece bias


#endif // #ifdef WITH_SPIN
