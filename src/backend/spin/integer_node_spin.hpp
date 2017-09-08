#ifndef INTEGER_NODE_SPIN_HPP
#define INTEGER_NODE_SPIN_HPP
#include "number_node_spin.hpp"

namespace bias
{

    class IntegerNode_spin : public NumberNode_spin<int64_t>
    {

        public:

            static spinNodeType ExpectedType();
            IntegerNode_spin();
            IntegerNode_spin(spinNodeHandle hNode);
            IntegerNode_spin(BaseNode_spin node);
            int64_t increment();

            virtual void print();

        protected:

            void initialize();


    };

} // namespace bias

#endif
