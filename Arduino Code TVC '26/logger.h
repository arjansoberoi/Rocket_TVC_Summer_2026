#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SD.h>

#include "sensors.h"     // for SensorData struct

class Logger {
private:
    File file;      //file placeholder
    String filename;    
    bool sdOK; 

    void writeCSVHeader();  //private helper function
public:
//constructor
Logger();
bool initializeLogger();
//pass by reference to avoid an expensive copy every cycle
void writeData(const SensorData &currentData);    //idk why SensorData is orange, possible problem. i included sensors.h tho
void closeFile();

//for leds
bool getSDStatus();

};

#endif