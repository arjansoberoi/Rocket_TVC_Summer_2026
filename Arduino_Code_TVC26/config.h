#ifndef CONFIG_H
#define CONFIG_H

//servo pins
constexpr int PITCH_SERVO_PIN = 9;
constexpr int YAW_SERVO_PIN = 6;     
constexpr int PAR1_SERVO_PIN = 3;
constexpr int PAR2_SERVO_PIN = 5;

//servo max, min, center, open, and closed angles (subject to change)
constexpr int PITCH_CENTER = 82;
constexpr int PITCH_MIN = 74;   //figure out
constexpr int PITCH_MAX = 90;   //figure out
constexpr int YAW_CENTER = 88;
constexpr int YAW_MIN = 80;     //figure out
constexpr int YAW_MAX = 91;     //figure out
constexpr int PAR1_CLOSED = 0;
constexpr int PAR1_OPEN = 90;
constexpr int PAR2_CLOSED = 0;
constexpr int PAR2_OPEN = 90;

//SD card chip select pin
constexpr int SD_CS_PIN = 10;        

//status LED pins
constexpr int IMU_LED_PIN = 2;       
constexpr int  DATA_LED_PIN = 4;

//PID control frequency and timestep
constexpr int PID_FREQ = 100;
constexpr float DT = 0.01;

//PID constants (subject to change)
constexpr float PITCH_KP = 0.5;
constexpr float PITCH_KI = 0.2;
constexpr float PITCH_KD = 0.1;

//TODO(team): tune independently, mirrored from pitch as a starting point
constexpr float YAW_KP = 0.5;
constexpr float YAW_KI = 0.2;
constexpr float YAW_KD = 0.1;

//flight state machine constants (subject to change)
constexpr float LAUNCH_ACCEL_THRESH = 24.5;    //2.5g
constexpr float MOTOR_BURN_TIME = 3450.0; //milliseconds (3.5 seconds)
constexpr float TOTAL_FLIGHT_TIME = 15000.0; //milliseconds (15 seconds), TODO(team): confirm against real flight profile

#endif 
