#include "Streaming.h"
#include "DioPulse.h"
#include "TimerOne.h"

DioPulseDev::DioPulseDev() 
{
    pulsePin_ = DEFAULT_PULSE_PIN;
    pulseLength_ = DEFAULT_PULSE_LENGTH;
    interruptCnt_ = 0;
}


void DioPulseDev::initialize()
{
    pinMode(pulsePin_,OUTPUT);
    digitalWrite(pulsePin_, LOW);
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


void DioPulseDev::timerCallback()
{
    DioPulse.stopPulse();
}

// Create instance
DioPulseDev DioPulse = DioPulseDev();


