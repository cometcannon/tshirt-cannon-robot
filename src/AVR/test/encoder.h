#ifndef Encoder_h
#define Encoder_h

#include <Arduino.h>

#define TICKS_PER_MOTOR_REV         250.0 

class Encoder
{    
    
public:
    Encoder();
    Encoder(int pinA, int pinB, int interruptPinRef);
    int ReturnEncoderInterruptPinRef();
    int ReturnEncoderPinA();
    int ReturnEncoderPinB();
    long ReturnEncoderTickCount();
    void ZeroEncoderTickCount();
    void HandleEncoderPinAInterrupt();
    float MeasureAngularVelocity();
    float ReturnAngularVelocity();
    
private:
    int encoderInterruptPinRef;
    int encoderPinA;
    int encoderPinB;
    float angularVelocity;
    
    long previousEncoderTickCount;
    volatile long encoderTickCount;
    volatile bool encoderPinBState;

    unsigned long timeOfLastUpdate, now; 
    
};

#endif
