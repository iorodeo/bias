#ifndef NODE_MAP_SPIN_HPP
#define NODE_MAP_SPIN_HPP
#include <cstdint>
#include "SpinnakerC.h"

#include <string>
#include <vector>
#include <map>

#include "exception.hpp"
#include "camera_info_spin.hpp"
#include "base_node_spin.hpp"
#include "string_node_spin.hpp"
#include "enum_node_spin.hpp"
#include "entry_node_spin.hpp"
#include "number_node_spin.hpp"
#include "float_node_spin.hpp"
#include "integer_node_spin.hpp"

namespace bias
{

    class NodeMap_spin
    {
        public:

            NodeMap_spin(); 
            NodeMap_spin(spinCamera &hCamera);

            size_t numberOfNodes();

            template<class T> 
            T getNodeByName(std::string nodeName); // Get node of type T by name

            template<class T>
            T getNodeByIndex(size_t nodeIndex);    // Get node of type T by index

            template<class T>
            std::vector<T> nodes();                // Get vector of all nodes of type T

            std::vector<std::string> nodeNames(spinNodeType nodeType=UnknownNode);
            std::map<std::string, spinNodeType> nodeNameToTypeMap();
            std::map<std::string, std::string> nodeNameToDisplayNameMap(spinNodeType nodeType=UnknownNode); 
            std::map<std::string, std::string> nodeNameToTooTipMap(spinNodeType nodeType=UnknownNode); 
            std::map<std::string, std::string> nodeNameToDescriptionMap(spinNodeType nodeType=UnknownNode); 


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


    template<class T>
    std::vector<T> NodeMap_spin::nodes()
    {
        std::vector<T> nodeVec;

        for (size_t i=0; i<numberOfNodes(); i++) 
        { 
            spinNodeHandle hNode = nullptr;
            try
            {
                getNodeHandleByIndex(i, hNode);
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }

            BaseNode_spin node(hNode);

            if ( !node.isAvailable()  || !node.isReadable() )
            {
                continue;
            }

            if (T::ExpectedType() != UnknownNode)
            {
                bool test = false;
                try
                {
                    test = node.isOfType(T::ExpectedType());
                }
                catch (RuntimeError &runtimeError)
                {
                    continue;
                }
                if (!test)
                {
                    continue;
                }

            }

            nodeVec.push_back(node);

        } // for (size_t ...

        return nodeVec;
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
