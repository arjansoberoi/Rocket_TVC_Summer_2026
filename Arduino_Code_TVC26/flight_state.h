#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

#include <Arduino.h>
#include "sensors.h"

//basically a struct of bools that all pertain to the same conditions
enum FlightPhase {
    GROUND, POWERED, COAST, APOGEE, DESCENT, LANDED
};

class FlightState {
private:
    FlightPhase currentPhase;   //enum declaration
     //for apogee detection
    uint8_t descendingCount = 0;
    //number of simultaneous cycles descending required to deploy parachutes
    static constexpr uint8_t APOGEE_CONFIRM_COUNT = 3;
    float prevAltitude;    

    //to ensure parachutes deployed only once
    bool deploy1;
    bool deploy2;

    //timer
    unsigned long launchTime;

    //private helper functions
    bool detectLaunch(const SensorData &data);
    bool detectCoast(const SensorData &data);
    bool detectApogee(const SensorData &data);
    bool detectLanding(const SensorData &data);
public:
    //constructor
    FlightState();

    //starts flight state machine
    void beginMachine();

    //refreshes data to check current flight state
    void update(const SensorData &data);

    //get current state
    FlightPhase getState();

    //when to deploy parachutes
    bool shouldDeploy1();
    bool shouldDeploy2();
};

#endif