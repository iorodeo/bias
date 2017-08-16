#include "enum_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>

namespace bias
{

    EnumNode_spin::EnumNode_spin() : BaseNode_spin() 
    {}


    EnumNode_spin::EnumNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
    {
        if (!isOfType(EnumerationNode))
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

} // namespace bias
