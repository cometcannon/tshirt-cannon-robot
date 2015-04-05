#ifndef Encoder_h
#define Encoder_h

#include <Arduino.h>

#define TICKS_PER_MOTOR_REV         250.0

class Encoder
{
public:
    Encoder(int pinA, int pinB, int interruptPinRef)
    {
        encoderTickCount = 0;
        previousEncoderTickCount = 0;
        encoderInterruptPinRef = interruptPinRef;
        encoderPinA = pinA;
        encoderPinB = pinB;
        angularVelocity = 0;
        timeOfLastUpdate = micros();
    }

    int ReturnEncoderInterruptPinRef()
    { return encoderInterruptPinRef; }

    int ReturnEncoderPinA()
    { return encoderPinA; }

    int ReturnEncoderPinB()
    { return encoderPinB; }

    long ReturnEncoderTickCount()
    { return encoderTickCount; }

    void ZeroEncoderTickCount()
    { encoderTickCount = 0; }

    void HandleEncoderPinAInterrupt()
    { encoderTickCount += digitalRead(encoderPinB) ? -1 : +1; }

    float MeasureAngularVelocity()
    {
        unsigned long now = micros();
        float dt = now - timeOfLastUpdate;

        if(dt < 0)
        {
            timeOfLastUpdate = now;
            return angularVelocity;
        }

        if(dt < 100)
            return angularVelocity;
        else
        {
            dt = dt/1000000.0;
            angularVelocity = (encoderTickCount - previousEncoderTickCount)/(TICKS_PER_MOTOR_REV * dt);
        }

        timeOfLastUpdate = now;

        previousEncoderTickCount = encoderTickCount;

        return angularVelocity;
    }

    float ReturnAngularVelocity()
    { return angularVelocity; }

private:
    int encoderInterruptPinRef;
    int encoderPinA;
    int encoderPinB;
    float angularVelocity;

    long previousEncoderTickCount;
    volatile long encoderTickCount;
    volatile bool encoderPinBState;

    unsigned long timeOfLastUpdate;

};

#endif
