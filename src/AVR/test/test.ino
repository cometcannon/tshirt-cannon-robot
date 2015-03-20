#include <Arduino.h>
#include <Servo.h>
#include "wheel.h"
#include "PinChangeInt.h"
#include "Streaming.h"

Servo esc0;
Servo esc1;
Servo esc2;
Servo esc3;

PIDController angularVelocityController0(10, 100, 1, 0, 500, -500);
PIDController angularVelocityController1(10, 1, 1, 1, 500, -500);
PIDController angularVelocityController2(10, 0, 0.5, 0, 500, -500);
PIDController angularVelocityController3(10, 0, 0, 0, 500, -500);

Encoder encoder0(2, 4, 0);
Encoder encoder1(3, 8, 1);
Encoder encoder2(7, 11, 4);
Encoder encoder3(12, 13, 12);

Wheel wheel0(5,  &esc0, &encoder0, &angularVelocityController0);
Wheel wheel1(6,  &esc1, &encoder1, &angularVelocityController1);
Wheel wheel2(9,  &esc2, &encoder2, &angularVelocityController2);
Wheel wheel3(10, &esc3, &encoder3, &angularVelocityController3);

bool debug = false;

const float MAX_WHEEL_SPEED = 5.5; //m/s. Could be slightly higher
const int MIN_PRESSURE = 200;

int cannonTriggerPin = A0;
int pressureRegulatorPin = A1;
int pressureSensorPin = A2;
float desiredPressure = MIN_PRESSURE;

byte commandBuffer[64];
byte commandBufferIndex = 0;

unsigned long keepAliveTimer;
unsigned long keepAliveTimeout = 1000;

unsigned long angVelTimer;
unsigned long angVelTimeout = 50;

bool cannonTriggered = false;
unsigned long cannonTriggerTime;
unsigned long cannonTriggerTimeout = 1000;

byte magicBytes[] = {0x47, 0x41, 0x4e, 0x53};
byte commandLengthArray[] = {5, 5, 7, 9, 6, 5, 7, 9, 6};

void setup()
{
    Serial.begin(57600);
    Serial1.begin(250000);

    pinMode(cannonTriggerPin, OUTPUT);
    digitalWrite(cannonTriggerPin, LOW);

    pinMode(pressureRegulatorPin, OUTPUT);
    digitalWrite(pressureRegulatorPin, LOW);

    pinMode(pressureSensorPin, INPUT);
    digitalWrite(pressureSensorPin, LOW);

    pinMode(2, INPUT_PULLUP);
    digitalWrite(2, LOW);
    pinMode(4, INPUT_PULLUP);
    digitalWrite(4, LOW);

    pinMode(3, INPUT_PULLUP);
    digitalWrite(3, LOW);
    pinMode(8, INPUT_PULLUP);
    digitalWrite(8, LOW);

    pinMode(7, INPUT_PULLUP);
    digitalWrite(7, LOW);
    pinMode(11, INPUT_PULLUP);
    digitalWrite(11, LOW);

    pinMode(12, INPUT_PULLUP);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    attachInterrupt(encoder0.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt0, RISING);
    attachInterrupt(encoder1.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt1, RISING);
    attachInterrupt(encoder2.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt2, RISING);
    attachPinChangeInterrupt(encoder3.ReturnEncoderInterruptPinRef(), HandleEncoderPinAInterrupt3, RISING);

    esc0.attach(5);
    esc1.attach(6);
    esc2.attach(9);
    esc3.attach(10);

    keepAliveTimer = millis();
    angVelTimer = millis();
    cannonTriggerTime  = millis();

    kill_motors();
}

void loop()
{
    if(millis() - angVelTimer > angVelTimeout)
    {
        encoder0.MeasureAngularVelocity();
        encoder1.MeasureAngularVelocity();
        encoder2.MeasureAngularVelocity();
        encoder3.MeasureAngularVelocity();

        angVelTimer = millis();
    }

    if(cannonTriggered && millis() - cannonTriggerTime > cannonTriggerTimeout)
    {
        digitalWrite(cannonTriggerPin, LOW);
        cannonTriggered = false;
        Serial1 << "Trigger closed\n";
    }

    if(desiredPressure < analogRead(pressureSensorPin) && !cannonTriggered)
        digitalWrite(pressureRegulatorPin, HIGH);
    else
        digitalWrite(pressureRegulatorPin, LOW);

    if(millis() - keepAliveTimer > keepAliveTimeout)
        kill_robot();

    checkForCommand();
}

void kill_robot()
{
    kill_motors();
    desiredPressure = MIN_PRESSURE;
    digitalWrite(cannonTriggerPin, LOW);
    cannonTriggered = false;
}

void kill_motors()
{
    cmd_all_motors(0, 0, 0, 0);
}

void cmd_single_motor(int8_t motor, int8_t value)
{
    float command;

    if (motor == 1 || motor == 2)
        command = -value * (1000.0 / 255.0) + 1502.0;
    else
        command = value * (1000.0 / 255.0) + 1502.0;

    switch (motor)
    {
        case 0:
            wheel0.SetThrottle(command);
            break;

        case 1:
            wheel1.SetThrottle(command);
            break;

        case 2:
            wheel2.SetThrottle(command);
            break;

        case 3:
            wheel3.SetThrottle(command);
            break;

    }

    if(debug)
        Serial1 << "Command Single Motor " << motor << ": " << value << "\n";
}

