#include <Arduino.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"
#include "wheel.h"

PIDController angularVelocityController(10, 0, 0, 0, 0, 0);
Encoder encoder(3, 4, 0);
Servo esc;
//Wheel wheel(5, &esc, &encoder, &angularVelocityController);

void setup()
{
  esc.attach(5);
  
  esc.writeMicroseconds(1500);
  delay(233);
  esc.writeMicroseconds(2000);
  delay(233);
  esc.writeMicroseconds(1000);
  delay(233);
  esc.writeMicroseconds(1500);
}

void loop(){}
