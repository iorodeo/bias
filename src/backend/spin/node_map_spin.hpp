#ifndef NODE_MAP_SPIN_HPP
#define NODE_MAP_SPIN_HPP
#include <cstdint>
#include "SpinnakerC.h"

#include <string>
#include <vector>
#include <map>

#include "base_node_spin.hpp"
#include "string_node_spin.hpp"
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

            // ---------------------------------------------------------------------------------------------------
            // Move to node classes

            // String nodes
            StringNode_spin getStringNodeByName(std::string nodeName);
            StringNode_spin getStringNodeByIndex(size_t nodeIndex);


            // Enumeration nodes
            size_t getEnumNodeNumberOfEntriesByName(std::string nodeName);
            std::vector<std::string> getEnumNodeEntryListByName(std::string nodeName);
            std::vector<std::string> getEnumNodeEntryListByIndex(size_t nodeIndex);

            // ---------------------------------------------------------------------------------------------------


        protected:

            spinNodeMapHandle hNodeMap_ = nullptr;

            void getNodeHandleByName(std::string nodeName, spinNodeHandle &hNode);
            void getNodeHandleByIndex(size_t nodeIndex, spinNodeHandle &hNode);

            // ----------------------------------------------------------------------------------------------------
            // Move to node classes
            spinNodeType getNodeType(spinNodeHandle &hNode);   // Done
            std::string getNodeName(spinNodeHandle &hNode);    // Done
            std::string getNodeDisplayName(spinNodeHandle &hNode); // Done

            std::string getNodeToolTip(spinNodeHandle &hNode); // Done
            std::string getNodeDescription(spinNodeHandle &hNode); // Done

            bool getNodeAvailability(spinNodeHandle &hNode); // Done
            bool getNodeReadability(spinNodeHandle &hNode);  // Done
            bool isNodeOfType(spinNodeType nodeType, spinNodeHandle &hNode); // Done

            std::string getStringNodeValue(spinNodeHandle &hNode); // Done
            // -----------------------------------------------------------------------------------------------------


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
