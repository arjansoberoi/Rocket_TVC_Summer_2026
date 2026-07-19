#include "led.h"
#include "config.h"

//constructor
LEDs::LEDs(int imuLEDPin, int sdLEDPin) {
    this->imuLEDPin = IMU_LED_PIN;    //intialize pins from config
    this->sdLEDPin = DATA_LED_PIN;
}

void LEDs::beginLEDs() {
    //connect LEDs
    pinMode(imuLEDPin, OUTPUT);
    pinMode(sdLEDPin, OUTPUT);

    //start off slow flashing
    digitalWrite(imuLEDPin, LOW);
    digitalWrite(sdLEDPin, LOW);
}

void LEDs::indicateIMU(bool status) {
    if(status) {
        //flash fast if working
        digitalWrite(imuLEDPin, HIGH);
    } else {
        //flash slow if not
        digitalWrite(imuLEDPin, LOW);
    }
}

void LEDs::indicateSD(bool status) {
    if(status) {
        //flash fast if working
        digitalWrite(sdLEDPin, HIGH);
    } else {
        //flash slow if not
        digitalWrite(sdLEDPin, LOW);
    }
}