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
            CameraInfo_spin(spinNodeMapHandle &hNodeMapTLDevice);

            void retrieve(spinNodeMapHandle &hNodeMapTLDevice);
            bool haveInfo();

            std::string toString();
            void print();
            void printNameToValueMap();

            std::string guid();
            std::string serialNumber();
            std::string vendorId();
            std::string modelId();
            std::string modelName();
            std::string vendorName();


        protected:

            bool haveInfo_ = false;
            std::map<std::string, std::string> nodeNameToValueMap_;

            size_t retrieveNumberOfNodes(spinNodeMapHandle &hNodeMapTLDevice);
            std::map<std::string, std::string> retrieveNodeNameToValueMap(spinNodeMapHandle &hNodeMapTLDevice);

    };

}

#endif // #ifndef BIAS_CAMERA_INFO_SPIN_HPP
#endif // #ifdef WITH_SPIN
