#include "float_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>


namespace bias
{
    unsigned int FloatNode_spin::NumberOfIntValues = 100;


    spinNodeType FloatNode_spin::ExpectedType()
    {
        return FloatNode;
    }


    FloatNode_spin::FloatNode_spin() : NumberNode_spin() 
    {
        initialize();
    }


    FloatNode_spin::FloatNode_spin(spinNodeHandle hNode) : NumberNode_spin(hNode)
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


    FloatNode_spin::FloatNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
        initialize();
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
            throw RuntimeError(ERROR_SPIN_GET_NUMBER_UNIT, ssError.str());
        }
        return std::string(buffer);
    }

    void FloatNode_spin::print()
    {
        std::cout << name() << ", float"  << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl; 
        std::cout << "value     = " << value() << std::endl;
        std::cout << "max value = " << maxValue() << std::endl; 
        std::cout << "min value = " << minValue() << std::endl; 
        std::cout << "unit      = " << unit() << std::endl; 
        std::cout << "available = " << isAvailable() << std::endl;
        std::cout << "readable  = " << isReadable() << std::endl;
        std::cout << "writable  = " << isWritable() << std::endl;
        std::cout << std::endl;
    }


    unsigned int FloatNode_spin::intValue()
    {
        return floatToInt(value());
    }


    unsigned int FloatNode_spin::minIntValue()
    {
        return 0;
    }


    unsigned int FloatNode_spin::maxIntValue()
    {
        return NumberOfIntValues-1; 
    }


    double FloatNode_spin::intToFloat(unsigned int intValue)
    {
        double floatValue = (maxValue() - minValue())*double(intValue)/double(NumberOfIntValues-1) + minValue();
        return std::max(std::min(floatValue, maxValue()), minValue());
    }


    unsigned int FloatNode_spin::floatToInt(double floatValue)
    {
       double floatValueClamp = std::max(std::min(floatValue, maxValue()), minValue());
       return (unsigned int)((floatValueClamp - minValue())*double(NumberOfIntValues-1)/(maxValue() - minValue()));
    }


    void FloatNode_spin::setValueFromInt(unsigned int intValue)
    {
        double floatValue = intToFloat(intValue);
        setValue(floatValue);
    }


    // Protected methods
    // --------------------------------------------------------------------------------------------
    void FloatNode_spin::initialize()
    {
        getValFunc_ = spinFloatGetValue;
        setValFunc_ = spinFloatSetValue;
        getMinFunc_ = spinFloatGetMin;
        getMaxFunc_ = spinFloatGetMax;
    }

} // namespace bias

