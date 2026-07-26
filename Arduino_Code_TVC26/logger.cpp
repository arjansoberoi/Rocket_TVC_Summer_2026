#include "logger.h"

//constructor
Logger::Logger() {
    sdOK = false;
    filename = "flight.csv";
    writeCount = 0;
}

void Logger::writeCSVHeader() {
    //header, order must match writeData()
    file.println("AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, IMUTemp, Altitude, Pressure, BMETemp, Humidity, Time");
    file.flush();
}

bool Logger::initializeLogger() {
    //check SD Card Writer
    if(!SD.begin()) {
        Serial.println("SD Card Writer Failed");
        sdOK = false;
        return false;
    }
    if(SD.begin()) {
        Serial.println("SD Card Writer Intialized");
        sdOK = true;
        file = SD.open(filename, FILE_WRITE);   //open file
    }
    //file check
    if(!file) {
        Serial.println("Failed to open csv file");
        sdOK = false;
        return false;
    }
    writeCSVHeader();   //calls helper function
    return true;
}

void Logger::writeData(const SensorData &currentData) {
    file.print(currentData.accelX);
    file.print(", ");
    file.print(currentData.accelY);
    file.print(", ");
    file.print(currentData.accelZ);
    file.print(", ");
    file.print(currentData.gyroX);
    file.print(", ");
    file.print(currentData.gyroY);
    file.print(", ");
    file.print(currentData.gyroZ);
    file.print(", ");
    file.print(currentData.imuTemperature);
    file.print(", ");
    file.print(currentData.altitude);
    file.print(", ");
    file.print(currentData.pressure);
    file.print(", ");
    file.print(currentData.bmeTemperature);
    file.print(", ");
    file.print(currentData.humidity);
    file.print(", ");
    file.println(currentData.timestamp);   //terminates the row

    //flush periodically so data survives a sudden power loss instead of
    //sitting in the SD buffer until closeFile() (which may never be called)
    writeCount++;
    if(writeCount >= 10) {
        file.flush();
        writeCount = 0;
    }
}

void Logger::closeFile() {
    if(file){
        file.flush();   //flush before closing
        file.close();
    }
}

//for leds
bool Logger::getSDStatus() {
    return sdOK;
}