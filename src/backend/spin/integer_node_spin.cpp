#include "integer_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>


namespace bias
{
    spinNodeType IntegerNode_spin::ExpectedType()
    {
        return IntegerNode;
    }


    IntegerNode_spin::IntegerNode_spin() : NumberNode_spin() 
    {
        initialize();
    }


    IntegerNode_spin::IntegerNode_spin(spinNodeHandle hNode) : NumberNode_spin(hNode)
    {
        if (!isOfType(ExpectedType()))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": incorrect node type";
            throw RuntimeError(ERROR_SPIN_INCORRECT_NODE_TYPE, ssError.str());
        }
        initialize();
    }


    IntegerNode_spin::IntegerNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
        initialize();
    }

    int64_t IntegerNode_spin::getIncrement()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();
        int64_t increment = 0;
        spinError err = spinIntegerGetIncr(hNode_,&increment);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get " << typeid(int64_t).name() << " node increment, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NUMBER_UNIT, ssError.str());
        }
        return increment;
    }

    // Protected methods
    // --------------------------------------------------------------------------------------------

    void IntegerNode_spin::initialize()
    {
        getValFunc_ = spinIntegerGetValue;
        setValFunc_ = spinIntegerSetValue;
        getMinFunc_ = spinIntegerGetMin;
        getMaxFunc_ = spinIntegerGetMax;
    }


} // namespace bias

