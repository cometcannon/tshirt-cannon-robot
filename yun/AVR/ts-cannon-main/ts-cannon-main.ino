#include <Arduino.h>
#include <Servo.h>
#include "horn.h"
#include "cannon.h"
#include "wheel.h"
#include "PinChangeInt.h"
#include "Streaming.h"

/*      Arduino Yun Pins **(Note that encoder pins have not been finalized)
        *** means unused

2       [INPUT]     Front Left Encoder Pin A
3~      [INPUT]     Front Right Encoder Pin A
4       [INPUT]     Front Left Encoder Pin B
5~      [OUTPUT]    Back Right Speed Controller PWM Pin
6~      [OUTPUT]    Back Left Speed Controller PWM Pin
7       [INPUT]     Back Right Encoder Pin A

8       [INPUT]     Front Right Encoder Pin B
9~      [OUTPUT]    Front Right Speed Controller PWM Pin
10~     [OUTPUT]    Front Left Speed Controller PWM Pin
11~     [OUTPUT]    Horn Pin
12      [INPUT]     Back Left Encoder Pin A
13      [OUTPUT]    Arduino LED Pin

AREF    ***
SDA     ***
SCL     ***

A0      [OUTPUT]    Pressure Regulator Pin
A1      [OUTPUT]    Cannon Trigger Pin
A2      [INPUT]     Pressure Sensor Pin
A3      [INPUT]     Back Right Encoder Pin B
A4      [INPUT]     Back Left Encoder Pin B
A5      ***

 */

#define FL_WHEEL_ESCPIN     10
#define FR_WHEEL_ESCPIN     9
#define BR_WHEEL_ESCPIN     5
#define BL_WHEEL_ESCPIN     6

#define FL_ENCODER_PIN_A    2
#define FR_ENCODER_PIN_A    3
#define BR_ENCODER_PIN_A    7
#define BL_ENCODER_PIN_A    12

#define FL_ENCODER_PIN_B    4
#define FR_ENCODER_PIN_B    8
#define BR_ENCODER_PIN_B    A3
#define BL_ENCODER_PIN_B    A4

#define FL_ENCODER_INTREF   0
#define FR_ENCODER_INTREF   1
#define BR_ENCODER_INTREF   4
#define BL_ENCODER_INTREF   12

#define CANNON_TRIG_PIN     A1
#define PRESS_REG_PIN       A0
#define PRESS_SENSOR_PIN    A2

#define HORN_PIN            11   //This pin must be a pwm pin

Horn horn(HORN_PIN);

Encoder fl_Encoder(FL_ENCODER_PIN_A, FL_ENCODER_PIN_B, FL_ENCODER_INTREF);
Encoder fr_Encoder(FR_ENCODER_PIN_A, FR_ENCODER_PIN_B, FR_ENCODER_INTREF);
Encoder br_Encoder(BR_ENCODER_PIN_A, BR_ENCODER_PIN_B, BR_ENCODER_INTREF);
Encoder bl_Encoder(BL_ENCODER_PIN_A, BL_ENCODER_PIN_B, BL_ENCODER_INTREF);

Wheel fl_Wheel(FL_WHEEL_ESCPIN, &fl_Encoder);
Wheel fr_Wheel(FR_WHEEL_ESCPIN, &fr_Encoder);
Wheel br_Wheel(BR_WHEEL_ESCPIN, &br_Encoder);
Wheel bl_Wheel(BL_WHEEL_ESCPIN, &bl_Encoder);

Cannon cannon(CANNON_TRIG_PIN, PRESS_REG_PIN, PRESS_SENSOR_PIN);

bool debug = false;

const float MAX_WHEEL_SPEED = 5.5; //m/s. Could be slightly higher
const int MIN_PRESSURE = 200;

unsigned long keepAliveTimer;
unsigned long keepAliveTimeout = 1000;

unsigned long angVelTimer;
unsigned long angVelTimeout = 50;

byte commandBuffer[64];
byte commandBufferIndex = 0;

byte magicBytes[] = {0x47, 0x41, 0x4e, 0x53};
byte commandLengthArray[] = {5, 5, 7, 9, 5, 5, 7, 9, 5, 5, 5};

void setup()
{
    Serial.begin(57600);
    Serial1.begin(250000);

    fl_Wheel.initialize();
    fr_Wheel.initialize();
    br_Wheel.initialize();
    bl_Wheel.initialize();

    attachInterrupt(fl_Encoder.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt0, RISING);
    attachInterrupt(fr_Encoder.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt1, RISING);
    attachInterrupt(br_Encoder.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt2, RISING);
    attachPinChangeInterrupt(bl_Encoder.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt3, RISING);

    keepAliveTimer = millis();
    angVelTimer = millis();
}

