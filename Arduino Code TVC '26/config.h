#ifndef CONFIG_H
#define CONFIG_H

//servo pins
#define PITCH_SERVO_PIN 9
#define YAW_SERVO_PIN 6     
#define PAR1_SERVO_PIN 3
#define PAR2_SERVO_PIN 5

//servo max, min, center, open, and closed angles (subject to change)
#define PITCH_CENTER 90
#define PITCH_MIN 82
#define PITCH_MAX 98
#define YAW_CENTER 90
#define YAW_MIN 82
#define YAW_MAX 98
#define PAR1_CLOSED 0
#define PAR1_OPEN 90
#define PAR2_CLOSED 0
#define PAR2_OPEN 90

//SD card chip select pin
#define SD_CS_PIN 10        

//status LED pins
#define IMU_LED_PIN 2       
#define DATA_LED_PIN 4

//PID control frequency and timestep
#define PID_FREQ 100
#define DT 0.01

//PID constants (subject to change)
#define PITCH_KP 0.5
#define PITCH_KI 0.2
#define PITCH_KD 0.1

//TODO(team): tune independently, mirrored from pitch as a starting point
#define YAW_KP 0.5
#define YAW_KI 0.2
#define YAW_KD 0.1

//flight state machine constants (subject to change)
#define LAUNCH_ACCEL_THRESH 24.5    //2.5g
#define MOTOR_BURN_TIME 3500.0 //milliseconds (3.5 seconds)
#define TOTAL_FLIGHT_TIME 7000.0 //milliseconds (7 seconds), TODO(team): confirm against real flight profile

#endif 
