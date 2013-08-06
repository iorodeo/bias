#ifndef BIAS_PROPERTY_HPP
#define BIAS_PROPERTY_HPP

#include "property_fwd.hpp"
#include <string>

namespace bias 
{
    struct Property
    { 
        PropertyType type;
        bool present;
        bool absoluteControl;
        bool onePush;
        bool on;
        bool autoActive;
        unsigned int value;
        unsigned int valueA;
        unsigned int valueB;
        float absoluteValue;

        Property();
        std::string toString();
        void print();
    };

    struct PropertyInfo 
    {

        PropertyType type;
        bool present;
        bool autoCapable;
        bool manualCapable;
        bool absoluteCapable;
        bool onePushCapable;
        bool onOffCapable;
        bool readOutCapable;

        unsigned int minValue;
        unsigned int maxValue;
        float minAbsoluteValue;
        float maxAbsoluteValue;

        bool haveUnits;
        std::string units;
        std::string unitsAbbr;

        PropertyInfo();
        std::string toString();
        void print();

    };

}

#endif // #ifndef BIAS_PROPERTY_HPP
