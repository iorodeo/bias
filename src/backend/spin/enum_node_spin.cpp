#include "enum_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>

namespace bias
{
    spinNodeType EnumNode_spin::ExpectedType()
    {
        return EnumerationNode;
    }


    EnumNode_spin::EnumNode_spin() : BaseNode_spin() 
    {}


    EnumNode_spin::EnumNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
    {
        if (!isOfType(ExpectedType()))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": incorrect node type";
            throw RuntimeError(ERROR_SPIN_INCORRECT_NODE_TYPE, ssError.str());
        }
    }


    EnumNode_spin::EnumNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
    }


    size_t EnumNode_spin::numberOfEntries()
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

        size_t num = 0;
        spinError err = spinEnumerationGetNumEntries(hNode_,&num);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get number of entries for enumeration node, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_ENUM_ENTRY_NODE, ssError.str());
        }
        return num;
    }

} // namespace bias
