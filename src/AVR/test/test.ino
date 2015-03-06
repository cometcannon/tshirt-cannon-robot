#include <Arduino.h>
#include <Servo.h>
#include "wheel.h"
#include "PinChangeInt.h"

Servo esc0;
Servo esc1;
Servo esc2;
Servo esc3;

PIDController angularVelocityController0(10, 0, 0, 0, 500, -500);
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

bool ang_vel_switch0 = false;
bool ang_vel_switch1 = false;
bool ang_vel_switch2 = false;
bool ang_vel_switch3 = false;

float wheel_vel_setpoint0 = 0;
float wheel_vel_setpoint1 = 0;
float wheel_vel_setpoint2 = 0;
float wheel_vel_setpoint3 = 0;

const float MAX_WHEEL_SPEED = 9.0;

int cannonTriggerPin = A0;
int pressureRegulatorPin = A1;
int pressureSensorPin = A2;
float desiredPressure = 0;

byte commandBuffer[64];
byte commandBufferIndex = 0;

unsigned long keepAliveTimer;
unsigned long keepAliveTimeout = 1000;

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

    pinMode(2, INPUT);
    digitalWrite(2, LOW);
    pinMode(4, INPUT);
    digitalWrite(4, LOW);

    pinMode(3, INPUT);
    digitalWrite(3, LOW);
    pinMode(8, INPUT);
    digitalWrite(8, LOW);

    pinMode(7, INPUT);
    digitalWrite(7, LOW);
    pinMode(11, INPUT);
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

    kill_motors();
}

void loop()
{
    if(ang_vel_switch0)
        wheel0.ControlAngularVelocity(wheel_vel_setpoint0);

    if(ang_vel_switch1)
        wheel1.ControlAngularVelocity(wheel_vel_setpoint1);

    if(ang_vel_switch2)
        wheel2.ControlAngularVelocity(wheel_vel_setpoint2);

    if(ang_vel_switch3)
        wheel3.ControlAngularVelocity(wheel_vel_setpoint3);

    if(cannonTriggered && millis() - cannonTriggerTime > cannonTriggerTimeout)
    {
        digitalWrite(cannonTriggerPin, LOW);
        cannonTriggered = false;
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
    desiredPressure = 0;
    digitalWrite(cannonTriggerPin, LOW);
    cannonTriggered = false;
}

void kill_motors()
{
    float neutral = 1502.0;

    wheel0.SetThrottle(neutral);
    wheel1.SetThrottle(neutral);
    wheel2.SetThrottle(neutral);
    wheel3.SetThrottle(neutral);

    ang_vel_switch0 = false;
    ang_vel_switch1 = false;
    ang_vel_switch2 = false;
    ang_vel_switch3 = false;
}

void cmd_single_motor(int8_t motor, int8_t value)
{
    float command;

    if (motor == 1 || motor == 2)
        command = -((int8_t) value) * (1000.0 / 255.0) + 1502.0;
    else
        command = ((int8_t) value) * (1000.0 / 255.0) + 1502.0;

    switch (motor)
    {
        case 0:
            wheel0.SetThrottle(command);
            ang_vel_switch0 = false;
            break;
        case 1:
            wheel1.SetThrottle(command);
            ang_vel_switch1 = false;
            break;
        case 2:
            wheel2.SetThrottle(command);
            ang_vel_switch2 = false;
            break;
        case 3:
            wheel3.SetThrottle(command);
            ang_vel_switch3 = false;
            break;
    }
}

void cmd_all_motors(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    float command1 = ((int8_t) value1) * (1000.0 / 255.0) + 1502.0;
    float command2 = -((int8_t) value2) * (1000.0 / 255.0) + 1502.0;
    float command3 = -((int8_t) value3) * (1000.0 / 255.0) + 1502.0;
    float command4 = ((int8_t) value4) * (1000.0 / 255.0) + 1502.0;

    wheel0.SetThrottle(command2);
    wheel1.SetThrottle(command2);
    wheel2.SetThrottle(command2);
    wheel3.SetThrottle(command2);

    ang_vel_switch0 = false;
    ang_vel_switch1 = false;
    ang_vel_switch2 = false;
    ang_vel_switch3 = false;
}

float print_motor_ang_vel(int8_t motor)
{
    switch(motor)
    {
        case 0:
            Serial1.println(encoder0.ReturnAngularVelocity());
            break;
        case 1:
            Serial1.println(encoder1.ReturnAngularVelocity());
            break;
        case 2:
            Serial1.println(encoder2.ReturnAngularVelocity());
            break;
        case 3:
            Serial1.println(encoder3.ReturnAngularVelocity());
            break;
    }
}

void fire_cannon()
{
    digitalWrite(cannonTriggerPin, HIGH);
    cannonTriggerTime = millis();
    cannonTriggered = true;
}

void cmd_single_motor_ang_vel(int8_t motor, int8_t value)
{
    switch (motor)
    {
        case 0:
            wheel_vel_setpoint0 = (value + 0.5) * MAX_WHEEL_SPEED/127.5;
            ang_vel_switch0 = true;
            break;
        case 1:
            wheel_vel_setpoint1 = -(value + 0.5) * MAX_WHEEL_SPEED/127.5;
            ang_vel_switch1 = true;
            break;
        case 2:
            wheel_vel_setpoint2 = -(value + 0.5) * MAX_WHEEL_SPEED/127.5;
            ang_vel_switch2 = true;
            break;
        case 3:
            wheel_vel_setpoint3 = (value + 0.5) * MAX_WHEEL_SPEED/127.5;
            ang_vel_switch3 = true;
            break;
    }
}

void cmd_all_motors_ang_vel(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    wheel_vel_setpoint0 = (value1 + 0.5) * MAX_WHEEL_SPEED/127.5;
    wheel_vel_setpoint1 = -(value2 + 0.5) * MAX_WHEEL_SPEED/127.5;
    wheel_vel_setpoint2 = -(value3 + 0.5) * MAX_WHEEL_SPEED/127.5;
    wheel_vel_setpoint3 = (value4 + 0.5) * MAX_WHEEL_SPEED/127.5;

    ang_vel_switch0 = true;
    ang_vel_switch1 = true;
    ang_vel_switch2 = true;
    ang_vel_switch3 = true;
}

void set_desired_pressure(float _desiredPressure)
{
    desiredPressure = map(_desiredPressure, 0, 255, 0, 1023);

    if(desiredPressure > 1023)
        desiredPressure = 1023;
}

void processCommand()
{
    switch (commandBuffer[4])
    {
        case 0:
            kill_robot();
            break;
        case 1:
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

