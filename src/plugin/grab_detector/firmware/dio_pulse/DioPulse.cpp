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
#if defined(__MK20DX256__)|| defined(__MK20DX128__) 
    // Teensy 3.xx
#else
    // Arduino
    Timer1.initialize(pulseLength_);
    Timer1.stop();
    Timer1.attachInterrupt(this->timerCallback);
#endif
    interruptCnt_ = 0;
}

void DioPulseDev::startPulse()
{
    digitalWrite(pulsePin_, HIGH);
    interruptCnt_ = 0;
#if defined(__MK20DX256__)|| defined(__MK20DX128__) 
    // Teensy 3.xx
    timer_.begin(this->timerCallback,pulseLength_);
#else
    // Arduino
    Timer1.start();
#endif
}

void DioPulseDev::stopPulse()
{
    // Note, set pulse low and stop timer on 2nd interrupt because interrupt is
    // generated on start.
#if defined(__MK20DX256__)|| defined(__MK20DX128__) 
    digitalWrite(pulsePin_, LOW);
    timer_.end(); 
#else
    if (interruptCnt_ > 0)
    {
        digitalWrite(pulsePin_, LOW);
        Timer1.stop(); 
    }
    else
    {
        interruptCnt_++;
    }
#endif
}


void DioPulseDev::setPulseLength(unsigned long pulseLength)
{
    pulseLength_ =  pulseLength;
#if defined(__MK20DX256__)|| defined(__MK20DX128__) 
    // Teensy 3.xx
#else
    Timer1.setPeriod(pulseLength_);
#endif
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


