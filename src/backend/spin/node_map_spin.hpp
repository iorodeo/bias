#ifndef NODE_MAP_SPIN_HPP
#define NODE_MAP_SPIN_HPP
#include <cstdint>
#include "SpinnakerC.h"

#include <string>
#include <vector>
#include <map>

namespace bias
{

    class NodeMap_spin
    {
        public:

            NodeMap_spin(spinCamera hCamera=nullptr);

            size_t numberOfNodes();

            std::vector<std::string> nodeNames();
            std::vector<std::string> nodeNames(spinNodeType nodeType);
            std::map<std::string, spinNodeType> nodeNameToTypeMap();

            //std::map<std::string, std::string> nodeNameToDisplayNameMap(); 
            //std::map<std::string, std::string> nodeNameToTooTipMap(); 
            //std::map<std::string, std::string> nodeNameToDescriptionMap(); 

            std::string getStringNodeValueByName(std::string nodeName);
            std::string getStringNodeValueByIndex(size_t index);


        protected:

            spinNodeMapHandle hNodeMap_ = nullptr;

            void getNodeHandleByName(std::string nodeName, spinNodeHandle &hNode);
            void getNodeHandleByIndex(size_t index, spinNodeHandle &hNode);

            spinNodeType getNodeType(spinNodeHandle &hNode);
            std::string getNodeName(spinNodeHandle &hNode);
            std::string getNodeDisplayName(spinNodeHandle &hNode);

            std::string getNodeToolTip(spinNodeHandle &hNode);
            std::string getNodeDescription(spinNodeHandle &hNode);

            bool getNodeAvailability(spinNodeHandle &hNode);
            bool getNodeReadability(spinNodeHandle &hNode);

            std::string getStringNodeValue(spinNodeHandle &hNode);


    };


    class NodeMapCamera_spin : public NodeMap_spin
    {
        public:

            NodeMapCamera_spin(spinCamera hCamera=nullptr);

    };


    class NodeMapTLDevice_spin : public NodeMap_spin
    {
        public:

            NodeMapTLDevice_spin(spinCamera hCamera=nullptr);

    };


} // namespace bias

#endif
