#include "PID.h"

//constructor
PID::PID(float kp, float ki, float kd, float outputMin, float outputMax)
    :
    kp(kp),
    ki(ki),
    kd(kd),     //intialize class member variables
    integral(0),
    prevError(0),
    outputMin(outputMin),
    outputMax(outputMax)
    {

    }

float PID::calculate(float target, float current, float dt) {
    //PID algorithm (double check)
    float error;
    float derivative;
    float output;
    //so we dont divide by 0
    if(dt <= 0) {
        return 0;
    }
    error = target - current;
    integral += error * dt;

    //in case integral is too large
    if(integral > outputMax) {
        integral = outputMax;
    } 

    if(integral < outputMin) {
        integral = outputMin;
    }
    derivative = (error - prevError) / dt;
    output = (kp * error) + (ki * integral) + (kd * derivative);
    prevError = error;
    if(output > outputMax) {
        output = outputMax;
    }
    if(output < outputMin) {
        output = outputMin;
    }
    return output;  //return PID output
}

//reset integral and prevError for next PID iteration
void PID::reset() {
    integral = 0;
    prevError = 0;
}

