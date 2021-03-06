#include "property.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>

namespace bias
{
    // Property methods
    // ------------------------------------------------------------------------
    Property::Property()
    {
        type = PROPERTY_TYPE_UNSPECIFIED;
        present = false;
        absoluteControl = false;
        onePush = false;
        on = false;
        autoActive = false;
        value = 0;
        valueA = 0;
        valueB = 0;
        absoluteValue = 0.0;

    }


    void Property::print()
    {
        std::cout << toString();
    }

    std::string Property::toString() 
    {
        std::stringstream ss;
        ss << std::boolalpha;
        ss << std::endl;
        ss << "type:             " << getPropertyTypeString(type) << std::endl;
        ss << "present:          " << present << std::endl;
        ss << "absoluteControl:  " << absoluteControl << std::endl;
        ss << "onePush:          " << onePush << std::endl;
        ss << "on:               " << on << std::endl;
        ss << "autoActive:       " << autoActive << std::endl;
        if (type == PROPERTY_TYPE_WHITE_BALANCE)
        {
            ss << "value Red:        " << valueA << std::endl;
            ss << "value Blue:       " << valueB << std::endl;
        }
        else
        {
            ss << "value:            " << value << std::endl;
        }
        ss << "absoluteValue:    " << absoluteValue << std::endl;
        ss << std::endl;
        ss << std::noboolalpha;
        return ss.str();
    }

    // PropertyInfo methods
    //-------------------------------------------------------------------------

    PropertyInfo::PropertyInfo()
    {
        type = PROPERTY_TYPE_UNSPECIFIED;
        present = false;
        autoCapable = false;
        manualCapable = false;
        absoluteCapable = false;
        onePushCapable = false;
        onOffCapable = false;
        readOutCapable = false;
        minValue = 0;
        maxValue = 0;
        minAbsoluteValue = 0.0;
        maxAbsoluteValue = 0.0;
        haveUnits = false;
        units = std::string("");
        unitsAbbr = std::string("");
    }

    void PropertyInfo::print()
    {
        std::cout << toString();
    }

    std::string PropertyInfo::toString()
    {
        std::stringstream ss;
        ss << std::boolalpha;
        ss << std::endl;
        ss << "type:             " << getPropertyTypeString(type) << std::endl;
        ss << "present:          " << present << std::endl;
        ss << "autoCapable:      " << autoCapable << std::endl;
        ss << "manualCapable:    " << manualCapable << std::endl;
        ss << "absoluteCapable:  " << absoluteCapable << std::endl;
        ss << "onePushCapable:   " << onePushCapable << std::endl;
        ss << "onOffCapable:     " << onOffCapable << std::endl;
        ss << "readOutCapable:   " << readOutCapable << std::endl;
        ss << "minValue:         " << minValue << std::endl;
        ss << "maxValue:         " << maxValue << std::endl;
        ss << "minAbsoluteValue: " << minAbsoluteValue << std::endl;
        ss << "maxAbsoluteValue: " << maxAbsoluteValue << std::endl;
        ss << "haveUnits:        " << haveUnits << std::endl;
        ss << "units:            " << units << std::endl;
        ss << "unitsAbbr:        " << unitsAbbr << std::endl;
        ss << std::endl;
        ss << std::noboolalpha;
        return ss.str();
    }

}
