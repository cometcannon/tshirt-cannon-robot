#ifndef CANNON_H
#define CANNON_H

#include <Arduino.h>

class Cannon
{
public:
    Cannon(int8_t cannonTriggerPin, int8_t pressureRegulatorPin, int8_t pressureSensorPin)
    {
        this->cannonTriggerPin = cannonTriggerPin;
        this->pressureRegulatorPin = pressureRegulatorPin;
        this->pressureSensorPin = pressureSensorPin;

        regulatorTriggered = false;
        regulatorTriggeredTime = millis();
        regulatorTriggeredTimeout = 200;

        cannonTriggered = false;
        cannonTriggerTime = millis();
        cannonTriggerTimeout = 1000;

        minPressure = 80;

        pinMode(cannonTriggerPin, OUTPUT);
        digitalWrite(cannonTriggerPin, LOW);

        pinMode(pressureRegulatorPin, OUTPUT);
        digitalWrite(pressureRegulatorPin, LOW);

        pinMode(pressureSensorPin, INPUT);
        digitalWrite(pressureSensorPin, LOW);

    }

    void HandleRegulatorTrigger()
    {
        if(MeasurePressure() < minPressure)
            IncreasePressure();

        else if(regulatorTriggered &&
                millis() - regulatorTriggeredTime > regulatorTriggeredTimeout)
        {
            digitalWrite(pressureRegulatorPin, LOW);
            regulatorTriggered = false;
        }
    }

    void HandleCannonTrigger()
    {
        if(cannonTriggered &&
            millis() - cannonTriggerTime > cannonTriggerTimeout)
        {
            digitalWrite(cannonTriggerPin, LOW);
            cannonTriggered = false;
        }
    }

    void Fire()
    {
        digitalWrite(cannonTriggerPin, HIGH);
        cannonTriggerTime = millis();
        cannonTriggered = true;
    }

    void IncreasePressure()
    {
        digitalWrite(pressureRegulatorPin, HIGH);
        regulatorTriggeredTime = millis();
        regulatorTriggered = true;
    }

    uint8_t MeasurePressure()
    {
        int pressure = analogRead(pressureSensorPin);
        return map(pressure, 0, 1023, 0, 200);
    }

private:
    int8_t cannonTriggerPin;
    int8_t pressureRegulatorPin;
    int8_t pressureSensorPin;

    bool regulatorTriggered;
    unsigned long regulatorTriggeredTime;
    unsigned long regulatorTriggeredTimeout;

    bool cannonTriggered;
    unsigned long cannonTriggerTime;
    unsigned long cannonTriggerTimeout;

    uint8_t minPressure; //This is not yet being used. It will eventually be used to ensure that the cannon recharges to this pressure automatically after being fired.
};

#endif
