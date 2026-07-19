#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LEDs {
private:
    int imuLEDPin;
    int sdLEDPin;
public:
    //constructor
    LEDs(int imuLEDPin, int sdLEDPin);

    //initialize dig pins
    void beginLEDs();

    //status indicators
    void indicateIMU(bool status);

    void indicateSD(bool status);
};

#endif