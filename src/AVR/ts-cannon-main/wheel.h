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
    Wheel(int8_t ESCPin, Encoder* encoder)
    {
        this->ESCPin = ESCPin;
        this->encoder = encoder;

        angularVelocityController =
                    PIDController(10, 0.0, 0.0, 0.0, 500.0, -500.0);

    }

    void initialize()
    {
        esc.attach(ESCPin);
        SetThrottle(1500);
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

        esc.writeMicroseconds(throttle);

        currentThrottle = throttle;
    }

    void ControlAngularVelocity(float angularVelocitySetpoint)
    {
        if(encoder != NULL)
            SetThrottle((int)angularVelocityController.ComputeOutput(
                        encoder->ReturnAngularVelocity(),
                        angularVelocitySetpoint) + 1500);
    }

private:
    Encoder* encoder;
    PIDController angularVelocityController;
    Servo esc;

    int currentThrottle;
    char ESCPin;
};

#endif
