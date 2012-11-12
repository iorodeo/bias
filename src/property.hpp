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
        bool present;
        bool absoluteControl;
        bool onePush;
        bool on;
        bool autoActive;
        unsigned int value;
        float absoluteValue;

        Property();
        std::string toString();
        void print();
    };

    typedef std::list<Property> PropertyList;
    typedef std::set<Property> PropertySet;
    typedef std::map<PropertyType, Property> PropertyMap;

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

    typedef std::list<PropertyInfo> PropertyInfoList;
    typedef std::set<PropertyInfo> PropertyInfoSet;
    typedef std::map<PropertyType, PropertyInfo> PropertyInfoMap;

}

#endif // #ifndef BIAS_PROPERTY_HPP
