#ifndef PROPERTY_FWD_HPP
#define PROPERTY_FWD_HPP

#include "basic_types.hpp"
#include <list>
#include <set>
#include <map>

namespace bias
{
    class Property;
    class PropertyInfo;

    typedef std::list<Property> PropertyList;
    typedef std::set<Property> PropertySet;
    typedef std::map<PropertyType, Property> PropertyMap;

    typedef std::list<PropertyInfo> PropertyInfoList;
    typedef std::set<PropertyInfo> PropertyInfoSet;
    typedef std::map<PropertyType, PropertyInfo> PropertyInfoMap;
}

#endif // PROPERTY_FWD_HPP
