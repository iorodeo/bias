#ifndef ENUM_NODE_SPIN_HPP
#define ENUM_NODE_SPIN_HPP
#include "base_node_spin.hpp"
#include "entry_node_spin.hpp"
#include <vector>
#include <map>

namespace bias
{

    class EnumNode_spin : public BaseNode_spin
    {
        public:

            static spinNodeType ExpectedType();

            EnumNode_spin();
            EnumNode_spin(spinNodeHandle hNode);
            EnumNode_spin(BaseNode_spin node);

            size_t numberOfEntries();
            EntryNode_spin currentEntry();

            void setEntryByInt(int64_t intValue);
            void setEntryByValue(size_t value);
            void setEntryByNode(EntryNode_spin entryNode);
            void setEntryByName(std::string nodeName);
            void setEntryBySymbolic(std::string symbolic);

            std::vector<EntryNode_spin> entries();
            EntryNode_spin getEntryByName(std::string name);
            EntryNode_spin getEntryBySymbolic(std::string symbolic);

            virtual void print();


    };


} // namespace bias

#endif
