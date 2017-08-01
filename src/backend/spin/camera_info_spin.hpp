#ifdef WITH_SPIN
#ifndef BIAS_CAMERA_INFO_SPIN_HPP
#define BIAS_CAMERA_INFO_SPIN_HPP

#include <string>
#include "SpinnakerC.h"
#include "basic_types.hpp"

namespace bias {


    class CameraInfo_spin
    {
        public:

            CameraInfo_spin();
            CameraInfo_spin(spinNodeMapHandle &hNodeMapTLDevice);

            void retrieve(spinNodeMapHandle &hNodeMapTLDevice);
            bool haveInfo();

            std::string toString();
            void print();

            std::string guid();
            std::string serialNumber();
            std::string vendorId();
            std::string modelId();
            std::string modelName();
            std::string vendorName();


        protected:

            bool haveInfo_ = false;
            std::string guid_ = std::string("");
            std::string serialNumber_ = std::string("");
            std::string vendorId_ = std::string("");
            std::string modelId_ = std::string("");
            std::string modelName_ = std::string("");
            std::string vendorName_ = std::string("");

            void getVendorName(spinNodeMapHandle &hNodeMapTLDevice);
    };

}

#endif // #ifndef BIAS_CAMERA_INFO_SPIN_HPP
#endif // #ifdef WITH_SPIN
