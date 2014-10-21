#include <encoder.h>

Encoder::Encoder(){}

Encoder::Encoder(char pinA, char pinB, char interruptPinRef)
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

byte Encoder::ReturnEncoderInterruptPinRef()
{
    return encoderInterruptPinRef;
}

void Encoder::HandleEncoderPinAInterrupt()
{
    encoderPinBState = digitalReadFast(encoderPinB); //Will want to replace this with our own version of digitalReadFast()
    encoderTickCount -= encoderPinBState ? -1 : +1; 
}

float Encoder::MeasureAngularVelocity() 
{
    now = micros();
    float angularVelocity;
    
    if(now > timeOfLastUpdate)
    {
        float dt = (now - timeOfLastUpdate)/1000000.0;
        angularVelocity = (encoderTickCount - previousEncoderTickCount)/(TICKS_PER_REV * dt);
        timeOfLastUpdate = now;
    }
    else
        timeOfLastUpdate = now;
    
    previousEncoderTickCount = encoderTickCount;
    
    return angularVelocity;
}
