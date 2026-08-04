#include "flight_state.h"
#include "config.h"

//construct with class member variables defaulted
FlightState::FlightState() {
    currentPhase = GROUND;
    prevAltitude = 0;
    launchTime = 0;
    deploy1 = false;
    deploy2 = false;
}

//start machine, this feels redundant but idk another way to go about this
void FlightState::beginMachine() {
    currentPhase = GROUND;
    prevAltitude = 0;
    launchTime = 0;
    deploy1 = false;
    deploy2 = false;
}

void FlightState::update(const SensorData &data) {
    //switch is basically a cleaner version of if else if else if else if
    switch(currentPhase) {
        case GROUND:
            if(detectLaunch(data)) {
            currentPhase = POWERED;
            launchTime = millis();  //start timer
            }
            break;  //next phase
        case POWERED:
            if(detectCoast(data)) {
                currentPhase = COAST;
            }
            break;  //next phase    
        case COAST:
            if(detectApogee(data)) {
                currentPhase = APOGEE;
            }
            break;  //next phase
        case APOGEE:
            if(!deploy1) {
                deploy1 = true;    //deploy at apogee
            }
            if(!deploy2) {
                deploy2 = true;    //deply at apogee
            }
            currentPhase = DESCENT;
            break;
        case DESCENT:
            if(detectLanding(data)) {
                currentPhase = LANDED;
            }
            break;  //next phase
        case LANDED:
            break;  //do nothing
    }
    prevAltitude = data.altitude;   //after updating, the loops altitude is now the prev altitude for next loop
}

//retrieve current flight phase directly from update()
FlightPhase FlightState::getState() {
    return currentPhase;
}

//parachute deployment functions, uses state of bools from update()
bool FlightState::shouldDeploy1() {
    return deploy1;
}

bool FlightState::shouldDeploy2() {
    return deploy2;
}

//private helper functions
bool FlightState::detectLaunch(const SensorData &data) {
    if(data.accelZ > LAUNCH_ACCEL_THRESH) {
        return true;
    }
    return false;
}

bool FlightState::detectCoast(const SensorData &data) {
    //or could use accel data
    if(millis() - launchTime > MOTOR_BURN_TIME) {
        return true;
    }
    return false;
}

bool FlightState::detectApogee(const SensorData &data) {
    //requires 3 consecutive loops where we are descending
    if(data.altitude < prevAltitude) {
        descendingCount++;  //increment everytime we detect descent
        if(descendingCount > APOGEE_CONFIRM_COUNT) {
            prevAltitude = data.altitude;
            return true;
        }
    } else {
        descendingCount = 0;    //reset only when we stop descending
    }
    prevAltitude = data.altitude;
    return false;
}

bool FlightState::detectLanding(const SensorData &data) {
    //my only idea on how to do this 
    if(millis() - launchTime > TOTAL_FLIGHT_TIME) {
        return true;
    }
    return false;
}

