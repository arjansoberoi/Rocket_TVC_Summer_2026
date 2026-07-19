#include "servo_control.h"
#include <Servo.h>

//constructor
ServoControl::ServoControl(int pitchPin, int yawPin, int par1Pin, int par2Pin){
    this->pitchPin = pitchPin;
    this->yawPin = yawPin;      //deref pointers to assign pin numbers to the class member variables
    this->par1Pin = par1Pin;
    this->par2Pin = par2Pin;

    //initialize config struct for tvc angles
    config.pitchCenterAngle = PITCH_CENTER;
    config.pitchMinAngle = PITCH_MIN;
    config.pitchMaxAngle = PITCH_MAX; 
    config.yawCenterAngle = YAW_CENTER;
    config.yawMinAngle = YAW_MIN;
    config.yawMaxAngle = YAW_MAX;    

    //intialize config struct for parachute positions
    config.closedPos1 = PAR1_CLOSED;
    config.openPos1 = PAR1_OPEN;
    config.closedPos2 = PAR2_CLOSED;
    config.openPos2 = PAR2_OPEN;

}

//center tvc servos to neutral position (tbd)
void ServoControl::centerTVC() {
    pitchServo.write(config.pitchCenterAngle);
    yawServo.write(config.yawCenterAngle);
}

//close par servo 1
void ServoControl::resetPar1() {
    par1Servo.write(config.closedPos1);
}

//close par servo2
void ServoControl::resetPar2() {
    par2Servo.write(config.closedPos2);
}

void ServoControl::begin() {
    //assign servos to their pins
    pitchServo.attach(pitchPin);
    yawServo.attach(yawPin);  
    par1Servo.attach(par1Pin);
    par2Servo.attach(par2Pin);

    //close parachutes, center pitch and yaw
    resetPar1();
    resetPar2();
    centerTVC();
 
}

//limit range of pitch servo correction to the gimbals allowance
float ServoControl::constrainPitchAngle(float angle) {
    if(angle < config.pitchMinAngle) {
        return config.pitchMinAngle;
    }
    if(angle > config.pitchMaxAngle) {
        return config.pitchMaxAngle;
    }
    return angle;   //return original angle parameter if neither if state applies
}

//limit range of yaw servo correction to the gimbals allowance
float ServoControl::constrainYawAngle(float angle) {
    if(angle < config.yawMinAngle) {
        return config.yawMinAngle;
    }
    if(angle > config.yawMaxAngle) {
        return config.yawMaxAngle;
    }
    return angle;   //return original angle parameter if neither if state applies
}

//deploy parachute servo 1
void ServoControl::deployPar1() {
    par1Servo.write(config.openPos1);
}

//deploy parachute servo 2
void ServoControl::deployPar2() {
    par2Servo.write(config.openPos2);
}

//pitch control
void ServoControl::setPitch(float command) {
    float angle = config.pitchCenterAngle + command;    //center + correction = desired angle
    //doesnt allow servo to hit mechanical limimt of the gimbal
    angle = constrainPitchAngle(angle);
    pitchServo.write(angle);
}

//yaw control, same logic as pitch function
void ServoControl::setYaw(float command) {
    float angle = config.yawCenterAngle + command;
    angle = constrainYawAngle(angle);
    yawServo.write(angle);
}





