#include "base_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>
#include <map>

namespace bias
{

    const size_t BaseNode_spin::MAX_BUF_LEN = 512;

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


    spinNodeType BaseNode_spin::ExpectedType()
    {
        return UnknownNode;
    }


    BaseNode_spin::BaseNode_spin()
    {
        hNode_ = nullptr;
    }


    BaseNode_spin::BaseNode_spin(spinNodeHandle hNode) : hNode_(hNode)
    {};


    bool BaseNode_spin::isAvailable()
    {
        checkNodeHandle();
        bool8_t isAvailable = False;
        spinError err = spinNodeIsAvailable(hNode_, &isAvailable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node availability, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_AVAILABLE, ssError.str());
        }
        return (isAvailable == False) ? false : true;
    }


    bool BaseNode_spin::isReadable()
    {
        checkNodeHandle();
        bool8_t isReadable = False;
        spinError err = spinNodeIsReadable(hNode_, &isReadable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node readability, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_READABLE, ssError.str());
        }
        return (isReadable == False) ? false : true;
    }


    bool BaseNode_spin::isWritable()
    {
        checkNodeHandle();
        bool8_t isWritable = False;
        spinError err = spinNodeIsWritable(hNode_, &isWritable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node readability, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_WRITABLE, ssError.str());
        }
        return (isWritable == False) ? false : true;

    }


    spinNodeType BaseNode_spin::type()
    {
        checkNodeHandle();
        spinNodeType nodeType;
        spinError err = spinNodeGetType(hNode_, &nodeType);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node type,  error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_TYPE, ssError.str());

        }
        return nodeType;
    }


    bool BaseNode_spin::isOfType(spinNodeType nodeType)
    {
        return nodeType == type();
    }



    std::string BaseNode_spin::typeAsString()
    {
        spinNodeType nodeType = type();

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


    std::string BaseNode_spin::name()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetName(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node name, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_NAME, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::displayName()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetDisplayName(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node display name, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_DISPLAY_NAME, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::toolTip()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetToolTip(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node tooltip, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_TOOLTIP, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::description()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetDescription(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node description, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NODE_DESCRIPTION, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::valueAsString()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeToString(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node value as string, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_VALUE_AS_STRING, ssError.str());
        }
        return std::string(buffer);
    }


    spinNodeHandle BaseNode_spin::handle()
    {
        return hNode_;
    }


    // Protected methods
    // --------------------------------------------------------------------------------------------
    
    void BaseNode_spin::checkNodeHandle()
    {
        if (hNode_ == nullptr)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": node handle is null"; 
            throw RuntimeError(ERROR_SPIN_NULL_HANDLE, ssError.str());
        }
    }


    void BaseNode_spin::checkAvailable()
    {
        if (!isAvailable())
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": node is not available";
            throw RuntimeError(ERROR_SPIN_NODE_NOT_AVAILABLE, ssError.str());

        }
    }


    void BaseNode_spin::checkReadable()
    {
        if (!isReadable())
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": node is not readable";
            throw RuntimeError(ERROR_SPIN_NODE_NOT_READABLE, ssError.str());

        } 
    }

    void BaseNode_spin::checkWritable()
    {
        if (!isWritable())
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": node is not writable";
            throw RuntimeError(ERROR_SPIN_NODE_NOT_WRITABLE, ssError.str());

        } 
    }


} // namespace bias
