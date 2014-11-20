#include <Arduino.h>
//#include <Bridge.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"
#include "wheel.h"

PIDController angularVelocityController(10, 0, 0, 0, 0, 0);
Encoder encoder(3, 4, 0);
Servo esc;
<<<<<<< HEAD
Wheel wheel(5, &esc, &encoder, &angularVelocityController);

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
    if(Serial.available())
    {
        byte _buffer[2];
        Serial.readBytes(_buffer, 2);
        command = _buffer[0] + (_buffer[1] << 8);

        wheel.SetThrottle(command);
    }
}