void cmd_all_motors(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    float command1 = value1 * (1000.0 / 255.0) + 1502.0;
    float command2 = -value2 * (1000.0 / 255.0) + 1502.0;
    float command3 = -value3 * (1000.0 / 255.0) + 1502.0;
    float command4 = value4 * (1000.0 / 255.0) + 1502.0;

    wheel0.SetThrottle(command1);
    wheel1.SetThrottle(command2);
    wheel2.SetThrottle(command3);
    wheel3.SetThrottle(command4);

    if(debug)
        Serial1 << "Command All Motors: " << command1 << " " << command2 << " " << command3 << " " << command4 << "\n";
}

float print_motor_ang_vel(int8_t motor)
{
    switch(motor)
    {
        case 0:
            Serial1 << digitalRead(encoder0.ReturnEncoderPinA()) << "\t" <<
                digitalRead(encoder0.ReturnEncoderPinB()) << "\t" <<
                encoder0.ReturnEncoderTickCount() << "\t" <<
                encoder0.ReturnAngularVelocity() << "\n";
            break;

        case 1:
            Serial1 << digitalRead(encoder1.ReturnEncoderPinA()) << "\t" <<
                digitalRead(encoder1.ReturnEncoderPinB()) << "\t" <<
                encoder1.ReturnEncoderTickCount() << "\t" <<
                encoder1.ReturnAngularVelocity() << "\n";
            break;

        case 2:
            Serial1 << digitalRead(encoder2.ReturnEncoderPinA()) << "\t" <<
                digitalRead(encoder2.ReturnEncoderPinB()) << "\t" <<
                encoder2.ReturnEncoderTickCount() << "\t" <<
                encoder2.ReturnAngularVelocity() << "\n";
            break;

        case 3:
            Serial1 << digitalRead(encoder3.ReturnEncoderPinA()) << "\t" <<
                digitalRead(encoder3.ReturnEncoderPinB()) << "\t" <<
                encoder3.ReturnEncoderTickCount() << "\t" <<
                encoder3.ReturnAngularVelocity() << "\n";
            break;

    }
}

void fire_cannon()
{
    digitalWrite(cannonTriggerPin, HIGH);
    cannonTriggerTime = millis();
    cannonTriggered = true;

    if(debug)
        Serial1 << "Trigger Pulled\n";
}

void cmd_single_motor_ang_vel(int8_t motor, int8_t value)
{
    float command;

    if (motor == 1 || motor == 2)
        command = -(((float)value + 0.5)) * MAX_WHEEL_SPEED/127.5;
    else
        command = ((float)value + 0.5) * MAX_WHEEL_SPEED/127.5;

    switch (motor)
    {
        case 0:
            wheel0.ControlAngularVelocity(command);
            break;

        case 1:
            wheel1.ControlAngularVelocity(command);
            break;

        case 2:
            wheel2.ControlAngularVelocity(command);
            break;

        case 3:
            wheel3.ControlAngularVelocity(command);
            break;

    }

    if(debug)
        Serial1 << "Command Single Motor Vel" << motor << ": " << command << "\n";
}

void cmd_all_motors_ang_vel(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    float command0 = ((float)value1 + 0.5) * MAX_WHEEL_SPEED/127.5;
    float command1 = -(((float)value2 + 0.5)) * MAX_WHEEL_SPEED/127.5;
    float command2 = -(((float)value3 + 0.5)) * MAX_WHEEL_SPEED/127.5;
    float command3 = ((float)value4 + 0.5) * MAX_WHEEL_SPEED/127.5;

    wheel0.ControlAngularVelocity(command0);
    wheel1.ControlAngularVelocity(command1);
    wheel2.ControlAngularVelocity(command2);
    wheel3.ControlAngularVelocity(command3);

    if(debug)
        Serial1 << "Command All Motor Vels: " << command0 << " " << command1 <<
        " " << command2 << " " << command3 << "\n";
}

void set_desired_pressure(float _desiredPressure)
{
    desiredPressure = map(_desiredPressure, 0, 255, 0, 1023);

    if(desiredPressure > 1023)
        desiredPressure = 1023;

    if(desiredPressure < MIN_PRESSURE)
        desiredPressure = MIN_PRESSURE;

    if(debug)
        Serial1 << "Set Desired Pressure to " << desiredPressure << "\n";
}

void processCommand()
{
    switch (commandBuffer[4])
    {
        case 0:
            kill_robot();
            break;

        case 1:
            print_motor_ang_vel(2);
            break;

        case 2:
            cmd_single_motor(commandBuffer[5], commandBuffer[6]);
            break;

        case 3:
            cmd_all_motors(commandBuffer[5], commandBuffer[6], commandBuffer[7], commandBuffer[8]);
            break;

        case 4:
            print_motor_ang_vel(commandBuffer[5]);
            break;

        case 5:
            fire_cannon();
            break;

        case 6:
            cmd_single_motor_ang_vel(commandBuffer[5], commandBuffer[6]);
            break;

        case 7:
            cmd_all_motors_ang_vel(commandBuffer[5], commandBuffer[6], commandBuffer[7], commandBuffer[8]);
            break;

        case 8:
            set_desired_pressure(commandBuffer[5]);
            break;

        case 9:
            debug = true;
            break;

    }

      keepAliveTimer = millis();

}

void checkForCommand()
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
    encoder0.HandleEncoderPinAInterrupt();
}

void HandleEncoderPinAInterrupt1()
{
    encoder1.HandleEncoderPinAInterrupt();
}

void HandleEncoderPinAInterrupt2()
{
    encoder2.HandleEncoderPinAInterrupt();
}

void HandleEncoderPinAInterrupt3()
{
    encoder3.HandleEncoderPinAInterrupt();
}

