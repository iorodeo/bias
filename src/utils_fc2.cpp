#ifdef WITH_FC2
#include "utils_fc2.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>
#include <bitset>

namespace bias {
    
    // Conversion from BIAS types to FlyCapture2 types
    // ------------------------------------------------------------------------

    fc2PropertyType convertPropertyType_to_fc2(PropertyType propertyType)
    {
        switch (propertyType)
        {
            case PROPERTY_TYPE_BRIGHTNESS: 
                return FC2_BRIGHTNESS;
                break;

            case PROPERTY_TYPE_AUTO_EXPOSURE:
                return FC2_AUTO_EXPOSURE;
                break;

            case PROPERTY_TYPE_SHARPNESS:
                return FC2_SHARPNESS;
                break;

            case PROPERTY_TYPE_WHITE_BALANCE:
                return FC2_WHITE_BALANCE;
                break;

            case PROPERTY_TYPE_HUE:
                return FC2_HUE;
                break;

            case PROPERTY_TYPE_SATURATION:
                return FC2_SATURATION;
                break;

            case PROPERTY_TYPE_GAMMA:
                return FC2_GAMMA;
                break;

            case PROPERTY_TYPE_IRIS:
                return FC2_IRIS;
                break;

            case PROPERTY_TYPE_FOCUS:
                return FC2_FOCUS;
                break;

            case PROPERTY_TYPE_ZOOM:
                return FC2_ZOOM;
                break;

            case PROPERTY_TYPE_PAN:
                return FC2_PAN;
                break;

            case PROPERTY_TYPE_TILT:
                return FC2_TILT;
                break;

            case PROPERTY_TYPE_SHUTTER:
                return FC2_SHUTTER;
                break;

            case PROPERTY_TYPE_GAIN:
                return FC2_GAIN;
                break;

            case PROPERTY_TYPE_TRIGGER_MODE:
                return FC2_TRIGGER_MODE;
                break;

            case PROPERTY_TYPE_TRIGGER_DELAY:
                return FC2_TRIGGER_DELAY;
                break;

            case PROPERTY_TYPE_FRAME_RATE:
                return FC2_FRAME_RATE;
                break;

            case PROPERTY_TYPE_TEMPERATURE:
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

    fc2FrameRate convertFrameRate_to_fc2(FrameRate frameRate)
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

    fc2VideoMode convertVideoMode_to_fc2(VideoMode videoMode)
    {
        switch(videoMode)
        {
            case VIDEOMODE_160x120YUV444: 
                return FC2_VIDEOMODE_160x120YUV444;
                break;

            case VIDEOMODE_320x240YUV422: 
                return FC2_VIDEOMODE_320x240YUV422;
                break;

            case VIDEOMODE_640x480YUV411: 
                return FC2_VIDEOMODE_640x480YUV411;
                break;

            case VIDEOMODE_640x480YUV422: 
                return FC2_VIDEOMODE_640x480YUV422;
                break;

            case VIDEOMODE_640x480RGB: 
                return FC2_VIDEOMODE_640x480RGB;
                break;

            case VIDEOMODE_640x480Y8: 
                return FC2_VIDEOMODE_640x480Y8;
                break;

            case VIDEOMODE_640x480Y16: 
                return FC2_VIDEOMODE_640x480Y16;
                break;

            case VIDEOMODE_800x600YUV422: 
                return FC2_VIDEOMODE_800x600YUV422;
                break;

            case VIDEOMODE_800x600RGB: 
                return FC2_VIDEOMODE_800x600RGB;
                break;

            case VIDEOMODE_800x600Y8: 
                return FC2_VIDEOMODE_800x600Y8;
                break;

            case VIDEOMODE_800x600Y16: 
                return FC2_VIDEOMODE_800x600Y16;
                break;

            case VIDEOMODE_1024x768YUV422: 
                return FC2_VIDEOMODE_1024x768YUV422;
                break;

            case VIDEOMODE_1024x768RGB: 
                return FC2_VIDEOMODE_1024x768RGB;
                break;

            case VIDEOMODE_1024x768Y8: 
                return FC2_VIDEOMODE_1024x768Y8;
                break;

            case VIDEOMODE_1024x768Y16: 
                return FC2_VIDEOMODE_1024x768Y16;
                break;

            case VIDEOMODE_1280x960YUV422: 
                return FC2_VIDEOMODE_1280x960YUV422;
                break;

            case VIDEOMODE_1280x960RGB: 
                return FC2_VIDEOMODE_1280x960RGB;
                break;

            case VIDEOMODE_1280x960Y8: 
                return FC2_VIDEOMODE_1280x960Y8;
                break;

            case VIDEOMODE_1280x960Y16: 
                return FC2_VIDEOMODE_1280x960Y16;
                break;

            case VIDEOMODE_1600x1200YUV422: 
                return FC2_VIDEOMODE_1600x1200YUV422;
                break;

            case VIDEOMODE_1600x1200RGB: 
                return FC2_VIDEOMODE_1600x1200RGB;
                break;

            case VIDEOMODE_1600x1200Y8: 
                return FC2_VIDEOMODE_1600x1200Y8;
                break;

            case VIDEOMODE_1600x1200Y16: 
                return FC2_VIDEOMODE_1600x1200Y16;
                break;

            case VIDEOMODE_FORMAT7: 
                return FC2_VIDEOMODE_FORMAT7;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert video mode to FlyCapture2 video mode";
                    throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
                }
                break;
        }
        return FC2_NUM_VIDEOMODES;
    }

    fc2Mode convertImageMode_to_fc2(ImageMode mode)
    {
        switch(mode)
        {
            case IMAGEMODE_0:
                return FC2_MODE_0;
                break;

            case IMAGEMODE_1:
                return FC2_MODE_1;
                break;

            case IMAGEMODE_2:
                return FC2_MODE_2;
                break;

            case IMAGEMODE_3:
                return FC2_MODE_3;
                break;

            case IMAGEMODE_4:
                return FC2_MODE_4;
                break;

            case IMAGEMODE_5:
                return FC2_MODE_5;
                break;

            case IMAGEMODE_6:
                return FC2_MODE_6;
                break;

            case IMAGEMODE_7:
                return FC2_MODE_7;
                break;

            case IMAGEMODE_8:
                return FC2_MODE_8;
                break;

            case IMAGEMODE_9:
                return FC2_MODE_9;
                break;

            case IMAGEMODE_10:
                return FC2_MODE_10;
                break;

            case IMAGEMODE_11:
                return FC2_MODE_11;
                break;

            case IMAGEMODE_12:
                return FC2_MODE_12;
                break;

            case IMAGEMODE_13:
                return FC2_MODE_13;
                break;

            case IMAGEMODE_14:
                return FC2_MODE_14;
                break;

            case IMAGEMODE_15:
                return FC2_MODE_15;
                break;

            case IMAGEMODE_16:
                return FC2_MODE_16;
                break;

            case IMAGEMODE_17:
                return FC2_MODE_17;
                break;

            case IMAGEMODE_18:
                return FC2_MODE_18;
                break;

            case IMAGEMODE_19:
                return FC2_MODE_19;
                break;

            case IMAGEMODE_20:
                return FC2_MODE_20;
                break;

            case IMAGEMODE_21:
                return FC2_MODE_21;
                break;

            case IMAGEMODE_22:
                return FC2_MODE_22;
                break;

            case IMAGEMODE_23:
                return FC2_MODE_23;
                break;

            case IMAGEMODE_24:
                return FC2_MODE_24;
                break;

            case IMAGEMODE_25:
                return FC2_MODE_25;
                break;

            case IMAGEMODE_26:
                return FC2_MODE_26;
                break;

            case IMAGEMODE_27:
                return FC2_MODE_27;
                break;

            case IMAGEMODE_28:
                return FC2_MODE_28;
                break;

            case IMAGEMODE_29:
                return FC2_MODE_29;
                break;

            case IMAGEMODE_30:
                return FC2_MODE_30;
                break;

            case IMAGEMODE_31:
                return FC2_MODE_31;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert image mode to FlyCapture2 ";
                    ssError << "imaging mode";
                    throw RuntimeError(ERROR_FC2_CONVERT_IMAGEMODE, ssError.str());
                }
                break;

        }
        return FC2_NUM_MODES;
    }

