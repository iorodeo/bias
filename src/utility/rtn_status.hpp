#ifndef BIAS_RTN_STATUS_HPP
#define BIAS_RTN_STATUS_HPP
#include <QString>

namespace bias
{

    class RtnStatus 
    {
        public:

            RtnStatus() { success=true; message=QString(""); }

            void appendMessage(QString moreMessage) 
            { 
                if (message.isEmpty())
                {
                    message = moreMessage;
                }
                else
                {
                    message = QString("%1, %2").arg(message).arg(moreMessage);
                }
            }

            bool success;
            QString message;
    };

} 

#endif // #ifndef BIAS_RTN_STATUS_HPP
