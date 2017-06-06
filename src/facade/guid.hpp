#ifndef BIAS_GUID_HPP
#define BIAS_GUID_HPP

#include "guid_fwd.hpp"
#include "basic_types.hpp"
#include <stdint.h>
#include <string>
#include <functional>
#include <iostream>
#include "guid_device.hpp"

#ifdef WITH_FC2
#include "FlyCapture2_C.h"
#include "guid_device_fc2.hpp"
#endif

#ifdef WITH_DC1394
#include "guid_device_dc1394.hpp"
#endif

#ifdef WITH_SPIN
#include "guid_device_spin.hpp"
#endif


namespace bias {
    
    class Guid 
    {
        // --------------------------------------------------------------------
        // Guid -  a Facade wrapper for camera guids. Its purpose is to create
        // a unified interface for the camera guids which are used by different 
        // underlying libraries  e.g. libdc1394, flycapture2, etc.
        // --------------------------------------------------------------------

        public:
            Guid();
            ~Guid();
            CameraLib getCameraLib();
            void printValue();
            std::string toString();

            friend bool operator== (const Guid &guid0, const Guid &guid1);
            friend bool operator!= (const Guid &guid0, const Guid &guid1);
            friend bool operator<  (const Guid &guid0, const Guid &guid1);
            friend bool operator<= (const Guid &guid0, const Guid &guid1);
            friend bool operator>  (const Guid &guid0, const Guid &guid1);
            friend bool operator>= (const Guid &guid0, const Guid &guid1);
            friend std::ostream& operator<< (std::ostream &out, Guid &guid);

        private:
            GuidDevicePtr guidDevicePtr_;

#ifdef WITH_FC2
        // FlyCapture2 specific features
        public:
            explicit Guid(fc2PGRGuid guid);
            fc2PGRGuid getValue_fc2();
#endif
#ifdef WITH_DC1394
        // Libdc1394 specific features
        public:
            explicit Guid(uint64_t guid);
            uint64_t getValue_dc1394();
#endif
#ifdef WITH_SPIN
        //public:
            // TO DO
#endif
           
    };

    class GuidCmp : public std::binary_function<Guid, Guid, bool>
    {
        // Comparision object for Guid objects
        public:
            bool operator() (const Guid &guid0, const Guid &guid1);
    };

    class GuidPtrCmp : public std::binary_function<GuidPtr, GuidPtr, bool>
    {
        // Comparison object for shared_ptrs to Guid objects  
        public:
            bool operator() (const GuidPtr &guidPtr0, const GuidPtr &guidPtr1);
    };

} // namespase bias

#endif // #ifndef BIAS_GUID_HPP
