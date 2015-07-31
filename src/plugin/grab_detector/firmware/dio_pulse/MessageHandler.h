#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include "SerialReceiver.h"

class MessageHandler : public SerialReceiver
{
    public:
        static const uint8_t CMD_ID_START_PULSE = 1;
        static const uint8_t CMD_ID_STOP_PULSE = 2;
        static const uint8_t CMD_ID_SET_PULSE_LENGTH = 3;
        static const uint8_t CMD_ID_GET_PULSE_LENGTH = 4;
        static const uint8_t CMD_ID_SET_OUTPUT_PIN = 5;
        static const uint8_t CMD_ID_GET_OUTPUT_PIN = 6;
        static const uint8_t CMD_ID_GET_ALLOWED_OUTPUT_PIN = 7;

        MessageHandler();
        void processInput();

    protected:
        void cmdSwitchYard();
        void sendAllowedOutputPin();

};

extern MessageHandler MsgHandler;

#endif
