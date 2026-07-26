#include "led.h"
#include "config.h"

//constructor
LEDs::LEDs(int imuLEDPin, int sdLEDPin) {
    this->imuLEDPin = imuLEDPin;
    this->sdLEDPin = sdLEDPin;
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
    static unsigned long lastTime = 0;
    static bool ledState = false;
    unsigned long interval;
    if(status) {
        interval = 250;
    } else {
        interval = 1000;
    }
    if(millis() -  lastTime >= interval) {
        lastTime = millis();
        ledState = !ledState;
        digitalWrite(imuLEDPin, ledState);
    }
}

void LEDs::indicateSD(bool status) {
    static unsigned long lastTime = 0;
    static bool ledState = false;
    unsigned long interval;
    if(status) {
        interval = 250;
    } else {
        interval = 1000;
    }
    if(millis() - lastTime >= interval) {
        lastTime = millis();
        ledState = !ledState;
        digitalWrite(sdLEDPin, ledState);
    }
}