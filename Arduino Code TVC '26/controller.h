#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

//ties together the implementation files, needs access
#include "Sensors.h"
#include "servo_control.h"
#include "PID.h"
#include "flight_state.h"
#include "Logger.h"
#include "led.h"
#include "config.h"

class Controller{
private:
    //existing objects
    Sensors &sensors;
    ServoControl &servos;

    PID &pitchPID;
    PID &yawPID;

    FlightState &flightState;

    Logger &logger;

    LEDs &led;

    SensorData currentData;

    unsigned long prevTime;
public:
//constructor
Controller(Sensors &sensorObj, ServoControl &servoObj, PID &pitchPIDObj, PID &yawPIDObj, FlightState &stateObj, Logger &loggerObj, LEDs &ledObj);

//intialize rocket electronics
void beginController();

//iterate flight loop
void updateController();

};

#endif
