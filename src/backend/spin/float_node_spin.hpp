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

            void setValueFromInt(unsigned int intValue);

            // For use with artificially import limits.
            unsigned int intValueWithLimits(double lowerLimit, double upperLimit);
            unsigned int intValueWithUpperLimit(double upperLimit);
            void setValueFromIntWithLimits(unsigned int intValue, double lowerLimit, double upperLimit);
            void setValueFromIntWithUpperLimit(unsigned int intValue, double upperLimit);


        protected:

            void initialize();

            double intToFloat(unsigned int intValue);
            unsigned int floatToInt(double floatValue);
            double intToFloatWithLimits(unsigned int intValue, double lowerLimit, double upperLimit);
            double intToFloatWithUpperLimit(unsigned int intValue, double upperLimit);
            unsigned int floatToIntWithLimits(double floatValue, double lowerLimit, double upperLimit);
            unsigned int floatToIntWithUpperLimit(double floatValue, double upperLimit);

    };

} // namespace bias

#endif
