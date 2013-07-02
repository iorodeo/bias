#ifndef BIAS_RTN_STATUS_HPP
#define BIAS_RTN_STATUS_HPP
#include <QString>

namespace bias
{

    struct RtnStatus 
    {
        bool success;
        QString message;
        RtnStatus() { success=true; message=QString(""); }
    };

} 

#endif // #ifndef BIAS_RTN_STATUS_HPP
