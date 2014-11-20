#include <Arduino.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"
#include "wheel.h"

PIDController angularVelocityController(10, 0, 0, 0, 0, 0);
Encoder encoder(3, 4, 0);
Servo esc;
Wheel wheel(6, &esc, &encoder, &angularVelocityController);

void setup()
{
    // esc.attach(6);

    // esc.writeMicroseconds(1500);
    // delay(233);
    // esc.writeMicroseconds(2000);
    // delay(233);
    // esc.writeMicroseconds(1000);
    // delay(233);
    // esc.writeMicroseconds(1500);
    // delay(2000);
    // esc.writeMicroseconds(1500);

    calibrate();
    Serial1.begin(115200);
    Serial.begin(115200);
}

void loop()
{
    getNextCommand();
    delay(10);
}

void calibrate()
{
    wheel.SetThrottle(1500);
    delay(233);
    wheel.SetThrottle(2000);
    delay(233);
    wheel.SetThrottle(1000);
    delay(233);
    wheel.SetThrottle(1500);
    delay(2000);
}

void getNextCommand()
{
    int command;
    if(Serial1.available())
    {
        byte _buffer[2];
        Serial1.readBytes(_buffer, 3);
        command = _buffer[1] + (_buffer[2] << 8);

        wheel.SetThrottle(command);
    }
}
