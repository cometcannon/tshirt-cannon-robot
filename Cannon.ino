/*
  Analog input, analog output, serial output

 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.

 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground

 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

// These constants won't change.  They're used to give names
// to the pins used:
const int charge = A0;  // Analog input pin that the potentiometer is attached to
const int fire = A1; // Analog output pin that the LED is attached to
const int pressure=A2;
const int w1=5;
const int w2=6;
const int w3=9;
const int w4=10;
const int buzz=11;

int i = 0;
int firePressure=300 ;
boolean on=false;
char in;
double sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(charge,OUTPUT);
  pinMode(fire, OUTPUT);
  pinMode(pressure, INPUT);
}

void loop() {
  sensorValue = (double)analogRead(pressure)/255.0*200;
  Serial.println(sensorValue);
  // read the analog in value:
  if(Serial.available())
  {
    in=Serial.read();
  }

  switch(in){
    case 'N' : on=true;Serial.println("SYSTEM ON");digitalWrite(charge,LOW);digitalWrite(fire,LOW); in='0'; break;
    case 'F' : if(on){digitalWrite(charge,LOW);digitalWrite(fire,HIGH);delay(500);digitalWrite(fire,LOW); Serial.println("FIRE!");in='0';} break;
    case 'C' : if(on){if(sensorValue<firePressure){digitalWrite(fire,LOW);digitalWrite(charge,HIGH);Serial.println("CHARGING!");}else{digitalWrite(fire,LOW);digitalWrite(charge,LOW);Serial.println("READY TO FIRE!");in='0';}}break;
    case 'O' : on=false; digitalWrite(charge,LOW);digitalWrite(fire,LOW); analogWrite(w1,0);analogWrite(w2,0);analogWrite(w3,0);analogWrite(w4,0);analogWrite(buzz,0);Serial.println("System Off"); i=0;in='0';break;
    case 'V' : i=(i+10)%(256*2);
                analogWrite(w1,i-256);
                Serial.println(i-256);
                 in='0';break;
    case 'W' : analogWrite(w2,255); in='0';break;
    case 'X' : analogWrite(w3,255);delay(100); in='0';break;
    case 'Y' : analogWrite(w4,255);delay(100); in='0';break;
    case 'b' : digitalWrite(buzz,HIGH);delay(10);digitalWrite(buzz,LOW);in='0';break;
    default : break;
    }  
}
