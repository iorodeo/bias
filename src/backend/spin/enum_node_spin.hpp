#ifndef ENUM_NODE_SPIN_HPP
#define ENUM_NODE_SPIN_HPP
#include "base_node_spin.hpp"

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

    };


} // namespace bias

#endif
