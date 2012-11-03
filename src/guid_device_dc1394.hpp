#ifdef WITH_DC1394
#ifndef GUID_DEVICE_DC1394_HPP
#define GUID_DEVICE_DC1394_HPP

#include <string>
#include <memory>
#include "basic_types.hpp"
#include "guid_device.hpp"

namespace bias {

    class GuidDevice_dc1394 : public GuidDevice
    {
        // --------------------------------------------------------------------
        // Provides represetation of libdc1394 specific camera guids
        // --------------------------------------------------------------------

        public:
            GuidDevice_dc1394() {};
            GuidDevice_dc1394( uint64_t guid_dc1394 );
            virtual ~GuidDevice_dc1394() {};
            virtual CameraLib getCameraLib();
            virtual void printValue();
            virtual std::string toString();
            uint64_t getValue();

        private:
            uint64_t value_;
            virtual bool isEqual(GuidDevice &guid);
            virtual bool lessThan(GuidDevice &guid);
            virtual bool lessThanEqual(GuidDevice &guid);
    };

    typedef std::shared_ptr<GuidDevice_dc1394> GuidDevicePtr_dc1394;
}

#endif // #ifndef GUID_DEVICE_DC1394
#endif // #ifdef  WITH_DC1394