void loop()
{
    if(millis() - angVelTimer > angVelTimeout)
    {
        fl_Encoder.MeasureAngularVelocity();
        fr_Encoder.MeasureAngularVelocity();
        br_Encoder.MeasureAngularVelocity();
        bl_Encoder.MeasureAngularVelocity();

        angVelTimer = millis();
    }

    cannon.HandleRegulatorTrigger();
    cannon.HandleCannonTrigger();

    horn.HandleHonking();

    if(millis() - keepAliveTimer > keepAliveTimeout)
        KillMotors();

    CheckForCommand();
}

void KillMotors()
{
    CommandAllMotorThrottles(0, 0, 0, 0);
}

void HonkHorn()
{
    horn.Honk();

    if(debug)
        Serial1 << "Honk Horn\n";
}

void CommandMotorThrottle(int8_t motor, int8_t value)
{
    float command;

    if (motor == 1 || motor == 2)
        command = -value * (1000.0 / 255.0) + 1502.0;
    else
        command = value * (1000.0 / 255.0) + 1502.0;

    switch (motor)
    {
        case 0:
            fl_Wheel.SetThrottle(command);
            break;

        case 1:
            fr_Wheel.SetThrottle(command);
            break;

        case 2:
            br_Wheel.SetThrottle(command);
            break;

        case 3:
            bl_Wheel.SetThrottle(command);
            break;

    }

    if(debug)
        Serial1 << "Command Single Motor " << motor << ": " << value << "\n";
}

void CommandAllMotorThrottles(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    float command1 = value1 * (1000.0 / 255.0) + 1502.0;
    float command2 = -value2 * (1000.0 / 255.0) + 1502.0;
    float command3 = -value3 * (1000.0 / 255.0) + 1502.0;
    float command4 = value4 * (1000.0 / 255.0) + 1502.0;

    fl_Wheel.SetThrottle(command1);
    fr_Wheel.SetThrottle(command2);
    br_Wheel.SetThrottle(command3);
    bl_Wheel.SetThrottle(command4);

    if(debug)
        Serial1 << "Command All Motors: " << command1 << " " << command2 << " " << command3 << " " << command4 << "\n";
}

void DebugEncoders(int8_t motor)
{
    switch(motor)
    {
        case 0:
            Serial1 << digitalRead(fl_Encoder.ReturnEncoderPinA()) << "\t" <<
                digitalRead(fl_Encoder.ReturnEncoderPinB()) << "\t" <<
                fl_Encoder.ReturnEncoderTickCount() << "\t" <<
                fl_Encoder.ReturnAngularVelocity() << "\n";
            break;

        case 1:
            Serial1 << digitalRead(fr_Encoder.ReturnEncoderPinA()) << "\t" <<
                digitalRead(fr_Encoder.ReturnEncoderPinB()) << "\t" <<
                fr_Encoder.ReturnEncoderTickCount() << "\t" <<
                fr_Encoder.ReturnAngularVelocity() << "\n";
            break;

        case 2:
            Serial1 << digitalRead(br_Encoder.ReturnEncoderPinA()) << "\t" <<
                digitalRead(br_Encoder.ReturnEncoderPinB()) << "\t" <<
                br_Encoder.ReturnEncoderTickCount() << "\t" <<
                br_Encoder.ReturnAngularVelocity() << "\n";
            break;

        case 3:
            Serial1 << digitalRead(bl_Encoder.ReturnEncoderPinA()) << "\t" <<
                digitalRead(bl_Encoder.ReturnEncoderPinB()) << "\t" <<
                bl_Encoder.ReturnEncoderTickCount() << "\t" <<
                bl_Encoder.ReturnAngularVelocity() << "\n";
            break;

    }
}

void SendMotorAngularVelocity()
{
    int8_t angVel0 = map(fl_Encoder.ReturnAngularVelocity(), -MAX_WHEEL_SPEED, MAX_WHEEL_SPEED, -128, 127);
    int8_t angVel1 = map(fr_Encoder.ReturnAngularVelocity(), -MAX_WHEEL_SPEED, MAX_WHEEL_SPEED, -128, 127);
    int8_t angVel2 = map(br_Encoder.ReturnAngularVelocity(), -MAX_WHEEL_SPEED, MAX_WHEEL_SPEED, -128, 127);
    int8_t angVel3 = map(bl_Encoder.ReturnAngularVelocity(), -MAX_WHEEL_SPEED, MAX_WHEEL_SPEED, -128, 127);


    for(int i = 0; i < sizeof magicBytes; i++)
        Serial1 << magicBytes[i];

    Serial1 << 0 << angVel0 << angVel1 << angVel2 << angVel3;

}

