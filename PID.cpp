#include "PID.h"

//constructor
PID::PID(float kp, float ki, float kd);
    :
    kp(kp),
    ki(ki),
    kd(kd),     //intialize class member variables
    integral(0),
    prevError(0)
    {

    }

float PID::calculate(float target, float current, float dt) {
    //PID algorithm (double check)
    float target, current, dt, error, derivative, output;
    error = target - current;
    integral += error * dt;
    derivative = (error - prevError) / dt;
    output = (kp * error) + (ki * integral) + (kd * derivative);

    return output;  //return PID output
}

//reset integral and prevError for next PID iteration
void PID::reset() {
    integral = 0;
    prevError = 0;
}

