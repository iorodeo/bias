#ifndef BIAS_EXCEPTION_HPP
#define BIAS_EXCEPTION_HPP

#include <string>
#include <stdexcept>

namespace bias {

    class RuntimeError : public std::runtime_error 
    {
        public:
            RuntimeError(const unsigned int error_id, const std::string &what_arg);
            unsigned int id();
        private:
            unsigned int error_id_;
    };

    void throw_ERROR_NO_FC2(std::string prettyFunctionStr);
    void throw_ERROR_NO_DC1394(std::string prettyFunctionStr);
    void throw_ERROR_NO_SPIN(std::string prettyFunctionStr);
}


#endif