void FireCannon()
{
    cannon.Fire();

    if(debug)
        Serial1 << "Cannon Fired\n";
}

void CommandMotorAngularVelocity(int8_t motor, int8_t value)
{
    float command;

    if (motor == 1 || motor == 2)
        command = -(((float)value + 0.5)) * MAX_WHEEL_SPEED/127.5;
    else
        command = ((float)value + 0.5) * MAX_WHEEL_SPEED/127.5;

    switch (motor)
    {
        case 0:
            fl_Wheel.ControlAngularVelocity(command);
            break;

        case 1:
            fr_Wheel.ControlAngularVelocity(command);
            break;

        case 2:
            br_Wheel.ControlAngularVelocity(command);
            break;

        case 3:
            bl_Wheel.ControlAngularVelocity(command);
            break;

    }

    if(debug)
        Serial1 << "Command Single Motor Vel" << motor << ": " << command << "\n";
}

void CommandAllMotorAngularVelocities(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    float command0 = ((float)value1 + 0.5) * MAX_WHEEL_SPEED/127.5;
    float command1 = -(((float)value2 + 0.5)) * MAX_WHEEL_SPEED/127.5;
    float command2 = -(((float)value3 + 0.5)) * MAX_WHEEL_SPEED/127.5;
    float command3 = ((float)value4 + 0.5) * MAX_WHEEL_SPEED/127.5;

    fl_Wheel.ControlAngularVelocity(command0);
    fr_Wheel.ControlAngularVelocity(command1);
    br_Wheel.ControlAngularVelocity(command2);
    bl_Wheel.ControlAngularVelocity(command3);

    if(debug)
        Serial1 << "Command All Motor Vels: " << command0 << " " << command1 <<
        " " << command2 << " " << command3 << "\n";
}

void IncreasePressure()
{
    cannon.IncreasePressure();

    //if(debug)
        Serial1 << "Current Pressure is " << cannon.MeasurePressure() << "\n";
}

void SendPressure()
{
    for(int i = 0; i < sizeof magicBytes; i++)
        Serial1 << magicBytes[i];

    Serial1 << (int8_t) 1 << cannon.MeasurePressure();
}

void processCommand()
{
    switch (commandBuffer[4])
    {
        case 0:
            break;

        case 1:
            KillMotors();
            break;

        case 2:
            HonkHorn();
           break;

        case 3:
            CommandAllMotorThrottles(commandBuffer[5],
                                    commandBuffer[6],
                                    commandBuffer[7],
                                    commandBuffer[8]);

            break;

        case 4:
            SendMotorAngularVelocity();
            break;

        case 5:
            FireCannon();
            break;

        case 6:
            CommandMotorAngularVelocity(commandBuffer[5], commandBuffer[6]);
            break;

        case 7:
            CommandAllMotorAngularVelocities(commandBuffer[5],
                                            commandBuffer[6],
                                            commandBuffer[7],
                                            commandBuffer[8]);
            break;

        case 8:
            IncreasePressure();
            break;

        case 9:
            SendPressure();
            break;

        case 10:
            debug = !debug;
            break;
    }

      keepAliveTimer = millis();

}

void CheckForCommand()
{
    int data = Serial1.read();
    if(data > -1)
    {
      commandBuffer[commandBufferIndex] = data;

      if(commandBufferIndex <= 3 && commandBuffer[commandBufferIndex] != magicBytes[commandBufferIndex])
      {
          memset(commandBuffer, 0, commandBufferIndex + 1);
          commandBufferIndex = 0;
      }

      else if(commandLengthArray[ commandBuffer[4] ] <= commandBufferIndex + 1)
      {
          processCommand();
          memset(commandBuffer, 0, commandBufferIndex + 1);
          commandBufferIndex = 0;
      }

      else
        commandBufferIndex++;
    }

}

void HandleEncoderPinAInterrupt0()
{
    fl_Encoder.HandleEncoderPinAInterrupt();
}

void HandleEncoderPinAInterrupt1()
{
    fr_Encoder.HandleEncoderPinAInterrupt();
}

void HandleEncoderPinAInterrupt2()
{
    br_Encoder.HandleEncoderPinAInterrupt();
}

void HandleEncoderPinAInterrupt3()
{
    bl_Encoder.HandleEncoderPinAInterrupt();
}
