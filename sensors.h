#ifndef SENSORS_H
#define SENSORS_H

//sensor data container
struct SensorData {
    float accelX;
    float accelY;       
    float accelZ;
    float gyroX;        //from IMU
    float gyroY;
    float gyroZ;
    float imuTemperature;

    float altitude;   
    float pressure;  
    float bmeTemperature;  //from altimeter
    float humidity;

    unsigned long timestamp;  //milliseconds
};

class Sensors {
private:
    float groundAltitude;
    SensorData currentData;  //current sensor readings holder

    float gyroOffsetX;
    float gyroOffsetY;      //imu offsets (for calibration)
    float gyroOffsetZ;

    bool mpuOK;     //status bools
    bool bmeOK;
public:
    //constructor
    Sensors();

    //start I2C and sensors
    void begin();

    //preflight sensor calibration
    void calibrate();

    //read sensor data and store in currrentData struct
    void read():

};

#endif