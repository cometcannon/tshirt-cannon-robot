#include <Wheel.h>

Wheel::Wheel(){}

Wheel::Wheel(char _ESCPin, Encoder* _encoder = NULL, PIDController* _angularVelocityController = NULL)
{
    pinMode(_ESCPin, INPUT);
    SetThrottle(0);

    encoder = _encoder;
    angularVelocityController = _angularVelocityController;

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
    currentThrottle = throttle;
}

void Wheel::ControlAngularVelocity(float angularVelocitySetpoint)
{
	if(angularVelocityController != NULL && encoder != NULL)
    	SetThrottle((int)angularVelocityController->ComputeOutput(encoder->MeasureAngularVelocity(), angularVelocitySetpoint));
}
