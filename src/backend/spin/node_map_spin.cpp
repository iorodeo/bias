#include "node_map_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>

namespace bias 
{
    const size_t MAX_BUF_LEN = 512;


    // NodeMap_spin - public methods
    // ----------------------------------------------------------------------------------


    NodeMap_spin::NodeMap_spin() 
    {} 


    NodeMap_spin::NodeMap_spin(spinCamera &hCamera) {}


    size_t NodeMap_spin::numberOfNodes()
    {
        size_t numNodes = 0;
        spinError err = SPINNAKER_ERR_SUCCESS;
        err = spinNodeMapGetNumNodes(hNodeMap_,&numNodes);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get number of nodes, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_VENDOR_NAME, ssError.str());
        }
        return numNodes;
    }


    std::vector<std::string> NodeMap_spin::nodeNames(spinNodeType nodeType)
    {
        std::vector<std::string> nodeNameVec;

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
            if (nodeType != UnknownNode)
            {
                bool test = false;
                try
                {
                    test = node.isOfType(nodeType);
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

            std::string nodeName("");
            try
            {
                nodeName = node.name();
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }

            nodeNameVec.push_back(nodeName);
        }

        return nodeNameVec;


    }


    std::map<std::string, spinNodeType> NodeMap_spin::nodeNameToTypeMap()
    {
        std::map<std::string, spinNodeType> nameToType;
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

            std::string nodeName("");
            try
            {
                nodeName = node.name();
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }

            spinNodeType nodeType;
            try
            {
                nodeType = node.type();
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            nameToType[nodeName] = nodeType;
        }
        return nameToType;
    }


    std::map<std::string, std::string> NodeMap_spin::nodeNameToDisplayNameMap(spinNodeType nodeType) 
    {
        std::map<std::string, std::string> nameToDispName;
        for (size_t i=0; i<numberOfNodes(); i++)
        {
            spinNodeHandle hNode = nullptr;
            try
            {
                getNodeHandleByIndex(i,hNode);
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            BaseNode_spin node(hNode);

            if (nodeType != UnknownNode)
            {
                bool test = false;
                try
                {
                    test = node.isOfType(nodeType);
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

            std::string nodeName("");
            try
            {
                nodeName = node.name();
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            
            std::string dispName("");
            try
            {
                dispName = node.displayName();
            } 
            catch (RuntimeError &runtimeError)
            {
                continue;
            }

            nameToDispName[nodeName] = dispName;
        }
        return nameToDispName;
    }


    std::map<std::string, std::string> NodeMap_spin::nodeNameToTooTipMap(spinNodeType nodeType) 
    {
        std::map<std::string, std::string> nameToToolTip;
        for (size_t i=0; i<numberOfNodes(); i++)
        {
            spinNodeHandle hNode = nullptr;
            try
            {
                getNodeHandleByIndex(i,hNode);
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            BaseNode_spin node(hNode);

            if (nodeType != UnknownNode)
            {
                bool test = false;
                try
                {
                    test = node.isOfType(nodeType);
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

            std::string nodeName("");
            try
            {
                nodeName = node.name();
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            
            std::string toolTip("");
            try
            {
                toolTip = node.toolTip();
            } 
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            nameToToolTip[nodeName] = toolTip;
        }
        return nameToToolTip;
    }


    std::map<std::string, std::string> NodeMap_spin::nodeNameToDescriptionMap(spinNodeType nodeType) 
    {
        std::map<std::string, std::string> nameToDescMap;
        for (size_t i=0; i<numberOfNodes(); i++)
        {
            spinNodeHandle hNode = nullptr;
            try
            {
                getNodeHandleByIndex(i,hNode);
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            BaseNode_spin node(hNode);

            if (nodeType != UnknownNode)
            {
                bool test = false;
                try
                {
                    test = node.isOfType(nodeType);
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
            std::string nodeName("");
            try
            {
                nodeName = node.name();
            }
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            
            std::string nodeDesc("");
            try
            {
                nodeDesc = node.description();
            } 
            catch (RuntimeError &runtimeError)
            {
                continue;
            }
            nameToDescMap[nodeName] = nodeDesc; 
        }
        
        return nameToDescMap;
    }


    // NodeMap_spin - protected methods
    // ----------------------------------------------------------------------------------

    void NodeMap_spin::getNodeHandleByName(std::string nodeName, spinNodeHandle &hNode)
    {
        spinError err = spinNodeMapGetNode(hNodeMap_, nodeName.c_str(), &hNode);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node handle, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_HANDLE, ssError.str());
        }

    }


    void NodeMap_spin::getNodeHandleByIndex(size_t nodeIndex, spinNodeHandle &hNode)
    {
        spinError err = spinNodeMapGetNodeByIndex(hNodeMap_, nodeIndex, &hNode);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node handle, error=" << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_HANDLE, ssError.str());
        }
    }




    // ----------------------------------------------------------------------------------
    // NodeMapCamera_spin
    // ----------------------------------------------------------------------------------
    
    NodeMapCamera_spin::NodeMapCamera_spin() {};


    NodeMapCamera_spin::NodeMapCamera_spin(spinCamera &hCamera)
    {
        // Get camera node map
        spinError err = spinCameraGetNodeMap(hCamera, &hNodeMap_);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to retrieve Spinnaker GenICam node map, error=" << err;
            throw RuntimeError(ERROR_SPIN_GET_CAMERA_NODE_MAP, ssError.str());
        }
    }


    // ----------------------------------------------------------------------------------
    // NodeMapTLDevice_spin
    // ----------------------------------------------------------------------------------
    
    NodeMapTLDevice_spin::NodeMapTLDevice_spin() {};


    NodeMapTLDevice_spin::NodeMapTLDevice_spin(spinCamera &hCamera)
    {
        // Get TLDevice node map
        spinError err = spinCameraGetTLDeviceNodeMap(hCamera, &hNodeMap_);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to retrieve Spinnaker TL device node map, error=" << err;
            throw RuntimeError(ERROR_SPIN_GET_TLDEVICE_NODE_MAP, ssError.str());
        }
    }


    CameraInfo_spin NodeMapTLDevice_spin::cameraInfo()
    { 
        std::string deviceId =  getNodeByName<StringNode_spin>("DeviceID").value();
        std::string serialNumber = getNodeByName<StringNode_spin>("DeviceSerialNumber").value();
        std::string modelName = getNodeByName<StringNode_spin>("DeviceModelName").value();
        std::string vendorName = getNodeByName<StringNode_spin>("DeviceVendorName").value();

        std::string modelId = std::string("NA");
        std::string vendorId = std::string("NA");

        CameraInfo_spin info;
        info.setGuid(deviceId);
        info.setSerialNumber(serialNumber);
        info.setModelName(modelName);
        info.setVendorName(vendorName);
        info.setModelId(modelId);
        info.setVendorId(vendorId);

        return info;
    }





} // namespace bias
