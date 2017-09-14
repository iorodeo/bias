#include "string_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>

namespace bias
{

    spinNodeType StringNode_spin::ExpectedType()
    {
        return StringNode;
    }


    StringNode_spin::StringNode_spin() : BaseNode_spin() 
    {}


    StringNode_spin::StringNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
    {
        if (!isOfType(ExpectedType()))
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": incorrect node type";
            throw RuntimeError(ERROR_SPIN_INCORRECT_NODE_TYPE, ssError.str());
        }
    }


    StringNode_spin::StringNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
    }


    std::string StringNode_spin::value()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;

        spinError err = spinStringGetValue(hNode_,buffer,&bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get string node value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_STRING_VALUE, ssError.str());
        }

        return std::string(buffer);
    }


    void StringNode_spin::print()
    {
        std::cout << name() << ", string"  << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl; 
        std::cout << "value     = " << value() << std::endl;
        std::cout << "available = " << isAvailable() << std::endl;
        std::cout << "readable  = " << isReadable() << std::endl;
        std::cout << "writable  = " << isWritable() << std::endl;
        std::cout << std::endl;
    }


} // namespace bias
