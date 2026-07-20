include "config.h"
#include "sensors.h"
#include "servo_control.h"
#include "PID.h"
#include "logger.h"
#include "flight_state.h"
#include "led.h"
#include "controller.h"

//create sensors object
Sensors sensors;

//intialize pins
ServoControl servos(PITCH_SERVO_PIN, YAW_SERVO_PIN, PAR1_SERVO_PIN, PAR2_SERVO_PIN);

PID pitchPID(PITCH_KP, PITCH_KI, PITCH_KD, -8, 8);

PID yawPID(YAW_KP, YAW_KI, YAW_KD, -8, 8);

Logger logger;

LEDs leds(IMU_LED_PIN, DATA_LED_PIN);

//main controller object
Controller controller(sensors, servos, pitchPID, yawPID, flightState, logger, leds);

void setup() {
  Serial.begin(115200);

  controller.beginController();
}

void loop() {
  controller.updateController();
}

