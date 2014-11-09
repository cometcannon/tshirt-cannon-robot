#ifndef Encoder_h
#define Encoder_h

#include <Arduino.h>

#define TICKS_PER_MOTOR_REV         250.0 

class Encoder
{    
    
public:
    Encoder();
    explicit Encoder(int pinA, int pinB, int interruptPinRef);
    int ReturnEncoderInterruptPinRef();
    long ReturnEncoderTickCount();
    void ZeroEncoderTickCount();
    void HandleEncoderPinAInterrupt();
    float MeasureAngularVelocity();
    
private:
    int encoderInterruptPinRef;
    int encoderPinB;
    
    long previousEncoderTickCount;
    volatile long encoderTickCount;
    volatile bool encoderPinBState;

    unsigned long timeOfLastUpdate, now; 
    
};

#endif
