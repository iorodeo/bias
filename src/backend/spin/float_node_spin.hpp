#ifndef FLOAT_NODE_SPIN_HPP
#define FLOAT_NODE_SPIN_HPP
#include "base_node_spin.hpp"

namespace bias
{

    class FloatNode_spin : public BaseNode_spin
    {

        public:

            static spinNodeType ExpectedType();

            FloatNode_spin();
            FloatNode_spin(spinNodeHandle hNode);
            FloatNode_spin(BaseNode_spin node);

            double value();
            void setValue(double value);

            double minValue();
            double maxValue();
            std::string unit();

    };

} // namespace bias

#endif
