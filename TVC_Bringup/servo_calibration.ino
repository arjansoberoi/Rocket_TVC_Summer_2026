#include <Servo.h>

const int par1Pin = 3;
const int par2Pin = 5;

Servo par1;
Servo par2;

//adjust until satisfied
const int open1 = 135;
const int closed1 = 90;

const int open2 = 135;
const int closed2 = 90;

//how long to wait between opening and closing (milliseconds)
const int delayTime = 5000;

/*this code will write the servos to their respective closed positions, wait 5 seconds, then write to their open positions
play around with the open and closed angles until it looks good

remember: red -> power (left pin when board facing up)
orange -> signal (middle)
brown -> GND (right pin when board facing up)*/

void setup() {
  Serial.begin(115200);
  par1.attach(par1Pin);
  par2.attach(par2Pin);
}


void loop() {

par1.write(closed1);
par2.write(closed2);
delay(delayTime); 

par1.write(open1);
par2.write(open2);
delay(delayTime);
}