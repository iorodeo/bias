#include "Streaming.h"
#include "TimerOne.h"
#include "SerialReceiver.h"
#include "MessageHandler.h"
#include "DioPulse.h"

const unsigned long BAUDRATE = 9600; 

void setup()
{
    Serial.begin(BAUDRATE);
    DioPulse.initialize();
}


void loop()
{
    MsgHandler.processInput();
}
