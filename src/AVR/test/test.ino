#include <Arduino.h>
#include <Servo.h>
#include "wheel.h"

Servo esc0;
Servo esc1;
Servo esc2;
Servo esc3;

PIDController angularVelocityController0(10, 0, 0, 0, 0, 0);
PIDController angularVelocityController1(10, 0, 0, 0, 0, 0);
PIDController angularVelocityController2(10, 0, 0, 0, 0, 0);
PIDController angularVelocityController3(10, 0, 0, 0, 0, 0);

Encoder encoder0(2, 4, 0);
Encoder encoder1(3, 8, 1);
Encoder encoder2(7, 11, 4);
Encoder encoder3(12, 13, 0);

Wheel wheel0(5,  &esc0, &encoder0, &angularVelocityController0);
Wheel wheel1(6,  &esc1, &encoder1, &angularVelocityController1);
Wheel wheel2(9,  &esc2, &encoder2, &angularVelocityController2);
Wheel wheel3(10, &esc3, &encoder3, &angularVelocityController3);

byte _buffer[10];
//uint32_t MAGIC = 0x47414e53;

void setup()
{
    Serial1.begin(250000);
    Serial.begin(115200);

    esc0.attach(5);
    esc1.attach(6);
    esc2.attach(9);
    esc3.attach(10);
}

void loop()
{
    getNextCommand();
    delay(10);
}

void kill_motors()
{
    float neutral = 1502.0;

    wheel0.SetThrottle(neutral);
    wheel1.SetThrottle(neutral);
    wheel2.SetThrottle(neutral);
    wheel3.SetThrottle(neutral);
}

void cmd_single_motor(int8_t motor, int8_t value)
{
    float command;

    if (motor == 1 || motor == 2)
        command = -((int8_t) value) * (1000.0 / 255.0) + 1502.0;
    else
        command = ((int8_t) value) * (1000.0 / 255.0) + 1502.0;

    switch (motor) {
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
}

void getNextCommand()
{
    if(Serial1.available()) {
        Serial1.readBytes(_buffer, Serial1.available());
        
        if (_buffer[0] == 0x47 &&
            _buffer[1] == 0x41 &&
            _buffer[2] == 0x4e &&
            _buffer[3] == 0x53) {
            
            switch (_buffer[4]) {
                case 0:
                    kill_motors();
                    break;
                case 1:
                    cmd_single_motor(_buffer[5], _buffer[6]);
                    break;
                case 2:
                    cmd_all_motors(_buffer[5], _buffer[6], _buffer[7], _buffer[8]);
                    break;
            }
        }
    }
}
