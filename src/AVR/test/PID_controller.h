#ifndef PIDController_h
#define PIDController_h

#include <Arduino.h>
				
class PIDController
{
public:
    PIDController();
    PIDController(int sampleTimeInMSecs, double _kp, double _ki, double _kd, double _maxOutput, double _minOutput);
    double ComputeOutput(double input, double setPoint);
    void SetGains(double _kp, double _ki, double _kd);
    void SetSamplingPeriod(int newSamplePeriodInMSecs);
    void SetOutputLimits(double _maxOutput, double _minOutput);
    double ReturnControllerOutput();
    void ResetMemory();
    
private:
    double kp, ki, kd;
    unsigned long previousSampleTime;
    double controllerOutput, maxOutput, minOutput, previousInput;
    double integralOutput, proportionalOutput, derivativeOutput;
    int currentSamplingPeriodInMS;
};

#endif
