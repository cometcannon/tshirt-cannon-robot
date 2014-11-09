#include "encoder.h"

Encoder::Encoder(){}

Encoder::Encoder(int pinA, int pinB, int interruptPinRef)
{
    encoderTickCount = 0;
    previousEncoderTickCount = 0;
    
    pinMode(pinA, INPUT);       
    digitalWrite(pinA, LOW);  
    pinMode(pinB, INPUT);
    digitalWrite(pinB, LOW);
    
    encoderInterruptPinRef = interruptPinRef;
    encoderPinB = pinB;
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

void Encoder::HandleEncoderPinAInterrupt()
{
    encoderPinBState = digitalRead(encoderPinB); //Will want to replace this with our own version of digitalReadFast()
    encoderTickCount -= encoderPinBState ? -1 : +1; 
}

float Encoder::MeasureAngularVelocity() 
{
    now = micros();
    float angularVelocity;
    
    if(now > timeOfLastUpdate)
    {
        float dt = (now - timeOfLastUpdate)/1000000.0;
        angularVelocity = (encoderTickCount - previousEncoderTickCount)/(TICKS_PER_MOTOR_REV * dt);
    }

    timeOfLastUpdate = now;
    
    previousEncoderTickCount = encoderTickCount;
    
    return angularVelocity;
}
