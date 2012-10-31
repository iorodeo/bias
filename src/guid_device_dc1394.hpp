#ifdef WITH_DC1394
#ifndef GUID_DEVICE_DC1394_HPP
#define GUID_DEVICE_DC1394_HPP

#include <string>
#include <iostream>
#include "basic_types.hpp"
#include "guid_device_base.hpp"

namespace bias {

    class GuidDevice_dc1394 : public GuidDevice_base
    {

        public:
            GuidDevice_dc1394() {};
            GuidDevice_dc1394( uint64_t guid );
            virtual ~GuidDevice_dc1394() {};
            virtual CameraLib getCameraLib();
            virtual void printValue();
            virtual string toString();
            uint64_t getValue();

        private:
            uint64_t value_;
            virtual bool isEqual( GuidDevice_base &guid );

    };
}

#endif // #ifndef GUID_DEVICE_DC1394
#endif // #ifdef  WITH_DC1394
