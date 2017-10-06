#include "enum_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>
#include <iostream>

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
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        size_t num = 0;
        spinError err = spinEnumerationGetNumEntries(hNode_,&num);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get number of entries for enumeration node, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_NUM_ENTRIES, ssError.str());
        }
        return num;
    }


    void EnumNode_spin::setEntryByInt(int64_t intValue)
    {
        checkNodeHandle();
        checkAvailable();
        checkWritable();

        EntryNode_spin currEntryNode = currentEntry();

        if (currEntryNode.intValue() != intValue)
        {
            spinError err = spinEnumerationSetIntValue(hNode_,intValue);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set enum node integer value, error = " << err;
                throw RuntimeError(ERROR_SPIN_SET_ENUM_INT_VALUE, ssError.str());
            }
        }
    }


    void EnumNode_spin::setEntryByValue(size_t value)
    {
        checkNodeHandle();
        checkAvailable();
        checkWritable();

        EntryNode_spin currEntryNode = currentEntry();

        if (currEntryNode.value() != value)
        {
            spinError err = spinEnumerationSetEnumValue(hNode_,value);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to enum node integer value, error = " << err;
                throw RuntimeError(ERROR_SPIN_SET_ENUM_VALUE, ssError.str());
            }
        }
    }


    void EnumNode_spin::setEntryByNode(EntryNode_spin entryNode)
    {
        setEntryByValue(entryNode.value());
    }


    void EnumNode_spin::setEntryByName(std::string nodeName)
    {
        EntryNode_spin node = getEntryByName(nodeName);
        setEntryByValue(node.value());
    }


    void EnumNode_spin::setEntryBySymbolic(std::string symbolic)
    {
        EntryNode_spin node = getEntryBySymbolic(symbolic);
        setEntryByValue(node.value());
    }


    EntryNode_spin EnumNode_spin::currentEntry()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        spinNodeHandle hEntryNode;
        spinError err = spinEnumerationGetCurrentEntry(hNode_, &hEntryNode);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get current enumeration entry node, error = " << err;
            throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_NODE, ssError.str());
        }
        EntryNode_spin entryNode(hEntryNode);
        return entryNode;
    }


    std::vector<EntryNode_spin> EnumNode_spin::entries()
    {
        checkNodeHandle();
        checkAvailable();
        checkReadable();

        std::vector<EntryNode_spin> entryNodeVec;

        for (size_t i=0; i<numberOfEntries(); i++)
        {
            spinNodeHandle hEntryNode;
            spinError err = spinEnumerationGetEntryByIndex(hNode_, i, &hEntryNode);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to get enumeration entry node, error = " << err;
                throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_NODE, ssError.str());
            }
            EntryNode_spin entryNode(hEntryNode);
            if (entryNode.isAvailable() and entryNode.isReadable())
            {
                entryNodeVec.push_back(hEntryNode);
            }
        }
        return entryNodeVec;
    }


    EntryNode_spin EnumNode_spin::getEntryByName(std::string name)
    {
        std::vector<EntryNode_spin> nodeVec = entries();

        EntryNode_spin nodeWithName;
        bool found = false;

        for (auto node : nodeVec)
        {
            if (node.name() == name)
            {
                found = true;
                nodeWithName = node;
                break;
            }
        }
        if (!found)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get enumeration entry node by name";
            throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_BY_NAME, ssError.str());
        }
        return nodeWithName;
    }

    EntryNode_spin EnumNode_spin::getEntryBySymbolic(std::string symbolic)
    {
        std::vector<EntryNode_spin> nodeVec = entries();
        EntryNode_spin nodeWithSymb;
        bool found = false;

        for (auto node : nodeVec)
        {
            if (node.symbolic() == symbolic)
            {
                found = true;
                nodeWithSymb = node;
                break;
            }
        }
        if (!found)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get enumeration entry node by symbolic";
            throw RuntimeError(ERROR_SPIN_GET_ENUM_ENTRY_BY_SYMBOLIC, ssError.str());
        }
        return nodeWithSymb;
    }


    void EnumNode_spin::print()
    {
        std::vector<EntryNode_spin> entryVec = entries();
        std::cout << name() <<  ", enum,  # entries = " << numberOfEntries() << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl; 
        std::cout << "current   = " << currentEntry().symbolic() << std::endl;
        std::cout << "available = " << isAvailable() << std::endl;
        std::cout << "readable  = " << isReadable() << std::endl;
        std::cout << "writable  = " << isWritable() << std::endl;
        std::cout << std::endl;
        std::cout << "  entries" << std::endl;
        std::cout << "  -------" << std::endl;
        for (auto entry : entryVec)
        {
            std::cout << "  " << entry.symbolic() << std::endl;
        }
        std::cout << std::endl;
    }


    bool EnumNode_spin::hasEntryName(std::string name)
    {
        bool rval = false;
        std::vector<EntryNode_spin> entryVec = entries();
        for (auto entryNode : entryVec)
        {
            if (entryNode.name() == name)
            {
                rval = true;
            }
        }
        return rval;

    }


    bool EnumNode_spin::hasEntrySymbolic(std::string symbolic) 
    {
        bool rval = false;
        std::vector<EntryNode_spin> entryVec = entries();
        for (auto entryNode : entryVec)
        {
            if (entryNode.symbolic() == symbolic)
            {
                rval = true;
            }
        }
        return rval;
    }


} // namespace bias
