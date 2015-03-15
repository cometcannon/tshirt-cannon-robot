#include "PID_Controller.h"

PIDController::PIDController(){}

PIDController::PIDController(int samplePeriod, double _kp, double _ki, double _kd, double _maxOutput, double _minOutput)
{
    currentSamplingPeriodInMS = samplePeriod;
    ResetMemory();
    SetGains(_kp, _ki, _kd);
    SetOutputLimits(_maxOutput, _minOutput);
}

double PIDController::ComputeOutput(double input, double setPoint)
{
    int currentSampleTime = millis();

    if(previousSampleTime > currentSampleTime)
        previousSampleTime = millis();

    if((currentSampleTime - previousSampleTime) > currentSamplingPeriodInMS)
    {
        proportionalOutput = kp * (setPoint - input);
        integralOutput += ki * (setPoint - input);
        derivativeOutput = kd * (previousInput - input);

        if(integralOutput > maxOutput)
            integralOutput = maxOutput;
        else if(integralOutput < minOutput)
            integralOutput = minOutput;

        controllerOutput = proportionalOutput + integralOutput + derivativeOutput;

        if(controllerOutput > maxOutput)
            controllerOutput = maxOutput;
        else if(controllerOutput < minOutput)
            controllerOutput = minOutput;

        previousInput = input;
        previousSampleTime = currentSampleTime;
    }

    return controllerOutput;
}

void PIDController::SetGains(double _kp, double _ki, double _kd)
{
    kp = _kp;
    ki = _ki;
    kd = _kd;
}

void PIDController::SetSamplingPeriod(int newSamplePeriod)
{

    ki /= (double)newSamplePeriod/(double)currentSamplingPeriodInMS;
    kd *= (double)newSamplePeriod/(double)currentSamplingPeriodInMS;

    currentSamplingPeriodInMS = newSamplePeriod;
}

void PIDController::SetOutputLimits(double _maxOutput, double _minOutput)
{
    maxOutput = _maxOutput;
    minOutput = _minOutput;
}

double PIDController::ReturnControllerOutput()
{
    return controllerOutput;
}

void PIDController::ResetMemory()
{
    integralOutput = 0;
    previousSampleTime = millis();
    previousInput = 0;
    controllerOutput = 0;
}
