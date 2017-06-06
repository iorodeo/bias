#ifdef WITH_SPIN
#ifndef BIAS_GUID_DEVICE_SPIN_HPP
#define BIAS_GUID_DEVICE_SPIN_HPP

#include <string>
#include <memory>
#include "basic_types.hpp"
#include "guid_device.hpp"
#include "SpinnakerC.h"

namespace bias {

    class GuidDevice_spin : public GuidDevice 
    {
        // ---------------------------------------------------------------------
        // Provides representation of Flycapture2 specific camera guids
        // ---------------------------------------------------------------------
        public:
            GuidDevice_spin();
            //explicit GuidDevice_spin( spinPGRGuid guid_spin );
            //virtual ~GuidDevice_spin() {};
            //virtual CameraLib getCameraLib();
            //virtual void printValue();
            //virtual std::string toString();
            //spinPGRGuid getValue();

        private:
            //spinPGRGuid value_;
            //virtual bool isEqual(GuidDevice &guid);
            //virtual bool lessThan(GuidDevice &guid);
            //virtual bool lessThanEqual(GuidDevice &guid);
    };

    typedef std::shared_ptr<GuidDevice_spin> GuidDevicePtr_spin;
}

#endif // #ifndef BIAS_GUID_DEVICE_SPIN_HPP
#endif // #ifdef  WITH_SPIN
