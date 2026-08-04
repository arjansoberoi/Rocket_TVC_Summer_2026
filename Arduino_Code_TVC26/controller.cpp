#include "controller.h"

//constructor, intialize class member variables
Controller::Controller(Sensors &sensorObj, ServoControl &servoObj, PID &pitchPIDObj, PID &yawPIDObj, FlightState &stateObj, Logger &loggerObj, LEDs &ledObj)
:
sensors(sensorObj),
servos(servoObj),
pitchPID(pitchPIDObj),
yawPID(yawPIDObj),
flightState(stateObj),
logger(loggerObj),
led(ledObj)
{
    prevTime = 0;
}

void Controller::beginController() {
    //start leds first
    led.beginLEDs();

    //start sensors
    sensors.begin();

    //check imu
    led.indicateIMU(sensors.getIMUStatus());

    //start sd card writer
    logger.initializeLogger();

    //check sd card writer
    led.indicateSD(logger.getSDStatus());

    //calibrate sensors
    sensors.calibrate();

    //initialize servos
    servos.begin();

    //start flight state machine
    flightState.beginMachine();

    //start timer
    prevTime = millis();

}

//this function is the brain/implementation of everything. needs to be triple checked
void Controller::updateController() {
    //calculate timestep
    unsigned long currentTime = millis();
    float dt = (currentTime - prevTime) / 1000.0;
    prevTime = currentTime;

    //read in sensor data
    currentData = sensors.read();

    //update flight phase
    flightState.update(currentData);

    FlightPhase phase = flightState.getState();

    //parachute deploys: checked every cycle (not gated on phase) since the
    //deploy flags can become true on a cycle where phase has already moved past APOGEE
    if(flightState.shouldDeploy1()) {
        servos.deployPar1();
    }
    if(flightState.shouldDeploy2()) {
        servos.deployPar2();
    }

    //TVC Control:
   switch(phase) {
    case POWERED:
    case COAST:
    {
        //new targets need to be the pitch and yaw servo centers?
        float pitchCommand = pitchPID.calculate(0, currentData.gyroY, dt);
        float yawCommand = yawPID.calculate(0, currentData.gyroZ, dt);
        servos.setPitch(pitchCommand);
        servos.setYaw(yawCommand);
        break;
    }
    case GROUND:
    case APOGEE:
    case DESCENT:
    case LANDED:
    default:
    break;   //do nothing essentially in terms of tvc control
    }
    logger.writeData(currentData);
}
