#ifdef WITH_SPIN
#ifndef BIAS_CAMERA_INFO_SPIN_HPP
#define BIAS_CAMERA_INFO_SPIN_HPP

#include <string>
#include <map>
#include "SpinnakerC.h"
#include "basic_types.hpp"

namespace bias {


    class CameraInfo_spin
    {
        public:

            CameraInfo_spin();

            std::string toString();
            void print();

            std::string guid();
            void setGuid(std::string guid);

            std::string serialNumber();
            void setSerialNumber(std::string serialNumber);

            std::string vendorId();
            void setVendorId(std::string vendorId);

            std::string modelId();
            void setModelId(std::string modelId);

            std::string modelName();
            void setModelName(std::string modelName);

            std::string vendorName();
            void setVendorName(std::string vendorName);


        protected:

            std::string guid_;
            std::string serialNumber_;
            std::string vendorId_;
            std::string modelId_;
            std::string modelName_;
            std::string vendorName_;


    };

}

#endif // #ifndef BIAS_CAMERA_INFO_SPIN_HPP
#endif // #ifdef WITH_SPIN
