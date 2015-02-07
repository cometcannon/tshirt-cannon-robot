#ifndef Wheel_h
#define Wheel_h

#include <Arduino.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"

/* Constants */
#define WHEEL_DIAMETER              0.1524 //Meters
#define GEAR_RATIO                  12.75

class Wheel
{     
public:
    Wheel();
    Wheel(int _ESCPin, Servo* _esc, Encoder* _encoder, PIDController* _angularVelocityController);
    int ReturnCurrentThrottle();
    char ReturnESCPin();
    void SetThrottle(float throttle);
    void ControlAngularVelocity(float angularVelocitySetpoint);
    
private:
    Encoder* encoder;
    PIDController* angularVelocityController;
    Servo* esc;
    
    int currentThrottle;
    char ESCPin;
};

#endif
