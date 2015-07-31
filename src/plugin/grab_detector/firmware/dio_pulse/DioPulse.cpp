#include "Streaming.h"
#include "DioPulse.h"
#include "TimerOne.h"

const int DioPulseDev::ALLOWED_PULSE_PIN[NUM_PULSE_PIN] = {2,3,4,5,6,7,8,9,10};

DioPulseDev::DioPulseDev() 
{
    pulsePin_ = DEFAULT_PULSE_PIN;
    pulseLength_ = DEFAULT_PULSE_LENGTH;
    interruptCnt_ = 0;
}


void DioPulseDev::initialize()
{
    for (int i=0; i<NUM_PULSE_PIN; i++)
    {
        int tmpPin = ALLOWED_PULSE_PIN[i];
        pinMode(tmpPin,OUTPUT);
        digitalWrite(tmpPin, LOW);
    }
    Timer1.initialize(pulseLength_);
    Timer1.stop();
    Timer1.attachInterrupt(this->timerCallback);
    interruptCnt_ = 0;
}

void DioPulseDev::startPulse()
{
    digitalWrite(pulsePin_, HIGH);
    interruptCnt_ = 0;
    Timer1.start();
}

void DioPulseDev::stopPulse()
{
    // Note, set pulse low and stop timer on 2nd interrupt because interrpt is
    // generated on start.
    if (interruptCnt_ > 0)
    {
        Timer1.stop();
        digitalWrite(pulsePin_, LOW);
    }
    else
    {
        interruptCnt_++;
    }
}


void DioPulseDev::setPulseLength(unsigned long pulseLength)
{
    pulseLength_ =  pulseLength;
    Timer1.setPeriod(pulseLength_);
}

unsigned long DioPulseDev::getPulseLength()
{
    return pulseLength_;
}

int DioPulseDev::getPulsePin()
{
    return pulsePin_;
}


void DioPulseDev::setPulsePin(int pulsePin)
{
    bool isValidPin = false;
    for (int i=0;  i<NUM_PULSE_PIN; i++)
    {
        if (pulsePin == ALLOWED_PULSE_PIN[i])
        {
            isValidPin = true;
            break;
        }
    }
    if (isValidPin)
    {
        pulsePin_ = pulsePin;
    } 
}


void DioPulseDev::timerCallback()
{
    DioPulse.stopPulse();
}

// Create instance
DioPulseDev DioPulse = DioPulseDev();


