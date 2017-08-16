#ifndef NODE_MAP_SPIN_HPP
#define NODE_MAP_SPIN_HPP
#include <cstdint>
#include "SpinnakerC.h"

#include <string>
#include <vector>
#include <map>

#include "base_node_spin.hpp"
#include "string_node_spin.hpp"
#include "enum_node_spin.hpp"
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

            //template<class T>
            //std::vector<T> nodes();

            template<class T> 
            T getNodeByName(std::string nodeName);

            template<class T>
            T getNodeByIndex(size_t nodeIndex);


        protected:

            spinNodeMapHandle hNodeMap_ = nullptr;

            void getNodeHandleByName(std::string nodeName, spinNodeHandle &hNode);
            void getNodeHandleByIndex(size_t nodeIndex, spinNodeHandle &hNode);
    };


    template<class T> 
    T NodeMap_spin::getNodeByName(std::string nodeName)
    {
        spinNodeHandle hNode = nullptr;
        getNodeHandleByName(nodeName, hNode);
        return T(hNode); 
    }


    template<class T>
    T NodeMap_spin::getNodeByIndex(size_t nodeIndex)
    {
        spinNodeHandle hNode = nullptr;
        getNodeHandleByIndex(nodeIndex, hNode);
        return T(hNode); 
    }


    // NodeMapCamera_spin 
    // --------------------------------------------------------------------------------------------

    class NodeMapCamera_spin : public NodeMap_spin
    {
        public:

            NodeMapCamera_spin();
            NodeMapCamera_spin(spinCamera &hCamera);

    };


    // NodeMapTLDevice_spin
    // --------------------------------------------------------------------------------------------

    class NodeMapTLDevice_spin : public NodeMap_spin
    {
        public:

            NodeMapTLDevice_spin();
            NodeMapTLDevice_spin(spinCamera &hCamera);
            
            CameraInfo_spin cameraInfo();



    };


} // namespace bias

#endif
