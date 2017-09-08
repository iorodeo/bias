#ifndef BOOL_NODE_SPIN_HPP
#define BOOL_NODE_SPIN_HPP
#include "base_node_spin.hpp"
#include <vector>
#include <map>

namespace bias
{

    class BoolNode_spin : public BaseNode_spin
    {
        public:

            static spinNodeType ExpectedType();

            BoolNode_spin();
            BoolNode_spin(spinNodeHandle hNode);
            BoolNode_spin(BaseNode_spin node);

            void setValue(bool value);
            bool value();

            virtual void print();

    };


} // namespace bias

#endif
