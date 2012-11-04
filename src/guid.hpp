#ifndef BIAS_GUID_HPP
#define BIAS_GUID_HPP

#include <string>
#include <iostream>
#include <set>
#include <list>
#include <memory>
#include <stdint.h>
#include "basic_types.hpp"
#include "guid_device.hpp"

#ifdef WITH_FC2
#include "C/FlyCapture2_C.h"
#include "guid_device_fc2.hpp"
#endif

#ifdef WITH_DC1394
#include "guid_device_dc1394.hpp"
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

            friend bool operator== (Guid &guid0, Guid &guid1);
            friend bool operator!= (Guid &guid0, Guid &guid1);
            friend bool operator<  (Guid &guid0, Guid &guid1);
            friend bool operator<= (Guid &guid0, Guid &guid1);
            friend bool operator>  (Guid &guid0, Guid &guid1);
            friend bool operator>= (Guid &guid0, Guid &guid1);

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
           
    };


    // ------------------------------------------------------------------------
    
    typedef std::shared_ptr<Guid> GuidPtr;

    class GuidPtrCmp : public std::binary_function<GuidPtr, GuidPtr, bool>
    {
        // Comparison object for shared_ptrs to Guid objects  
        public:
            bool operator() (const GuidPtr &guidPtr0, const GuidPtr &guidPtr1);
    };

    typedef std::set<GuidPtr,GuidPtrCmp> GuidPtrSet;
    typedef std::list<GuidPtr> GuidPtrList;

} // namespase bias

#endif // #ifndef BIAS_GUID_HPP
