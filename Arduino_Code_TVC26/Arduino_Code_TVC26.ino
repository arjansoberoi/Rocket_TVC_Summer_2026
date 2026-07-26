#include "config.h"
#include "sensors.h"
#include "servo_control.h"
#include "PID.h"
#include "logger.h"
#include "flight_state.h"
#include "led.h"
#include "controller.h"

//create sensors object
Sensors sensors;

FlightState flightstate;

//constructors
ServoControl servos(PITCH_SERVO_PIN, YAW_SERVO_PIN, PAR1_SERVO_PIN, PAR2_SERVO_PIN);

PID pitchPID(PITCH_KP, PITCH_KI, PITCH_KD, -8, 8);

PID yawPID(YAW_KP, YAW_KI, YAW_KD, -8, 8);

Logger logger;

//another constructor
LEDs leds(IMU_LED_PIN, DATA_LED_PIN);

//main controller object constructor
Controller controller(sensors, servos, pitchPID, yawPID, flightstate, logger, leds);

void setup() {
  Serial.begin(115200); //115200 baud

  //only on setup
  controller.beginController();
}

void loop() {
  controller.updateController();
}

