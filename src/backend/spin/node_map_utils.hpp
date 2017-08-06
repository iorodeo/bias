#ifdef WITH_SPIN
#ifndef NODE_MAP_UTILS_HPP
#define NODE_MAP_UTILS_HPP
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "SpinnakerC.h"

namespace bias
{

    size_t getNumberOfNodes(spinNodeMapHandle &hNodeMap);

    std::string getNodeTypeString(spinNodeType nodeType);

    std::vector<std::string> getNodeNames(spinNodeMapHandle &hNodeMap);

    std::map<std::string, spinNodeType> getNodeNameToTypeMap(spinNodeMapHandle &hNodeMap);

    std::map<std::string, std::string> getStringNodeNameToValueMap(spinNodeMapHandle &hNodeMap);

    std::vector<std::string> getEnumerationNodeEntryNames(spinNodeMapHandle &hNodeMap, std::string nodeName);


} // namespace bias


#endif
#endif
