#include "wheel.h"

Wheel::Wheel(){}

Wheel::Wheel(int _ESCPin, Servo* _esc, Encoder* _encoder, PIDController* _angularVelocityController)
{
    SetThrottle(0);

    encoder = _encoder;
    angularVelocityController = _angularVelocityController;
    esc = _esc;
    
    ESCPin = _ESCPin;
}

int Wheel::ReturnCurrentThrottle()
{
    return currentThrottle;
}

char Wheel::ReturnESCPin()
{
    return ESCPin;
}

void Wheel::SetThrottle(float throttle)
{
    if(throttle > 2000)
        throttle = 2000;
    else if (throttle < 1000)
        throttle = 1000;

    esc->writeMicroseconds(throttle);

    currentThrottle = throttle;
}

void Wheel::ControlAngularVelocity(float angularVelocitySetpoint)
{
    if(angularVelocityController != NULL && encoder != NULL)
        SetThrottle((int)angularVelocityController->ComputeOutput(encoder->MeasureAngularVelocity(), angularVelocitySetpoint));
}
