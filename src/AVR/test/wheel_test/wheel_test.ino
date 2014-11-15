#include <Arduino.h>
//#include <Bridge.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"
#include "wheel.h"

PIDController angularVelocityController(10, 0, 0, 0, 0, 0);
Encoder encoder(3, 4, 0);
Servo esc;
//Wheel wheel(6, &esc, &encoder, &angularVelocityController);

void setup()
{
  Serial.begin(9600);
  
  /*
  esc.attach(6);
  
  esc.writeMicroseconds(1500);
  delay(233);
  esc.writeMicroseconds(2000);
  delay(233);
  esc.writeMicroseconds(1000);
  delay(233);
  esc.writeMicroseconds(1500);
  
  
  esc.writeMicroseconds(1600);
  delay(2000);
  */
  
  attachInterrupt(encoder.ReturnEncoderInterruptPinRef(), handleEncoderPinAInterrupt, RISING);  
  
  
  /*
  esc.writeMicroseconds(1600);
  delay(2000);
  esc.writeMicroseconds(1650);
  delay(2000);
  esc.writeMicroseconds(1700);
  delay(2000);
  esc.writeMicroseconds(1750);
  delay(2000);
  
  esc.writeMicroseconds(1500);
  */
  
  
  //Bridge.begin();
  
  //Serial.begin(57600);
  
  //while(!Serial){}
  
  /*
  wheel.SetThrottle(1500);
  delay(233);
  wheel.SetThrottle(2000);
  delay(233);
  wheel.SetThrottle(1000);
  delay(233);
  wheel.SetThrottle(1500);
  
  delay(2000);
  
  wheel.SetThrottle(1600);
  */
}



void loop()
{
  /*
  Serial.println(wheel.currentThrottle);
  Serial.println(wheel.ESCPin);
  delay(2000);
  */
  
  Serial.println(encoder.MeasureAngularVelocity());
  delay(100);
}

void calibrate()
{
  
}

void handleEncoderPinAInterrupt()
{
    encoder.HandleEncoderPinAInterrupt();
}
