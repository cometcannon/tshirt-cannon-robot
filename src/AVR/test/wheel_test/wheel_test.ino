#include <Arduino.h>
#include <Servo.h>
#include "PID_controller.h"
#include "encoder.h"
#include "wheel.h"

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
    /*
    if(Serial1.available())
    {
        char c = (char)Serial1.read();
        Serial.write(c);
    }
    */
    delay(10);
}

/*
void calibrate()
{
    delay(2000);
    
    esc1.writeMicroseconds(1500);
    esc2.writeMicroseconds(1500);
    esc3.writeMicroseconds(1500);
    esc4.writeMicroseconds(1500);
    delay(233);
    
    esc1.writeMicroseconds(1000);
    esc2.writeMicroseconds(1000);
    esc3.writeMicroseconds(1000);
    esc4.writeMicroseconds(1000);
    delay(233);
    
    esc1.writeMicroseconds(2000);
    esc2.writeMicroseconds(2000);
    esc3.writeMicroseconds(2000);
    esc4.writeMicroseconds(2000);
    delay(233);
    
    esc1.writeMicroseconds(1500);
    esc2.writeMicroseconds(1500);
    esc3.writeMicroseconds(1500);
    esc4.writeMicroseconds(1500);
    delay(1000);
 
}*/

/*
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
    
}*/

/*
void escTest()
{
    esc.attach(6);
    
    esc.writeMicroseconds(1500);
    delay(2000);
    esc.writeMicroseconds(2000);
    delay(2000);
    esc.writeMicroseconds(1000);
    delay(2000);
    esc.writeMicroseconds(1500);
    delay(2000);
    
    esc.writeMicroseconds(1700);
}
*/

void getNextCommand()
{
    if(Serial1.available())
    {
        Serial1.readBytes(_buffer, Serial1.available());

        if(_buffer[0] == 2)
        {
           float command = ((int8_t)(_buffer[2]))*(1000.0/255.0) + 1502.0;

            switch(_buffer[1])
            {
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

                default:
                break;
            }
        }

        if(_buffer[0] == 1)
        {
            float command1 = ((int8_t)(_buffer[2]))*(1000.0/255.0) + 1502.0;
            float command2 = ((int8_t)(_buffer[2]))*(1000.0/255.0) + 1502.0;
            float command3 = ((int8_t)(_buffer[2]))*(1000.0/255.0) + 1502.0;
            float command4 = ((int8_t)(_buffer[2]))*(1000.0/255.0) + 1502.0;

            switch(_buffer[1])
            {
                case 0:
                esc1.writeMicroseconds(command1);
                break;

                case 1:
                esc2.writeMicroseconds(command2);
                break;

                case 2:
                esc3.writeMicroseconds(command3);
                break;

                case 3:
                esc4.writeMicroseconds(command4);
                break;

                default:
                break;
            }
        }


    }
}
