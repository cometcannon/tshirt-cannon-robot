#ifndef Wheel_h
#define Wheel_h

#include <Arduino.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"

#define WHEEL_DIAMETER              0.1524 //Meters
#define GEAR_RATIO                  12.75

class Wheel
{
    public:
    Wheel(int _ESCPin, Servo* _esc, Encoder* _encoder, PIDController* _angularVelocityController)
    {
        SetThrottle(1500);

        encoder = _encoder;
        angularVelocityController = _angularVelocityController;
        esc = _esc;

        ESCPin = _ESCPin;
    }

    int ReturnCurrentThrottle()
    { return currentThrottle; }

    char ReturnESCPin()
    { return ESCPin; }

    void SetThrottle(float throttle)
    {
        if (throttle == currentThrottle)
            return;

        if(throttle > 2000)
            throttle = 2000;
        else if (throttle < 1000)
            throttle = 1000;

        esc->writeMicroseconds(throttle);

        currentThrottle = throttle;
    }

    void ControlAngularVelocity(float angularVelocitySetpoint)
    {
        if(angularVelocityController != NULL && encoder != NULL)
            SetThrottle((int)angularVelocityController->ComputeOutput(encoder->ReturnAngularVelocity(), angularVelocitySetpoint) + 1500);
    }

    private:
    Encoder* encoder;
    PIDController* angularVelocityController;
    Servo* esc;

    int currentThrottle;
    char ESCPin;
};

#endif
