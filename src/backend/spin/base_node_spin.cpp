#include "base_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>

namespace bias
{



    const size_t BaseNode_spin::MAX_BUF_LEN = 512;

    BaseNode_spin::BaseNode_spin(spinNodeHandle &hNode) : hNode_(hNode)
    {};


    bool BaseNode_spin::isAvailable()
    {
        bool8_t isAvailable = False;
        spinError err = spinNodeIsAvailable(hNode_, &isAvailable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node availability, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_AVAILABLE, ssError.str());
        }
        return (isAvailable == False) ? false : true;
    }


    bool BaseNode_spin::isReadable()
    {
        bool8_t isReadable = False;
        spinError err = spinNodeIsReadable(hNode_, &isReadable);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node readability, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_READABLE, ssError.str());
        }
        return (isReadable == False) ? false : true;
    }


    spinNodeType BaseNode_spin::type()
    {
        spinNodeType nodeType;
        spinError err = spinNodeGetType(hNode_, &nodeType);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node type,  error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_TYPE, ssError.str());

        }
        return nodeType;
    }


    bool BaseNode_spin::isOfType(spinNodeType nodeType)
    {
        return nodeType == type();
    }



    std::string BaseNode_spin::name()
    {
        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetName(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node name, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_NAME, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::displayName()
    {
        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetDisplayName(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node display name, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_DISPLAY_NAME, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::toolTip()
    {
        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetToolTip(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node tooltip, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_TOOLTIP, ssError.str());
        }
        return std::string(buffer);
    }


    std::string BaseNode_spin::description()
    {
        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinNodeGetDescription(hNode_, buffer, &bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get node description, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NODE_DESCRIPTION, ssError.str());
        }
        return std::string(buffer);
    }


} // namespace bias
