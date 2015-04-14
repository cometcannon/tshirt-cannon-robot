#include <Arduino.h>
#include "Streaming.h"

uint8_t encoder0PinA = 2;
uint8_t encoder1PinA = 3;
uint8_t encoder2PinA = 7;
uint8_t encoder3PinA = 12;

uint8_t encoder0PinB = 4;
uint8_t encoder1PinB = 8;
uint8_t encoder2PinB = 11;
uint8_t encoder3PinB = A4;

uint8_t encoder0IntRef = 0;
uint8_t encoder1IntRef = 1;
uint8_t encoder2IntRef = 4;
uint8_t encoder3IntRef = 12;

long encoder0Ticks = 0;
long encoder1Ticks = 0;
long encoder2Ticks = 0;
long encoder3Ticks = 0;

void setup()
{
    Serial.begin(57600);

    pinMode(encoder0PinA, INPUT);
    digitalWrite(encoder0PinA, LOW);
    pinMode(encoder0PinB, INPUT);
    digitalWrite(encoder0PinB, LOW);

    pinMode(encoder1PinA, INPUT);
    digitalWrite(encoder1PinA, LOW);
    pinMode(encoder1PinB, INPUT);
    digitalWrite(encoder1PinB, LOW);

    pinMode(encoder2PinA, INPUT);
    digitalWrite(encoder2PinA, LOW);
    pinMode(encoder2PinB, INPUT);
    digitalWrite(encoder2PinB, LOW);

    pinMode(encoder3PinA, INPUT);
    digitalWrite(encoder3PinA, LOW);
    pinMode(encoder3PinB, INPUT);
    digitalWrite(encoder3PinB, LOW);

    attachInterrupt(encoder0IntRef, HandleEncoderPinAInterrupt0, RISING);
    attachInterrupt(encoder1IntRef, HandleEncoderPinAInterrupt1, RISING);
    attachInterrupt(encoder2IntRef, HandleEncoderPinAInterrupt2, RISING);
    attachInterrupt(encoder3IntRef, HandleEncoderPinAInterrupt3, RISING);

}

void loop()
{
    PrintPinInfo(0);
    PrintPinInfo(2);
    delay(100);
}

void PrintPinInfo(uint8_t enc)
{
    switch(enc)
    {

        case 0:
            Serial << digitalRead(encoder0PinA) << "\t" << 
                digitalRead(encoder0PinB) << "\t" <<
                encoder0Ticks << "\n";
            break;

        case 1:
            Serial << digitalRead(encoder1PinA) << "\t" << 
                digitalRead(encoder1PinB) << "\t" <<
                encoder1Ticks << "\n";
            break;

        case 2:
            Serial << digitalRead(encoder2PinA) << "\t" << 
                digitalRead(encoder2PinB) << "\t" <<
                encoder2Ticks << "\n";
            break;

        case 3:
            Serial << digitalRead(encoder3PinA) << "\t" << 
                digitalRead(encoder3PinB) << "\t" <<
                encoder3Ticks << "\n";
            break;

    }
}

void PrintEncoderTicks()
{
    Serial << encoder0Ticks << "\t" << encoder1Ticks << "\t" << encoder2Ticks << "\t" <<
        encoder3Ticks << "\t" << "\n";
}


void HandleEncoderPinAInterrupt0()
{
    encoder0Ticks += digitalRead(encoder0PinB) ? -1 : 1;
}

void HandleEncoderPinAInterrupt1()
{
    encoder1Ticks += digitalRead(encoder1PinB) ? -1 : 1;
}

void HandleEncoderPinAInterrupt2()
{
    encoder2Ticks += digitalRead(encoder2PinB) ? -1 : 1;
}

void HandleEncoderPinAInterrupt3()
{
    encoder3Ticks += digitalRead(encoder3PinB) ? -1 : 1;
}

