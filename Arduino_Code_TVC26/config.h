#ifndef CONFIG_H
#define CONFIG_H

//servo pins
constexpr int PITCH_SERVO_PIN = 9;
constexpr int YAW_SERVO_PIN = 6;     
constexpr int PAR1_SERVO_PIN = 3;
constexpr int PAR2_SERVO_PIN = 5;

//servo max, min, center, open, and closed angles (subject to change)
constexpr int PITCH_CENTER = 80;
constexpr int PITCH_MIN = 60;   //figure out
constexpr int PITCH_MAX = 100;   //figure out
constexpr int YAW_CENTER = 115;
constexpr int YAW_MIN = 135;     //figure out
constexpr int YAW_MAX = 95;     //figure out
constexpr int PAR1_CLOSED = 90;
constexpr int PAR1_OPEN = 135;
constexpr int PAR2_CLOSED = 90;
constexpr int PAR2_OPEN = 135;

//SD card chip select pin
constexpr int SD_CS_PIN = 10;        

//status LED pins
constexpr int IMU_LED_PIN = 2;       
constexpr int  DATA_LED_PIN = 4;

//PID control frequency and timestep
constexpr int PID_FREQ = 100;
constexpr float DT = 0.01;

//PID constants (subject to change)
constexpr float PITCH_KP = 5.85;
constexpr float PITCH_KI = 2.87;
constexpr float PITCH_KD = 3.97;

//TODO(team): tune independently, mirrored from pitch as a starting point
constexpr float YAW_KP = 5.85;
constexpr float YAW_KI = 2.87;
constexpr float YAW_KD = 3.97;

//flight state machine constants (subject to change)
constexpr float LAUNCH_ACCEL_THRESH = 14.71;    //1.5g
constexpr float MOTOR_BURN_TIME = 3450.0; //milliseconds (3.5 seconds)
constexpr float TOTAL_FLIGHT_TIME = 10000.0; //milliseconds (15 seconds), TODO(team): confirm against real flight profile

#endif 
