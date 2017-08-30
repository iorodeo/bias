#include "bool_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>

namespace bias
{
    spinNodeType BoolNode_spin::ExpectedType()
    {
        return BooleanNode;
    }


    BoolNode_spin::BoolNode_spin() : BaseNode_spin() 
    {}


    BoolNode_spin::BoolNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
    {
        if (!isOfType(ExpectedType()))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": incorrect node type";
            throw RuntimeError(ERROR_SPIN_INCORRECT_NODE_TYPE, ssError.str());
        }
    }


    BoolNode_spin::BoolNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
    }


    void BoolNode_spin::setValue(bool value)
    {
        checkNodeHandle();
        checkAvailable();
        checkWritable();

        spinError err = spinBooleanSetValue(hNode_,bool8_t(value));
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set boolean node  value, error = " << err;
            throw RuntimeError(ERROR_SPIN_SET_BOOLEAN_VALUE, ssError.str());
        }
    }


    bool BoolNode_spin::getValue()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        bool8_t value = False;

        spinError err = spinBooleanGetValue(hNode_,&value);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get boolean node  value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_BOOLEAN_VALUE, ssError.str());
        }
        return bool(value);
    }


} // namespace bias
