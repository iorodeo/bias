#ifndef NUMBER_NODE_SPIN_HPP
#define NUMBER_NODE_SPIN_HPP

#include "base_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <functional>

namespace bias
{

    template <class T>
    class NumberNode_spin : public BaseNode_spin
    {

        public:

            NumberNode_spin() : BaseNode_spin() {};
            NumberNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode) {};

            T value();
            void setValue(T value);

            T minValue();
            T maxValue();

            virtual void print();

        protected:

            std::function<spinError(spinNodeHandle,T*)> getValFunc_ = nullptr;
            std::function<spinError(spinNodeHandle,T )> setValFunc_ = nullptr;
            std::function<spinError(spinNodeHandle,T*)> getMinFunc_ = nullptr;
            std::function<spinError(spinNodeHandle,T*)> getMaxFunc_ = nullptr;
    };


    template <class T>
    T NumberNode_spin<T>::value()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();
        T value = 0.0;
        spinError err = getValFunc_(hNode_,&value);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get " << typeid(T).name() << " node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NUMBER_VALUE, ssError.str());
        }
        return value;
    }


    template <class T>
    void NumberNode_spin<T>::setValue(T value)
    {
        checkNodeHandle();
        checkAvailable();
        checkWritable();
        spinError err = SPINNAKER_ERR_SUCCESS;
        if  ((value >= minValue()) && value <= maxValue())
        { 
            err = setValFunc_(hNode_,value);
        }
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to set " << typeid(T).name() << " node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NUMBER_VALUE, ssError.str());
        }
    }

    template <class T>
    T NumberNode_spin<T>::minValue()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        T minValue = 0.0;
        spinError err = getMinFunc_(hNode_,&minValue);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get " << typeid(float).name() << " node min value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NUMBER_MIN_VALUE, ssError.str());
        }
        return minValue;
    }


    template <class T>
    T NumberNode_spin<T>::maxValue()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        T maxValue = 0.0;
        spinError err = getMaxFunc_(hNode_,&maxValue);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get " << typeid(float).name() << " node max value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NUMBER_MAX_VALUE, ssError.str());
        }
        return maxValue;
    }


    template <class T>
    void NumberNode_spin<T>::print()
    {
        std::cout << name() << ", number"  << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl; 
        std::cout << "type      = " << typeAsString() << std::endl;
        std::cout << "value     = " << value() << std::endl;
        std::cout << "max value = " << maxValue() << std::endl; 
        std::cout << "min value = " << minValue() << std::endl; 
        std::cout << "available = " << isReadable() << std::endl;
        std::cout << "readable  = " << isReadable() << std::endl;
        std::cout << "writable  = " << isReadable() << std::endl;
        std::cout << std::endl;
    }


} // namespace bias

#endif
