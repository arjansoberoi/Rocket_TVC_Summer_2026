#ifndef PID_H
#define PID_H

class PID{
private:
    float kp;
    float ki;
    float kd;
    float integral;
    float prevError;
    float outputMin;
    float outputMax;
public:
    //contructor
    PID(float kp, float ki, float kd, float outputMin, float outputMax);

    //calculate PID output
    float calculate(float target, float current, float dt);

    void reset();

};

#endif
