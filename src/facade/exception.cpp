#include "exception.hpp"
#include "basic_types.hpp"
#include <sstream>

namespace bias {

    RuntimeError::RuntimeError( const unsigned int error_id, const std::string &what_arg)
        : std::runtime_error(what_arg)
    {
        error_id_ = error_id;
    }

    unsigned int RuntimeError::id()
    {
        return error_id_;
    }

    void throw_ERROR_NO_FC2(std::string prettyFunctionStr)
    {
        std::stringstream ssError;
        ssError << prettyFunctionStr;
        ssError << ": FlyCapure2 libary not present";
        throw RuntimeError(ERROR_NO_FC2, ssError.str());
    }

    void throw_ERROR_NO_DC1394(std::string prettyFunctionStr)
    {
        std::stringstream ssError;
        ssError << prettyFunctionStr;
        ssError << ": libdc1394 libary not present";
        throw RuntimeError(ERROR_NO_DC1394, ssError.str());
    }

    void throw_ERROR_NO_SPIN(std::string prettyFunctionStr)
    {
        std::stringstream ssError;
        ssError << prettyFunctionStr;
        ssError << ": FlyCapure2 libary not present";
        throw RuntimeError(ERROR_NO_SPIN, ssError.str());
    }

}
