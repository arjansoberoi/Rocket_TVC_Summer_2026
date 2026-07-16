#ifndef CONFIG_H
#define CONFIG_H

//servo pins
#define PITCH_SERVO_PIN 9
#define YAW_SERVO_PIN 6     
#define PAR1_SERVO_PIN 3
#define PAR2_SERVO_PIN 5

//SD card chip select pin
#define SD_CS_PIN 10        

//status LED pins
#define IMU_LED_PIN 2       
#define DATA_LED_PIN 4

//PID control frequency and timestep
#define PID_FREQ 100
#define DT 0.01

//servo orientations (subject to change)
#define SERVO_CENTER 90
#define SERVO_MIN 60
#define SERVO_MAX 120

//PID constants (subject to change)
#define PITCH_KP 0.5
#define PITCH_KI 0.2
#define PITCH_KD 0.1

#endif 
