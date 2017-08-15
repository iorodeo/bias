#ifndef BASE_NODE_SPIN_HPP
#define BASE_NODE_SPIN_HPP
#include <string>
#include <cstdint>
#include "SpinnakerC.h"

namespace bias
{

    class BaseNode_spin
    {
        public:

            static const size_t MAX_BUF_LEN;

            BaseNode_spin();
            BaseNode_spin(spinNodeHandle hNode_);

            bool isAvailable();
            bool isReadable();

            spinNodeType type();
            bool isOfType(spinNodeType nodeType);

            std::string name();
            std::string displayName();
            std::string toolTip();
            std::string description();


        protected:

            spinNodeHandle hNode_ = nullptr;
            void checkNodeHandle();

    };


} // namespace bias

#endif
