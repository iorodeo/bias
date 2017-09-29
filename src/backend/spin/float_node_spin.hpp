#ifndef FLOAT_NODE_SPIN_HPP
#define FLOAT_NODE_SPIN_HPP
#include "number_node_spin.hpp"

namespace bias
{

    class FloatNode_spin : public NumberNode_spin<double>
    {

        public:

            static unsigned int NumberOfIntValues;
            static spinNodeType ExpectedType();

            FloatNode_spin();
            FloatNode_spin(spinNodeHandle hNode);
            FloatNode_spin(BaseNode_spin node);
            std::string unit();
            virtual void print();

            unsigned int intValue();
            unsigned int minIntValue();
            unsigned int maxIntValue();

            double intToFloat(unsigned int intValue);
            unsigned int floatToInt(double floatValue);
            void setValueFromInt(unsigned int intValue);


        protected:

            void initialize();

    };

} // namespace bias

#endif
