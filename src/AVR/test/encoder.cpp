#include "encoder.h"

Encoder::Encoder(){}

Encoder::Encoder(int pinA, int pinB, int interruptPinRef)
{
    encoderTickCount = 0;
    previousEncoderTickCount = 0;
    encoderInterruptPinRef = interruptPinRef;
    encoderPinA = pinA;
    encoderPinB = pinB;
    angularVelocity = 0;
}

void Encoder::ZeroEncoderTickCount()
{
    encoderTickCount = 0;
}

long Encoder::ReturnEncoderTickCount()
{
    return encoderTickCount;
}

int Encoder::ReturnEncoderInterruptPinRef()
{
    return encoderInterruptPinRef;
}

int Encoder::ReturnEncoderPinA()
{
    return encoderPinA;
}

int Encoder::ReturnEncoderPinB()
{
    return encoderPinB;
}

void Encoder::HandleEncoderPinAInterrupt()
{
    encoderTickCount += digitalRead(encoderPinB) ? -1 : +1; //Will want to replace this with our own version of digitalReadFast()
}

float Encoder::MeasureAngularVelocity() 
{
    now = micros();
    
    if(now > timeOfLastUpdate)
    {
        float dt = (now - timeOfLastUpdate)/1000000.0;
        angularVelocity = (encoderTickCount - previousEncoderTickCount)/(TICKS_PER_MOTOR_REV * dt);
    }

    timeOfLastUpdate = now;
    
    previousEncoderTickCount = encoderTickCount;
    
    return angularVelocity;
}

float Encoder::ReturnAngularVelocity()
{
    return angularVelocity;
}
