#ifndef DIO_PULSE_H
#define DIO_PULSE_H

class DioPulseDev
{
    public:

        static const int NUM_PULSE_PIN = 9;
        static const int ALLOWED_PULSE_PIN[NUM_PULSE_PIN];
        static const int DEFAULT_PULSE_PIN = 4;
        static const unsigned long DEFAULT_PULSE_LENGTH = 10000; // us

        DioPulseDev();
        void initialize();
        void startPulse();
        void stopPulse();
        void setPulseLength(unsigned long pulseLength);
        unsigned long getPulseLength();
        void setPulsePin(int pulsePin);
        int getPulsePin();

    protected:
        int pulsePin_;
        int interruptCnt_;
        unsigned long pulseLength_;
        static void timerCallback();

};


extern DioPulseDev DioPulse;

#endif
