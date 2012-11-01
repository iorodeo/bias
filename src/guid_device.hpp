#ifndef GUID_DEVICE_HPP
#define GUID_DEVICE_HPP

#include <string>
#include <iostream>
#include "basic_types.hpp"

namespace bias {

    class GuidDevice 
    {
        friend bool operator== (GuidDevice &guid0, GuidDevice &guid1)
        {
            return guid0.isEqual(guid1);
        };
        
        friend bool operator!= (GuidDevice &guid0, GuidDevice &guid1)
        {
            return !(guid0.isEqual(guid1));
        };
        
        public:
            GuidDevice() {};
            virtual ~GuidDevice() {};
            virtual CameraLib getCameraLib() { return CAMERA_LIB_UNDEFINED; };
            virtual void printValue() { std::cout << toString() << std::endl; };
            virtual std::string toString() { return std::string("guid value not defined"); };

        private:
            virtual bool isEqual(GuidDevice &guid) 
            { 
                return false; 
            };
    }; // class GuidDevice

} // namespace bias


#endif // #ifndef GUID_DEVICE_HPP
