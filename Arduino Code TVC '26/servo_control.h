#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H
#include <Servo.h>
#include <Arduino.h>

struct ServoConfig {
    int pitchCenterAngle;
    int pitchMinAngle;        //for pitch and yaw servos
    int pitchMaxAngle;
    int yawCenterAngle;
    int yawMinAngle;
    int yawMaxAngle;

    int closedPos1;
    int openPos1;       //for parachute servos
    int closedPos2;
    int openPos2;
};

class ServoControl {
private:
    Servo pitchServo;
    Servo yawServo;     //servo objects
    Servo par1Servo;
    Servo par2Servo;

    int pitchPin;
    int yawPin;     //digital pins
    int par1Pin;
    int par2Pin;

    ServoConfig config;     //configuration struct for servo angles

    float constrainPitchAngle(float angle);  //priv helper function to constrain pitch angles to min/max
    float constrainYawAngle(float angle);  //priv helper function to constrain pitch angles to min/max
public:
    //constructor
    ServoControl(int pitchPin, int yawPin, int par1Pin, int par2Pin);

    //intialize servos
    void begin();

    //for pitch control
    void setPitch(float command);

    //for yaw control
    void setYaw(float command);

    //deploy 1st parachute servo
    void deployPar1();

    //deploy 2nd parachute servo
    void deployPar2();

    void resetPar1();
                            //reset parachute servos to closed position
    void resetPar2();

    //center TVC servos to neutral position
    void centerTVC();

};

#endif