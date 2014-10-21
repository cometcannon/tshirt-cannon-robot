#ifndef Wheel_h
#define Wheel_h

#include <Arduino.h>
#include <PIDController.h>
#include <encoder.h>
#include <digitalWriteFast.h>

/* Constants */
#define WHEEL_DIAMETER              0.1524 //Meters
#define GEAR_RATIO                  12.75
#define TICKS_PER_MOTOR_REV         250.0 
#define TICKS_PER_WHEEL_REV         GEAR_RATIO * TICKS_PER_WHEEL_REV

class Wheel
{     
public:
    Wheel();
    explicit Wheel(char _ESCPin, Encoder* _encoder = NULL, PIDController* _angularVelocityController = NULL);
    int ReturnThrottle();
    void SetThrottle(int throttle);
    void ControlAngularVelocity(float angularVelocitySetpoint);
    
private:
    Encoder* encoder;
    PIDController* angularVelocityController;

    char ESCPin
    int currentThrottle;
};

#endif