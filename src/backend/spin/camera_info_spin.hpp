#ifdef WITH_SPIN
#ifndef BIAS_CAMERA_INFO_SPIN_HPP
#define BIAS_CAMERA_INFO_SPIN_HPP
#include <string>

namespace bias {


    class CameraInfo_spin
    {
        public:

            CameraInfo_spin();
            std::string toString();
            void print();

        protected:

            std::string guid_ = std::string("");
            std::string serialNumber_ = std::string("");
            std::string vendorId_ = std::string("");
            std::string modelId_ = std::string("");
            std::string modeName_ = std::string("");
    };

}

#endif // #ifndef BIAS_CAMERA_INFO_SPIN_HPP
#endif // #ifdef WITH_SPIN