    // Conversion from FlyCapture2 types to BIAS types
    // ------------------------------------------------------------------------

    PropertyType convertPropertyType_from_fc2(fc2PropertyType propertyType_fc2)
    {
        switch(propertyType_fc2)
        {
            case FC2_BRIGHTNESS:
                return PROPERTY_TYPE_BRIGHTNESS;
                break;

            case FC2_AUTO_EXPOSURE:
                return PROPERTY_TYPE_AUTO_EXPOSURE;
                break;

            case FC2_SHARPNESS:
                return PROPERTY_TYPE_SHARPNESS;
                break;

            case FC2_WHITE_BALANCE:
                return PROPERTY_TYPE_WHITE_BALANCE;
                break;

            case FC2_HUE:
                return PROPERTY_TYPE_HUE;
                break;

            case FC2_SATURATION:
                return PROPERTY_TYPE_SATURATION;
                break;

            case FC2_GAMMA:
                return PROPERTY_TYPE_GAMMA;
                break;

            case FC2_IRIS:
                return PROPERTY_TYPE_IRIS;
                break;

            case FC2_FOCUS:
                return PROPERTY_TYPE_FOCUS;
                break;

            case FC2_ZOOM:
                return PROPERTY_TYPE_ZOOM;
                break;

            case FC2_PAN:
                return PROPERTY_TYPE_PAN;
                break;

            case FC2_TILT:
                return PROPERTY_TYPE_TILT;
                break;

            case FC2_SHUTTER:
                return PROPERTY_TYPE_SHUTTER;
                break;

            case FC2_GAIN:
                return PROPERTY_TYPE_GAIN;
                break;

            case FC2_TRIGGER_MODE:
                return PROPERTY_TYPE_TRIGGER_MODE;
                break;

            case FC2_TRIGGER_DELAY:
                return PROPERTY_TYPE_TRIGGER_DELAY;
                break;

            case FC2_FRAME_RATE:
                return PROPERTY_TYPE_FRAME_RATE;
                break;

            case FC2_TEMPERATURE:
                return PROPERTY_TYPE_TEMPERATURE;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert FlyCapture2 PropertyType";
                    throw RuntimeError(ERROR_FC2_CONVERT_PROPERTY_TYPE, ssError.str());
                }
                break;


        }
        return PROPERTY_TYPE_UNSPECIFIED;
    }

    VideoMode convertVideoMode_from_fc2(fc2VideoMode videoMode_fc2)
    {
        switch (videoMode_fc2)
        {
            case FC2_VIDEOMODE_160x120YUV444:
                return VIDEOMODE_160x120YUV444;
                break;

            case FC2_VIDEOMODE_320x240YUV422:
                return VIDEOMODE_320x240YUV422;
                break;

            case FC2_VIDEOMODE_640x480YUV411:
                return VIDEOMODE_640x480YUV411;
                break;

            case FC2_VIDEOMODE_640x480YUV422:
                return VIDEOMODE_640x480YUV422;
                break;

            case FC2_VIDEOMODE_640x480RGB:
                return VIDEOMODE_640x480RGB;
                break;

            case FC2_VIDEOMODE_640x480Y8:
                return VIDEOMODE_640x480Y8;
                break;

            case FC2_VIDEOMODE_640x480Y16:
                return VIDEOMODE_640x480Y16;
                break;

            case FC2_VIDEOMODE_800x600YUV422:
                return VIDEOMODE_800x600YUV422;
                break;

            case FC2_VIDEOMODE_800x600RGB:
                return VIDEOMODE_800x600RGB;
                break;

            case FC2_VIDEOMODE_800x600Y8:
                return VIDEOMODE_800x600Y8;
                break;

            case FC2_VIDEOMODE_800x600Y16:
                return VIDEOMODE_800x600Y16;
                break;

            case FC2_VIDEOMODE_1024x768YUV422:
                return VIDEOMODE_1024x768YUV422;
                break;

            case FC2_VIDEOMODE_1024x768RGB:
                return VIDEOMODE_1024x768RGB;
                break;

            case FC2_VIDEOMODE_1024x768Y8:
                return VIDEOMODE_1024x768Y8;
                break;

            case FC2_VIDEOMODE_1024x768Y16:
                return VIDEOMODE_1024x768Y16;
                break;

            case FC2_VIDEOMODE_1280x960YUV422:
                return VIDEOMODE_1280x960YUV422;
                break;

            case FC2_VIDEOMODE_1280x960RGB:
                return VIDEOMODE_1280x960RGB;
                break;

            case FC2_VIDEOMODE_1280x960Y8:
                return VIDEOMODE_1280x960Y8;
                break;

            case FC2_VIDEOMODE_1280x960Y16:
                return VIDEOMODE_1280x960Y16;
                break;

            case FC2_VIDEOMODE_1600x1200YUV422:
                return VIDEOMODE_1600x1200YUV422;
                break;

            case FC2_VIDEOMODE_1600x1200RGB:
                return VIDEOMODE_1600x1200RGB;
                break;

            case FC2_VIDEOMODE_1600x1200Y8:
                return VIDEOMODE_1600x1200Y8;
                break;

            case FC2_VIDEOMODE_1600x1200Y16:
                return VIDEOMODE_1600x1200Y16;
                break;

            case FC2_VIDEOMODE_FORMAT7:
                return VIDEOMODE_FORMAT7;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert FlyCapture2 VideoMode";
                    throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
                }
                break;

        }
        return NUMBER_OF_VIDEOMODE;
    }

    FrameRate convertFrameRate_from_fc2(fc2FrameRate frameRate_fc2)
    {
        switch (frameRate_fc2)
        {
            case FC2_FRAMERATE_1_875:
                return FRAMERATE_1_875;
                break;

            case FC2_FRAMERATE_3_75:
                return FRAMERATE_3_75;
                break;

            case FC2_FRAMERATE_7_5:
                return FRAMERATE_7_5;
                break;

            case FC2_FRAMERATE_15:
                return FRAMERATE_15;
                break;

            case FC2_FRAMERATE_30:
                return FRAMERATE_30;
                break;

            case FC2_FRAMERATE_60:
                return FRAMERATE_60;
                break;

            case FC2_FRAMERATE_120:
                return FRAMERATE_120;
                break;

            case FC2_FRAMERATE_240:
                return FRAMERATE_240;
                break;

            case FC2_FRAMERATE_FORMAT7:
                return FRAMERATE_FORMAT7;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert FlyCapture2 FrameRate";
                    throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
                }
                break;

        }
        return NUMBER_OF_FRAMERATE;
    }

    ImageMode convertImageMode_from_fc2(fc2Mode mode_fc2)
    {
        switch(mode_fc2) 
        {
            case FC2_MODE_0:
                return IMAGEMODE_0;
                break;

            case FC2_MODE_1:
                return IMAGEMODE_1;
                break;

            case FC2_MODE_2:
                return IMAGEMODE_2;
                break;

            case FC2_MODE_3:
                return IMAGEMODE_3;
                break;

            case FC2_MODE_4:
                return IMAGEMODE_4;
                break;

            case FC2_MODE_5:
                return IMAGEMODE_5;
                break;

            case FC2_MODE_6:
                return IMAGEMODE_6;
                break;

            case FC2_MODE_7:
                return IMAGEMODE_7;
                break;

            case FC2_MODE_8:
                return IMAGEMODE_8;
                break;

            case FC2_MODE_9:
                return IMAGEMODE_9;
                break;

            case FC2_MODE_10:
                return IMAGEMODE_10;
                break;

            case FC2_MODE_11:
                return IMAGEMODE_11;
                break;

            case FC2_MODE_12:
                return IMAGEMODE_12;
                break;

            case FC2_MODE_13:
                return IMAGEMODE_13;
                break;

            case FC2_MODE_14:
                return IMAGEMODE_14;
                break;

            case FC2_MODE_15:
                return IMAGEMODE_15;
                break;

            case FC2_MODE_16:
                return IMAGEMODE_16;
                break;

            case FC2_MODE_17:
                return IMAGEMODE_17;
                break;

            case FC2_MODE_18:
                return IMAGEMODE_18;
                break;

            case FC2_MODE_19:
                return IMAGEMODE_19;
                break;

            case FC2_MODE_20:
                return IMAGEMODE_20;
                break;

            case FC2_MODE_21:
                return IMAGEMODE_21;
                break;

            case FC2_MODE_22:
                return IMAGEMODE_22;
                break;

            case FC2_MODE_23:
                return IMAGEMODE_23;
                break;

            case FC2_MODE_24:
                return IMAGEMODE_24;
                break;

            case FC2_MODE_25:
                return IMAGEMODE_25;
                break;

            case FC2_MODE_26:
                return IMAGEMODE_26;
                break;

            case FC2_MODE_27:
                return IMAGEMODE_27;
                break;

            case FC2_MODE_28:
                return IMAGEMODE_28;
                break;

            case FC2_MODE_29:
                return IMAGEMODE_29;
                break;

            case FC2_MODE_30:
                return IMAGEMODE_30;
                break;

            case FC2_MODE_31:
                return IMAGEMODE_31;
                break;

            default:
                {
                    std::stringstream ssError;
                    ssError << __PRETTY_FUNCTION__;
                    ssError << ": unable to convert FlyCapture2 Mode";
                    throw RuntimeError(ERROR_FC2_CONVERT_VIDEOMODE, ssError.str());
                }
                break;

        }
        return NUMBER_OF_IMAGEMODE;
    }

    Property convertProperty_from_fc2(
            fc2Property property_fc2, 
            fc2PropertyInfo propertyInfo_fc2
            )
    {
        Property property;
        property.type = convertPropertyType_from_fc2(property_fc2.type);
        property.on = bool(property_fc2.onOff);
        property.autoActive = bool(property_fc2.autoManualMode);
        property.value = property_fc2.valueA;
        property.absoluteValue = property_fc2.absValue;
        property.units = std::string(propertyInfo_fc2.pUnits);
        property.unitsAbbr = std::string(propertyInfo_fc2.pUnitAbbr);
        property.present = bool(propertyInfo_fc2.present);
        property.autoCapable = bool(propertyInfo_fc2.autoSupported);
        property.manualCapable = bool(propertyInfo_fc2.manualSupported);
        property.absoluteCapable = bool(propertyInfo_fc2.absValSupported);
        property.onePushCapable = bool(propertyInfo_fc2.onePushSupported);
        property.onOffCapable = bool(propertyInfo_fc2.onOffSupported);
        property.haveUnits =  bool(property.units.length());
        property.minValue = propertyInfo_fc2.min;
        property.maxValue = propertyInfo_fc2.max;
        property.minAbsoluteValue = propertyInfo_fc2.absMin;
        property.maxAbsoluteValue = propertyInfo_fc2.absMax;
        return property;
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
            std::cout << "   [" << i << "] = " << packetInfo.reserved[i] << std::endl;
        }
        std::cout << std::endl;
    }

    void printPropertyInfo_fc2(fc2PropertyInfo &propInfo)
    {
        std::cout << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << "FlyCapture2 Property Info" << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " type:                " << getPropertyTypeString_fc2(propInfo.type) << std::endl;
        std::cout << std::boolalpha;
        std::cout << " present:             " << bool(propInfo.present) << std::endl;
        std::cout << " autoSupported:       " << bool(propInfo.autoSupported) << std::endl;
        std::cout << " manualSupported:     " << bool(propInfo.manualSupported) << std::endl;
        std::cout << " onOffSupported:      " << bool(propInfo.onOffSupported) << std::endl;
        std::cout << " onePushSupported:    " << bool(propInfo.onePushSupported) << std::endl;
        std::cout << " absValSupported:     " << bool(propInfo.absValSupported) << std::endl;
        std::cout << " readOutSupported:    " << bool(propInfo.absValSupported) << std::endl;
        std::cout << std::noboolalpha;
        std::cout << " min (unsigned int):  " << propInfo.min << std::endl;
        std::cout << " max (unsigned int):  " << propInfo.max << std::endl;
        std::cout << " absMin:              " << propInfo.absMin << std::endl;
        std::cout << " absMax:              " << propInfo.absMax << std::endl;
        std::cout << " pUnits:              " << propInfo.pUnits << std::endl;
        std::cout << " pUnitAbbr:           " << propInfo.pUnitAbbr << std::endl;
        std::cout << " reserved             " << std::endl;
        for (int i=0; i<8; i++)
        {
            std::cout << "   [" << i << "] = " << propInfo.reserved[i] << std::endl; 
        }
        std::cout << std::endl; 
    }

    void printProperty_fc2(fc2Property &property)
    {
        std::cout << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << "FlyCapture2 Property" << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << std::endl;
        std::cout << " type:            " << getPropertyTypeString_fc2(property.type) << std::endl;
        std::cout << std::boolalpha << std::endl;
        std::cout << " present:         " << bool(property.present) << std::endl; 
        std::cout << " absControl:      " << bool(property.absControl) << std::endl;
        std::cout << " onePush:         " << bool(property.onePush) << std::endl;
        std::cout << " onOff:           " << bool(property.onOff) << std::endl;
        std::cout << " autoManualMode:  " << bool(property.autoManualMode) << std::endl;
        std::cout << std::noboolalpha;
        std::cout << " valueA:          " << property.valueA << std::endl;
        std::cout << " valueB:          " << property.valueB << std::endl;
        std::cout << " absValue:        " << property.absValue << std::endl;
        for (int i=0; i<8; i++)
        {
            std::cout << "   [" << i << "] = " << property.reserved[i] << std::endl; 
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

    std::string getPropertyTypeString_fc2(fc2PropertyType propType)
    {
        switch (propType)
        {
            case FC2_BRIGHTNESS:
                return std::string("FC2_BRIGHTNESS");
                break;

            case FC2_AUTO_EXPOSURE:
                return std::string("FC2_AUTO_EXPOSURE");
                break;

            case FC2_SHARPNESS:
                return std::string("FC2_SHARPNESS");
                break;

            case FC2_WHITE_BALANCE:
                return std::string("FC2_WHITE_BALANCE");
                break;

            case FC2_HUE:
                return std::string("FC2_HUE");
                break;

            case FC2_SATURATION:
                return std::string("FC2_SATURATION");
                break;

            case FC2_GAMMA:
                return std::string("FC2_GAMMA");
                break;

            case FC2_IRIS:
                return std::string("FC2_IRIS");
                break;

            case FC2_FOCUS:
                return std::string("FC2_FOCUS");
                break;

            case FC2_ZOOM:
                return std::string("FC2_ZOOM");
                break;

            case FC2_PAN:
                return std::string("FC2_PAN");
                break;

            case FC2_TILT:
                return std::string("FC2_TILT");
                break;

            case FC2_SHUTTER:
                return std::string("FC2_SHUTTER");
                break;

            case FC2_GAIN:
                return std::string("FC2_GAIN");
                break;

            case FC2_TRIGGER_MODE:
                return std::string("FC2_TRIGGER_MODE");
                break;

            case FC2_TRIGGER_DELAY:
                return std::string("FC2_TRIGGER_DELAY");
                break;

            case FC2_FRAME_RATE:
                return std::string("FC2_FRAME_RATE");
                break;

            case FC2_TEMPERATURE:
                return std::string("FC2_TEMPERATURE");
                break;

            case FC2_UNSPECIFIED_PROPERTY_TYPE:
                return std::string("FC2_UNSPECIFIED_PROPERTY_TYPE");
                break;
        }
    }
}

#endif // #ifdef WITH_FC2
