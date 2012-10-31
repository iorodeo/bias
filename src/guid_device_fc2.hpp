#ifdef WITH_FC2
#ifndef GUID_DEVICE_FC2_HPP
#define GUID_DEVICE_FC2_HPP

#include <string>
#include <stdint.h>
#include "C/FlyCapture2_C.h"
#include "basic_types.hpp"
#include "guid_device_base.hpp"

namespace bias {

    class GuidDevice_fc2 : public GuidDevice_base 
    {
        public:
            GuidDevice_fc2() {};
            GuidDevice_fc2( fc2PGRGuid guid );
            virtual ~GuidDevice_fc2() {};
            virtual CameraLib getCameraLib();
            virtual void printValue();
            virtual std::string toString();
            fc2PGRGuid getValue();

        private:
            fc2PGRGuid value_;
            virtual bool isEqual(GuidDevice_base &guid);

    };
}

#endif // #ifndef GUID_DEVICE_FC2_HPP
#endif // #ifdef  WITH_FC2
