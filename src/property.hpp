#ifndef BIAS_PROPERTY_HPP
#define BIAS_PROPERTY_HPP

#include "basic_types.hpp"
#include <string>
#include <list>
#include <set>
#include <map>

namespace bias 
{
    struct Property
    { 
        PropertyType type;
        bool on;
        bool autoActive;
        unsigned int value;
        float absoluteValue;
        std::string units;
        std::string unitsAbbr;

        bool present;
        bool autoCapable;
        bool manualCapable;
        bool absoluteCapable;
        bool onePushCapable;
        bool onOffCapable;
        bool readOutCapable;
        bool haveUnits;

        unsigned int minValue;
        unsigned int maxValue;
        float minAbsoluteValue;
        float maxAbsoluteValue;

        Property();
        std::string toString();
        void print();
    };

    typedef std::list<Property> PropertyList;
    typedef std::set<Property> PropertySet;
    typedef std::map<PropertyType, Property> PropertyMap;

}

#endif // #ifndef BIAS_PROPERTY_HPP
