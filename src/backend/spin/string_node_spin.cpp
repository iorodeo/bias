#include "string_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>

namespace bias
{
    StringNode_spin::StringNode_spin() : BaseNode_spin() 
    {}

    StringNode_spin::StringNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
    {
        if (!isOfType(StringNode))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": incorrect node type";
            throw RuntimeError(ERROR_SPIN_INCORRECT_NODE_TYPE, ssError.str());
        }
    }

    std::string StringNode_spin::value()
    {
        if (!isAvailable())
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": node is not available";
            throw RuntimeError(ERROR_SPIN_NODE_NOT_AVAILABLE, ssError.str());

        }

        if (!isReadable())
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": node is not readable";
            throw RuntimeError(ERROR_SPIN_NODE_NOT_READABLE, ssError.str());

        }

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;

        spinError err = spinStringGetValue(hNode_,buffer,&bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get string node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_STRING_VALUE, ssError.str());
        }

        return std::string(buffer);
    }


} // namespace bias
