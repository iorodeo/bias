#ifndef NODE_MAP_SPIN_HPP
#define NODE_MAP_SPIN_HPP
#include <cstdint>
#include "SpinnakerC.h"

#include <string>
#include <vector>
#include <map>

#include "camera_info_spin.hpp"

namespace bias
{

    class NodeMap_spin
    {
        public:

            NodeMap_spin(); 
            NodeMap_spin(spinCamera &hCamera);

            size_t numberOfNodes();

            // Map and vector queries
            std::vector<std::string> nodeNames(spinNodeType nodeType=UnknownNode);
            std::map<std::string, spinNodeType> nodeNameToTypeMap();
            std::map<std::string, std::string> nodeNameToDisplayNameMap(spinNodeType nodeType=UnknownNode); 
            std::map<std::string, std::string> nodeNameToTooTipMap(spinNodeType nodeType=UnknownNode); 
            std::map<std::string, std::string> nodeNameToDescriptionMap(spinNodeType nodeType=UnknownNode); 

            // String nodes
            std::string getStringNodeValueByName(std::string nodeName);
            std::string getStringNodeValueByIndex(size_t nodeIndex);

            // Enumeration nodes
            size_t getEnumNodeNumberOfEntriesByName(std::string nodeName);

            std::vector<std::string> getEnumNodeEntryListByName(std::string nodeName);
            std::vector<std::string> getEnumNodeEntryListByIndex(size_t nodeIndex);


        protected:

            spinNodeMapHandle hNodeMap_ = nullptr;

            void getNodeHandleByName(std::string nodeName, spinNodeHandle &hNode);
            void getNodeHandleByIndex(size_t nodeIndex, spinNodeHandle &hNode);

            spinNodeType getNodeType(spinNodeHandle &hNode);
            std::string getNodeName(spinNodeHandle &hNode);
            std::string getNodeDisplayName(spinNodeHandle &hNode);

            std::string getNodeToolTip(spinNodeHandle &hNode);
            std::string getNodeDescription(spinNodeHandle &hNode);

            bool getNodeAvailability(spinNodeHandle &hNode);
            bool getNodeReadability(spinNodeHandle &hNode);
            bool isNodeOfType(spinNodeType nodeType, spinNodeHandle &hNode);

            std::string getStringNodeValue(spinNodeHandle &hNode);


    };


    class NodeMapCamera_spin : public NodeMap_spin
    {
        public:

            NodeMapCamera_spin();
            NodeMapCamera_spin(spinCamera &hCamera);

    };


    class NodeMapTLDevice_spin : public NodeMap_spin
    {
        public:

            NodeMapTLDevice_spin();
            NodeMapTLDevice_spin(spinCamera &hCamera);
            
            CameraInfo_spin cameraInfo();



    };


} // namespace bias

#endif
