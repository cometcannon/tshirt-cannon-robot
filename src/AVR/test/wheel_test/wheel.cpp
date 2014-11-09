#include "wheel.h"

Wheel::Wheel(){}

Wheel::Wheel(int _ESCPin, Servo* _esc, Encoder* _encoder, PIDController* _angularVelocityController)
{
    pinMode(_ESCPin, INPUT);
    SetThrottle(0);

    encoder = _encoder;
    angularVelocityController = _angularVelocityController;

    esc = _esc;
    esc->attach(_ESCPin);

    ESCPin = _ESCPin;
}

int Wheel::ReturnThrottle()
{
    return currentThrottle;
}

void Wheel::SetThrottle(int throttle)
{
/*
    Will have to test the ESC to determine how to implement this function
*/
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
