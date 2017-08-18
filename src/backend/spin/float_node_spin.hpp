#ifndef FLOAT_NODE_SPIN_HPP
#define FLOAT_NODE_SPIN_HPP
#include "number_node_spin.hpp"

namespace bias
{

    class FloatNode_spin : public NumberNode_spin<double>
    {

        public:

            static spinNodeType ExpectedType();
            FloatNode_spin();
            FloatNode_spin(spinNodeHandle hNode);
            FloatNode_spin(BaseNode_spin node);
            std::string unit();

        protected:

            void initialize();

    };

} // namespace bias

#endif
