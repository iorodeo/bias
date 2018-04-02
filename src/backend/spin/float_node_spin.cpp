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
        if (hNode_ == nullptr) 
        {
            return;
        }

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

    void FloatNode_spin::setValueFromInt(unsigned int intValue)
    {
        double floatValue = intToFloat(intValue);
        setValue(floatValue);
    }


    unsigned int FloatNode_spin::intValueWithLimits(double lowerLimit, double upperLimit)
    {
        return floatToIntWithLimits(value(), lowerLimit, upperLimit);

    }


    unsigned int FloatNode_spin::intValueWithUpperLimit(double upperLimit)
    {
        return floatToIntWithUpperLimit(value(), upperLimit);

    }


    void FloatNode_spin::setValueFromIntWithLimits(unsigned int intValue, double lowerLimit, double upperLimit)
    {
        double floatValue = intToFloatWithLimits(intValue, lowerLimit, upperLimit);
        setValue(floatValue);
    }

    void FloatNode_spin::setValueFromIntWithUpperLimit(unsigned int intValue, double upperLimit)
    {
        double floatValue = intToFloatWithUpperLimit(intValue, upperLimit);
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


    double FloatNode_spin::intToFloat(unsigned int intValue)
    {
        return intToFloatWithLimits(intValue, minValue(), maxValue());
    }


    unsigned int FloatNode_spin::floatToInt(double floatValue)
    {
        return floatToIntWithLimits(floatValue, minValue(), maxValue());
    }


    double FloatNode_spin::intToFloatWithLimits(unsigned int intValue, double lowerLimit, double upperLimit)
    {
        double floatValue = (upperLimit - lowerLimit)*double(intValue)/double(NumberOfIntValues-1) + lowerLimit;
        floatValue =  std::max(std::min(floatValue, upperLimit), lowerLimit);
        return std::max(std::min(floatValue, maxValue()), minValue());
    }


    double FloatNode_spin::intToFloatWithUpperLimit(unsigned int intValue, double upperLimit)
    {
        return intToFloatWithLimits(intValue, minValue(), upperLimit);
    }


    unsigned int FloatNode_spin::floatToIntWithLimits(double floatValue, double lowerLimit, double upperLimit)
    {
       double floatValueClamp = std::max(std::min(floatValue, upperLimit), lowerLimit);
       floatValueClamp = std::max(std::min(floatValueClamp, maxValue()), minValue());
       return (unsigned int)((floatValueClamp - lowerLimit)*double(NumberOfIntValues-1)/(upperLimit - lowerLimit));
    }


    unsigned int FloatNode_spin::floatToIntWithUpperLimit(double floatValue, double upperLimit)
    {
        return floatToIntWithLimits(floatValue, minValue(), upperLimit);

    }


} // namespace bias

