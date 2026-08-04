#ifndef CONFIG_H
#define CONFIG_H

// =====================================================================
// Config.h - every tunable value for the TVC bring-up sketch lives here.
// Change a pin / address / test angle here, never in the .ino.
// =====================================================================

// Confirmed board: Arduino Nano Every (megaAVR/ATmega4809, not classic
// AVR). Pin numbering below still matches Uno-style (I2C on A4/A5,
// hardware SPI on 11/12/13) - Nano Every keeps this pinout for
// shield/wiring compatibility. Board package in Boards Manager is
// "Arduino megaAVR Boards", not "Arduino AVR Boards".

// ---- Serial ----
#define SERIAL_BAUD 115200

// ---- I2C sensor addresses ----
#define MPU6050_ADDR 0x68   // AD0 -> GND (would be 0x69 if AD0 high)
#define BME280_ADDR  0x77   // SDO strapped high (0x76 is the alternate)

// ---- I2C clock speeds ----
#define I2C_CLOCK_DEFAULT 100000L   // safe start-up speed
#define I2C_CLOCK_FAST    400000L   // target speed for the flight firmware

// ---- Servo pins (PWM-capable digital pins) ----
// Power servos from their OWN rail, NOT the Arduino 5V pin; tie that rail's
// ground to the Arduino ground so the PWM signal shares a reference.
#define PIN_SERVO_PITCH      9   // TVC gimbal - pitch
#define PIN_SERVO_YAW        6   // TVC gimbal - yaw
#define PIN_SERVO_PARACHUTE1 3   // parachute deploy #1
#define PIN_SERVO_PARACHUTE2 5   // parachute deploy #2

// ---- Status LED pins (single-color, not RGB) ----
#define PIN_LED_IMU 2   // solid = IMU OK, blinking = IMU fault
#define PIN_LED_SD  4   // solid = SD OK,  blinking = SD fault

// ---- SD card (SPI) ----
#define PIN_SD_CS   10             // MOSI/MISO/SCK are the fixed HW SPI pins
#define SD_TEST_FILE "bringup.csv"  // file used for the write/read-back test (.csv opens in Excel)
#define SENSOR_LOG_FILE "senslog.csv" // continuous MPU6050/BME280 log written from loop()

// ---- Sensor config ----
#define SEA_LEVEL_HPA 1013.25f   // reference for the BME280 altitude calc

// ---- Servo test motion (degrees) ----
#define SERVO_CENTER_DEG          90   // nominal center / rest angle
#define SERVO_TEST_SWING_DEG      30   // +/- swing for the TVC servos
#define PARACHUTE_CLOSED_DEG      90   // parachute "closed" / rest angle
#define PARACHUTE_TEST_SWING_DEG  15   // small, safe swing for parachute servos
#define SERVO_STEP_DELAY_MS       15   // delay per degree so motion is visible

// ---- Misc test config ----
#define LED_TEST_BLINKS      3     // times each LED flashes during the LED test
#define LOOP_STREAM_DELAY_MS 500   // how often loop() prints live readings (ms)

#endif // CONFIG_H
