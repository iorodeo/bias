#include "enum_node_spin.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <sstream>

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
        checkReadableAndAvailable();

        size_t num = 0;
        spinError err = spinEnumerationGetNumEntries(hNode_,&num);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get number of entries for enumeration node, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_NUM_ENTRIES, ssError.str());
        }
        return num;
    }


    EntryNode_spin EnumNode_spin::currentEntry()
    {
        checkNodeHandle();
        checkReadableAndAvailable();

        spinNodeHandle hEntryNode;
        spinError err = spinEnumerationGetCurrentEntry(hNode_, &hEntryNode);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to get current enumeration entry node, error = " << err;
            throw RuntimeError(ERROR_SPIN_RETRIEVE_ENUM_ENTRY_NODE, ssError.str());
        }
        EntryNode_spin entryNode(hEntryNode);
        return entryNode;
    }


    std::vector<EntryNode_spin> EnumNode_spin::entries()
    {
        checkNodeHandle();
        checkReadableAndAvailable();

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
                throw RuntimeError(ERROR_SPIN_RETRIEVE_ENUM_ENTRY_NODE, ssError.str());
            }
            EntryNode_spin entryNode(hEntryNode);
            if (entryNode.isAvailable() and entryNode.isReadable())
            {
                entryNodeVec.push_back(hEntryNode);
            }
        }
        return entryNodeVec;
    }



} // namespace bias
