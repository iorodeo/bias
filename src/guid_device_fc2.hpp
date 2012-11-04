#ifdef WITH_FC2
#ifndef BIAS_GUID_DEVICE_FC2_HPP
#define BIAS_GUID_DEVICE_FC2_HPP

#include <string>
#include <memory>
#include "basic_types.hpp"
#include "guid_device.hpp"
#include "C/FlyCapture2_C.h"

namespace bias {

    class GuidDevice_fc2 : public GuidDevice 
    {
        // ---------------------------------------------------------------------
        // Provides representation of Flycapture2 specific camera guids
        // ---------------------------------------------------------------------
        public:
            GuidDevice_fc2() {};
            explicit GuidDevice_fc2( fc2PGRGuid guid_fc2 );
            virtual ~GuidDevice_fc2() {};
            virtual CameraLib getCameraLib();
            virtual void printValue();
            virtual std::string toString();
            fc2PGRGuid getValue();

        private:
            fc2PGRGuid value_;
            virtual bool isEqual(GuidDevice &guid);
            virtual bool lessThan(GuidDevice &guid);
            virtual bool lessThanEqual(GuidDevice &guid);
    };

    typedef std::shared_ptr<GuidDevice_fc2> GuidDevicePtr_fc2;
}

#endif // #ifndef BIAS_GUID_DEVICE_FC2_HPP
#endif // #ifdef  WITH_FC2
