#ifndef STRING_NODE_SPIN_HPP
#define STRING_NODE_SPIN_HPP
#include "base_node_spin.hpp"

namespace bias
{
    class StringNode_spin : public BaseNode_spin
    {
        public:

            StringNode_spin();
            StringNode_spin(spinNodeHandle hNode);
            std::string value();

    };

} // namespace bias

#endif
