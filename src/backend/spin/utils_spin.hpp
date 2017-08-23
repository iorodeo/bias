#ifdef WITH_SPIN
#ifndef BIAS_UTILS_SPIN_HPP
#define BIAS_UTILS_SPIN_HPP

#include "basic_types.hpp"
#include "property.hpp"
#include "format7.hpp"
#include <string>
#include "SpinnakerC.h"

namespace bias 
{
    struct ImageInfo_spin
    {
        size_t xpad;
        size_t ypad;
        size_t cols;
        size_t rows;
        size_t stride;
        void *dataPtr;
    };


    ImageInfo_spin getImageInfo_spin(spinImage hImage);


    //// Image conversion - for mapping from Spinnaker to opencv 
    //// -----------------------------------------------------------------------

    //spinPixelFormat getSuitablePixelFormat(spinPixelFormat pixFormat);

    //int getCompatibleOpencvFormat(spinPixelFormat pixFormat);


    //// Conversion from BIAS types to Spinnaker  types
    //// ------------------------------------------------------------------------
    //
    //spinPropertyType convertPropertyType_to_spin(PropertyType propType);

    //spinFrameRate convertFrameRate_to_spin(FrameRate frmRate);

    //spinVideoMode convertVideoMode_to_spin(VideoMode vidMode);

    //spinMode convertImageMode_to_spin(ImageMode imgMode);

    //spinProperty convertProperty_to_spin(Property prop);

    //spinPixelFormat convertPixelFormat_to_spin(PixelFormat pixFormat);

    //spinFormat7ImageSettings convertFormat7Settings_to_spin(Format7Settings settings);


    //// Conversion from Spinnaker types to BIAS types
    //// ------------------------------------------------------------------------

    //PropertyType convertPropertyType_from_spin(spinPropertyType propType_spin);

    //VideoMode convertVideoMode_from_spin(spinVideoMode vidMode_spin);

    //FrameRate convertFrameRate_from_spin(spinFrameRate frmRate_spin);

    //ImageMode convertImageMode_from_spin(spinMode mode_spin);

    //Property convertProperty_from_spin(spinProperty prop_spin); 

    //PropertyInfo convertPropertyInfo_from_spin(spinPropertyInfo propInfo_spin); 

    //PixelFormat convertPixelFormat_from_spin(spinPixelFormat pixFormat_spin);

    //Format7Settings convertFormat7Settings_from_spin(spinFormat7ImageSettings settings_spin);


    //// Print functions for Spinnaker configurations, settings and info
    ////-------------------------------------------------------------------------
    //
    //void printImageInfo_spin(spinImage &image);

    //void printFormat7ImageSettings_spin(spinFormat7ImageSettings &imageSettings);

    //void printFormat7PacketSettings_spin(unsigned int packetSize, float percentage);

    //void printFormat7Configuration_spin(
    //        spinFormat7ImageSettings &imageSettings,
    //        unsigned int packetSize,
    //        float percentage
    //        );

    //void printFormat7Info_spin(spinFormat7Info &format7Info);

    //void printFormat7PacketInfo_spin(spinFormat7PacketInfo &packetInfo);

    //void printPropertyInfo_spin(spinPropertyInfo &propInfo);

    //void printProperty_spin(spinProperty &prop);

    //void printTriggerMode_spin(spinTriggerMode &trigMode);

    //void printTriggerModeInfo_spin(spinTriggerModeInfo &trigModeInfo);

    //void printConfiguration_spin(spinConfig &config);


    //// Spinnaker enumeration to string converstions
    //// ------------------------------------------------------------------------
    //
    //std::string getInterfaceTypeString_spin(spinInterfaceType ifaceType);

    //std::string getPixelFormatString_spin(spinPixelFormat foramt);

    //std::string getBayerTileFormatString_spin(spinBayerTileFormat bayerFormat);

    //std::string getVideoModeString_spin(spinVideoMode vidMode);

    //std::string getFrameRateString_spin(spinFrameRate frmRate);

    //std::string getModeString_spin(spinMode mode);

    //std::string getPropertyTypeString_spin(spinPropertyType propType);

    //std::string getGrabTimeoutString_spin(spinGrabTimeout grabTimeout);
    //
    //std::string getGrabTimeoutString_spin(int grabTimeout);

    //std::string getGrabModeString_spin(spinGrabMode grabMode);

    //std::string getBusSpeedString_spin(spinBusSpeed busSpeed);

    //std::string getBandwidthAllocationString_spin(spinBandwidthAllocation bandwidthAllocation);



} // namespace BIAS

#endif // #ifndef BIAS_UTILS_SPIN_HPP
#endif // #ifdef WITH_SPIN
