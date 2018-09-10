#include "entry_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>

namespace bias
{
    spinNodeType EntryNode_spin::ExpectedType()
    {
        return EnumEntryNode;
    }


    EntryNode_spin::EntryNode_spin() : BaseNode_spin() 
    {}


    EntryNode_spin::EntryNode_spin(spinNodeHandle hNode) : BaseNode_spin(hNode)
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
    }


    EntryNode_spin::EntryNode_spin(BaseNode_spin node)
    {
        hNode_ = node.handle();
    }


    size_t EntryNode_spin::value()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        size_t entryVal = 0;
        spinError err = spinEnumerationEntryGetEnumValue(hNode_, &entryVal);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get current enumeration entry value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_VALUE, ssError.str());
        }
        return entryVal;
    }


    int64_t EntryNode_spin::intValue()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        int64_t entryIntVal = 0;
        spinError err = spinEnumerationEntryGetIntValue(hNode_, &entryIntVal);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get current enumeration entry int value, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_INT_VALUE, ssError.str());
        }
        return entryIntVal;
    }

    std::string EntryNode_spin::symbolic()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        char buffer[MAX_BUF_LEN];
        size_t bufferLen = MAX_BUF_LEN;

        spinError err = spinEnumerationEntryGetSymbolic(hNode_,buffer,&bufferLen);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get entry node symbolic, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_SYMBOLIC, ssError.str());
        }

        return std::string(buffer);
    }


    bool EntryNode_spin::isSymbolicValueEqualTo(std::string value)
    {
        return (value == symbolic());
        
    }


    void EntryNode_spin::print()
    {
        std::cout << name() << ", base"  << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl; 
        std::cout << "symbolic  = " << symbolic() << std::endl;
        std::cout << "value     = " << value() << std::endl;
        std::cout << "intValue  = " << intValue() << std::endl; 
        std::cout << "available = " << isAvailable() << std::endl;
        std::cout << "readable  = " << isReadable() << std::endl;
        std::cout << "writable  = " << isWritable() << std::endl;
        std::cout << std::endl;
    }

    std::string EntryNode_spin::toString()
    {
        std::stringstream ss;
        ss << name() << ", base"  << std::endl;
        ss << "--------------------------------------------------------" << std::endl; 
        ss << "symbolic  = " << symbolic() << std::endl;
        ss << "value     = " << value() << std::endl;
        ss << "intValue  = " << intValue() << std::endl; 
        ss << "available = " << isAvailable() << std::endl;
        ss << "readable  = " << isReadable() << std::endl;
        ss << "writable  = " << isWritable() << std::endl;
        ss << std::endl;
        return ss.str();
    }

} // namespace bias
