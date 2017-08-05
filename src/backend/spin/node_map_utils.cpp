#ifdef WITH_SPIN
#include "node_map_utils.hpp"
#include "exception.hpp"
#include "basic_types.hpp"
#include <sstream>
#include <iostream>

namespace bias
{
    const size_t MAX_BUF_LEN = 256;
    std::map<spinNodeType, std::string> NodeTypeToStringMap = { 
        {ValueNode,       "ValueNode"},
        {BaseNode,        "BaseNode"},
        {IntegerNode,     "IntegerNode"},
        {BooleanNode,     "BooleanNode"},
        {FloatNode,       "FloatNode"},
        {CommandNode,     "CommandNode"},
        {StringNode,      "StringNode"},
        {RegisterNode,    "RegisterNode"},
        {EnumerationNode, "EnumerationNode"},
        {EnumEntryNode,   "EnumEntryNode"},
        {CategoryNode,    "CategoryNode"},
        {PortNode,        "PortNode"},
        {UnknownNode,     "UnnknownNode"}, 
    };

    size_t getNumberOfNodes(spinNodeMapHandle &hNodeMap)
    {
        size_t numNodes = 0;
        spinError err = SPINNAKER_ERR_SUCCESS;
        err = spinNodeMapGetNumNodes(hNodeMap,&numNodes);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get vendor name readability, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
        }
        return numNodes;
    }

    std::vector<std::string> getNodeNames(spinNodeMapHandle &hNodeMap)
    {
        std::vector<std::string> nodeNameVec;
        spinError err = SPINNAKER_ERR_SUCCESS;

        size_t numberOfNodes = getNumberOfNodes(hNodeMap);

        for (size_t i=0; i<numberOfNodes; i++) 
        { 
            // Try to get the node handle
            spinNodeHandle hNode = nullptr;
            err = spinNodeMapGetNodeByIndex(hNodeMap,i,&hNode);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get this node handle - skip it. 
                continue;
            }

            // Try to get node name
            char nodeNameBuf[MAX_BUF_LEN];
            size_t lenNodeName = MAX_BUF_LEN;
            err = spinNodeGetName(hNode, nodeNameBuf, &lenNodeName);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node name - skip it. 
                continue;
            }
            std::string nodeName = std::string(nodeNameBuf);
            nodeNameVec.push_back(nodeName);
        }

        return nodeNameVec;
    }

    std::map<std::string, std::string> getNodeNameToStringValueMap(spinNodeMapHandle &hNodeMap)
    {
        //
        // Note: gets name to string value for nodes of string type only.
        //
        
        std::map<std::string, std::string> nodeNameToValueMap;
        std::vector<std::string> nodeNameVec = getNodeNames(hNodeMap);

        for (auto nodeName : nodeNameVec)
        {
            spinError err = SPINNAKER_ERR_SUCCESS;
            spinNodeHandle hNode = nullptr;

            err = spinNodeMapGetNode(hNodeMap,nodeName.c_str(),&hNode);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get this node handle - skip it. 
                continue;
            }

            bool8_t isAvailable = False;
            err = spinNodeIsAvailable(hNode, &isAvailable);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node availability - skip it. 
                continue;
            }

            bool8_t isReadable = False;
            err = spinNodeIsReadable(hNode, &isReadable);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node readability - skip it. 
                continue;
            }

            spinNodeType nodeType;
            err = spinNodeGetType(hNode, &nodeType);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node type - skip it.
                continue;
            }

            if (nodeType != StringNode)
            {
                continue;
            }

            char nodeValueBuf[MAX_BUF_LEN];
            size_t lenValueBuf = MAX_BUF_LEN;
            if (isAvailable && isReadable)
            {
                err = spinStringGetValue(hNode, nodeValueBuf, &lenValueBuf);
                if (err != SPINNAKER_ERR_SUCCESS)
                {
                    // Can't get value - skip it. 
                    continue;
                }
            }
            std::string nodeValue = std::string(nodeValueBuf);
            nodeNameToValueMap[nodeName] = nodeValue;
        }

        return nodeNameToValueMap;
    }


    std::map<std::string, spinNodeType> getNodeNameToTypeMap(spinNodeMapHandle &hNodeMap)
    {
        std::map<std::string, spinNodeType> nameToTypeMap;
        std::vector<std::string> nodeNameVec = getNodeNames(hNodeMap);

        for (auto nodeName : nodeNameVec)
        {
            spinError err = SPINNAKER_ERR_SUCCESS;
            spinNodeHandle hNode = nullptr;

            err = spinNodeMapGetNode(hNodeMap,nodeName.c_str(),&hNode);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get this node handle - skip it. 
                continue;
            }

            spinNodeType nodeType;
            err = spinNodeGetType(hNode, &nodeType);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                // Can't get node type - skip it.
                continue;
            }

            nameToTypeMap[nodeName] = nodeType;

            std::cout << nodeName << " " << getNodeTypeString(nodeType) << std::endl;

        }

        return nameToTypeMap;
    }


    std::string getNodeTypeString(spinNodeType nodeType)
    {
        std::string typeString;
        if (NodeTypeToStringMap.count(nodeType) != 0)
        {
            typeString = NodeTypeToStringMap[nodeType];
        }
        else
        {
            typeString = std::string("NodeFound");
        }
        return typeString;
    }


} // namespace bias
#endif
