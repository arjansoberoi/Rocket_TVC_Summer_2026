#include "sensors.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//sensor objects
Adafruit_MPU6050 mpu;   
Adafruit_BME280 bme;

//sea level hPa for Trophy Club (should prob be global variable idk)
float seaLevelhPa = 1016.5;  

//constructor
Sensors::Sensors() 
: 
gyroOffsetX(0),
gyroOffsetY(0),
gyroOffsetZ(0),     //initialize class member variables
groundAltitude(0),
mpuOK(false),
bmeOK(false)
{

} 

void Sensors::begin() {
    //start I2C and sensors
    Wire.begin();
   
    //check mpu initialization
    if (!mpu.begin()) {
        Serial.println("MPU6050 initialization failed");
        mpuOK = false;
    } else {
            Serial.println("MPU6050 initialized successfully");
            mpuOK = true;
        }
    

    //check bme initialization
    if (!bme.begin(0x77)) {
        Serial.println("BME280 initialization failed");  
        bmeOK =  false;
     } else {
            Serial.println("BME280 initialized successfully");
            bmeOK = true;
        }
    

    //set mpu settings (subject to change, but prob fine)
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

}

void Sensors::calibrate() {
    Serial.println("Calibrating sensors, please dont touch rocket");
    
    //sample size for sensor calibration
    const int readings = 500;

    //intialie gyro and altitude sums
    float gyroSumX = 0;
    float gyroSumY = 0;
    float gyroSumZ = 0;
    float altitudeSum = 0;

    //sensor readings placeholders
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;

    for(int i = 0; i < readings; i++) {
        //read mpu
        mpu.getEvent(&accel, &gyro, &temp);  

        //sum gyro values
        gyroSumX += gyro.gyro.x;
        gyroSumY += gyro.gyro.y;
        gyroSumZ += gyro.gyro.z;

        //read bme, sum
        altitudeSum += bme.readAltitude(seaLevelhPa);
        delay(5);  //delay readings (subject to change) 
    }

    //calculate offsets and altitude average
    groundAltitude = altitudeSum / readings;
    gyroOffsetX = gyroSumX / readings;
    gyroOffsetY = gyroSumY / readings;
    gyroOffsetZ = gyroSumZ / readings;

    //print results
    Serial.println("Calibration complete");
    Serial.println("Ground Altitude: ");
    Serial.println(groundAltitude);
    Serial.println("Gyro Offsets: ");
    Serial.print("X: ");
    Serial.println(gyroOffsetX);
    Serial.print("Y: ");
    Serial.println(gyroOffsetY);
    Serial.print("Z: ");
    Serial.println(gyroOffsetZ);

}

SensorData Sensors::read() {
    //read mpu
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    mpu.getEvent(&accel, &gyro, &temp);

    //store mpu readings in currentData struct
    currentData.accelX = accel.acceleration.x;
    currentData.accelZ = accel.acceleration.y;
    currentData.accelY = accel.acceleration.z;
    currentData.gyroX = gyro.gyro.x - gyroOffsetX;  //apply offsets
    currentData.gyroY = gyro.gyro.y - gyroOffsetY;
    currentData.gyroZ = gyro.gyro.z - gyroOffsetZ;
    currentData.imuTemperature = temp.temperature;

    //read bme and store in currentData struct
    currentData.altitude = bme.readAltitude(seaLevelhPa) - groundAltitude;
    currentData.pressure = bme.readPressure();
    currentData.bmeTemperature = bme.readTemperature();
    currentData.humidity = bme.readHumidity();

    //store timestamp 
    currentData.timestamp = millis();

    return currentData;  //return struct with all sensor readings
}

//for LEDs
bool Sensors::getIMUStatus() {
    return mpuOK;
}




