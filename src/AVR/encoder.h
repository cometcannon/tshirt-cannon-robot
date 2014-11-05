#ifndef Encoder_h
#define Encoder_h

#include <Arduino.h>
#include <digitalWriteFast.h>

class Encoder
{    
    
public:
    Encoder();
    explicit Encoder(char pinA, char pinB, char interruptPinRef);
    char ReturnEncoderInterruptPinRef();
    long ReturnEncoderTickCount();
    void ZeroEncoderTickCount();
    void HandleEncoderPinAInterrupt();
    float MeasureAngularVelocity();
    
private:
    char encoderInterruptPinRef;
    char encoderPinB;
    
    long previousEncoderTickCount;
    volatile long encoderTickCount;
    volatile bool encoderPinBState;

    unsigned long timeOfLastUpdate, now; 
    
};

#endif