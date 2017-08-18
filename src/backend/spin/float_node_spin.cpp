#include "float_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>


namespace bias
{
    spinNodeType FloatNode_spin::ExpectedType()
    {
        return FloatNode;
    }


    FloatNode_spin::FloatNode_spin() : BaseNode_spin() 
    {}


    FloatNode_spin::FloatNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
    {
        if (!isOfType(ExpectedType()))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": incorrect node type";
            throw RuntimeError(ERROR_SPIN_INCORRECT_NODE_TYPE, ssError.str());
        }
    }


    FloatNode_spin::FloatNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
    }


    double FloatNode_spin::value()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();
        double value = 0.0;
        spinError err = spinFloatGetValue(hNode_,&value);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get float node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_FLOAT_VALUE, ssError.str());
        }
        return value;
    }


    void FloatNode_spin::setValue(double value)
    {
        checkNodeHandle();
        checkAvailable();
        checkWritable();
        spinError err = spinFloatSetValue(hNode_,value);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set float node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_FLOAT_VALUE, ssError.str());
        }
    }

    double FloatNode_spin::minValue()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        double minValue = 0.0;
        spinError err = spinFloatGetMin(hNode_,&minValue);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get float node min value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_FLOAT_MIN_VALUE, ssError.str());
        }
        return minValue;
    }


    double FloatNode_spin::maxValue()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        double maxValue = 0.0;
        spinError err = spinFloatGetMax(hNode_,&maxValue);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get float node max value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_FLOAT_MAX_VALUE, ssError.str());
        }
        return maxValue;
    }


    std::string FloatNode_spin::unit()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;
        spinError err = spinFloatGetUnit(hNode_,buffer,&bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get string node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_FLOAT_UNIT, ssError.str());
        }
        return std::string(buffer);
    }

} // namespace bias

