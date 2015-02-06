#include <Arduino.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"
#include "wheel.h"

#define MAGIC 0x47414e53

//PIDController angularVelocityController(10, 0, 0, 0, 0, 0);
//Encoder encoder(3, 4, 0);
Servo esc1;
Servo esc2;
Servo esc3;
Servo esc4;
//Wheel wheel(6, &esc1, &encoder, &angularVelocityController);
byte _buffer[10];

void setup()
{
    Serial1.begin(250000);
    Serial.begin(115200);

    esc1.attach(3);
    esc2.attach(5);
    esc3.attach(6);
    esc4.attach(9);
}

void loop()
{
    getNextCommand();
    delay(10);
}

void kill_motors()
{
    float neutral = 1502.0;

    esc1.writeMicroseconds(neutral);
    esc2.writeMicroseconds(neutral);
    esc3.writeMicroseconds(neutral);
    esc4.writeMicroseconds(neutral);
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
            esc1.writeMicroseconds(command);
            break;
        case 1:
            esc2.writeMicroseconds(command);
            break;
        case 2:
            esc3.writeMicroseconds(command);
            break;
        case 3:
            esc4.writeMicroseconds(command);
            break;
    }
}

void cmd_all_motors(int8_t value1, int8_t value2, int8_t value3, int8_t value4)
{
    float command1 = ((int8_t) value1) * (1000.0 / 255.0) + 1502.0;
    float command2 = -((int8_t) value2) * (1000.0 / 255.0) + 1502.0;
    float command3 = -((int8_t) value3) * (1000.0 / 255.0) + 1502.0;
    float command4 = ((int8_t) value4) * (1000.0 / 255.0) + 1502.0;

    esc1.writeMicroseconds(command1);
    esc2.writeMicroseconds(command2);
    esc3.writeMicroseconds(command3);
    esc4.writeMicroseconds(command4);
}

void getNextCommand()
{
    if(Serial1.available()) {
        Serial1.readBytes(_buffer, Serial1.available());
        
        if (_buffer[0] == MAGIC >> 24 & 0xff &&
            _buffer[1] == MAGIC >> 16 & 0xff &&
            _buffer[2] == MAGIC >> 8  & 0xff &&
            _buffer[3] == MAGIC >> 0  & 0xff   ) {

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
