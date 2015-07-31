#include "MessageHandler.h"
#include "Streaming.h"
#include "DioPulse.h"

MessageHandler::MessageHandler()
{ }

void MessageHandler::processInput()
{
    while (Serial.available() > 0)
    {
        process(Serial.read());
        if (messageReady())
        {
            cmdSwitchYard();
            reset();
        }
    }
}

void MessageHandler::cmdSwitchYard()
{
    uint8_t cmdId;
    
    cmdId = readInt(0);

    switch (cmdId)
    {

        case CMD_ID_START_PULSE:
            DioPulse.startPulse();
            break;

        case CMD_ID_STOP_PULSE:
            DioPulse.stopPulse();
            break;

        case CMD_ID_SET_PULSE_LENGTH:
            {
                unsigned long pulseLength = (unsigned long)(readLong(1));
                DioPulse.setPulseLength(pulseLength);
                break;
            }

        case CMD_ID_GET_PULSE_LENGTH:
            {
                unsigned long pulseLength = DioPulse.getPulseLength();
                Serial << _DEC(pulseLength) << endl;
                break;
            }

        case CMD_ID_SET_OUTPUT_PIN:
            {
                int newPulsePin = readInt(1);
                DioPulse.setPulsePin(newPulsePin); 
                break;
            }

        case CMD_ID_GET_OUTPUT_PIN:
            {
                int pulsePin = DioPulse.getPulsePin();
                Serial << _DEC(pulsePin) << endl;
                break;
            }

        case CMD_ID_GET_ALLOWED_OUTPUT_PIN:
            sendAllowedOutputPin();
            break;

        default:
            break;
    }

}

void MessageHandler::sendAllowedOutputPin()
{
    for (int i=0; i<DioPulse.NUM_PULSE_PIN; i++)
    {
        int tmpPin = DioPulse.ALLOWED_PULSE_PIN[i];
        Serial << _DEC(tmpPin);
        if (i<DioPulse.NUM_PULSE_PIN-1)
        {
            Serial << ",";
        }
    }
    Serial << endl;
}

// Create instance
MessageHandler MsgHandler = MessageHandler();
