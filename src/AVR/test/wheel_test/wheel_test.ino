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
     /*
     esc.attach(6);
     
     esc.writeMicroseconds(1500);
     delay(2000);
     esc.writeMicroseconds(2000);
     delay(2000);
     esc.writeMicroseconds(1000);
     delay(2000);
     esc.writeMicroseconds(1500);
     delay(2000);
     esc.writeMicroseconds(1500);
     */

    //calibrate
    Serial1.begin(250000);
    Serial.begin(115200);
}

void loop()
{
    getNextCommand();
    /*
    if(Serial1.available())
    {
      char c = (char)Serial1.read();
      Serial.write(c);
    }
    */
    delay(100);
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
    float command;
    if(Serial1.available())
    {
        byte _buffer[2];
        Serial1.readBytes(_buffer, 3);
        command = ((int8_t)(_buffer[2]))*(1000.0/255.0) + 1502.0;

        wheel.SetThrottle(command);
        /*
        Serial.println(wheel.ReturnThrottle());
        Serial.print(_buffer[0]);
        Serial.print(" ");
        Serial.print(_buffer[1]);
        Serial.print(" ");
        Serial.print((int8_t)_buffer[2]);
        Serial.print(" ");
        Serial.println(command);
        /*
        
    }
}
