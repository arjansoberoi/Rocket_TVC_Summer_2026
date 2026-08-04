// =====================================================================
// TVC_Bringup.ino - hardware bring-up / diagnostic sketch
//
// Proves every connection on the TVC flight-computer PCB works BEFORE
// any flight logic is written: I2C bus + both sensors, SD read/write,
// the servos, and both status LEDs - all running together.
//   setup() runs the full diagnostic sequence once.
//   loop()  repeats the servo sweep, streams live sensor readings, and
//           appends each reading as a CSV row on the SD card.
//
// Install via Arduino Library Manager (Tools > Manage Libraries):
//   - Adafruit MPU6050
//   - Adafruit BME280 Library
//   - Adafruit Unified Sensor   (dependency of both Adafruit libs)
// Built-in (no install): Wire, SPI, SD, Servo
//
// Open the Serial Monitor at 115200 baud.
// =====================================================================

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_BME280.h>

#include "Config.h"

// ---- Hardware objects ----
Adafruit_MPU6050 mpu;
Adafruit_BME280  bme;
Servo servoPitch, servoYaw, servoPara1, servoPara2;

// ---- Subsystem results (set in setup(), used by the summary + loop()) ----
bool g_i2cPass = false;
bool g_mpuPass = false;
bool g_bmePass = false;
bool g_sdPass  = false;
bool g_logPass = false;   // true once senslog.csv is created with its header row

// ---------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

// Blink an LED a fixed number of times (blocking - fine in a diagnostic).
void blinkLed(int pin, int times, int onMs, int offMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH); delay(onMs);
    digitalWrite(pin, LOW);  delay(offMs);
  }
}

// True if a device ACKs at this I2C address.
bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

// ---------------------------------------------------------------------
// [1] I2C bus scan - verifies A4/A5 wiring for both sensors
// ---------------------------------------------------------------------
bool testI2C() {
  Serial.println();
  Serial.println("[1] I2C bus scan");

  // Full scan so we see everything on the bus, expected or not.
  int found = 0;
  for (uint8_t addr = 0x01; addr <= 0x7F; addr++) {
    if (i2cPresent(addr)) {
      Serial.print("    found device @ 0x");
      if (addr < 16) Serial.print('0');
      Serial.println(addr, HEX);
      found++;
    }
  }
  if (found == 0) Serial.println("    (nothing responded - check SDA/SCL/power)");

  // The two devices we specifically require.
  bool mpuFound = i2cPresent(MPU6050_ADDR);
  bool bmeFound = i2cPresent(BME280_ADDR);
  Serial.println(mpuFound ? "    MPU6050 @ 0x68 ....... FOUND"
                          : "    MPU6050 @ 0x68 ....... NOT FOUND (check SDA/SCL/power/AD0)");
  Serial.println(bmeFound ? "    BME280  @ 0x77 ....... FOUND"
                          : "    BME280  @ 0x77 ....... NOT FOUND (check SDA/SCL/power/SDO)");

  // Staying at the safe 100kHz default for bring-up - the rocket doesn't
  // need 400kHz here, and this removes one more possible I2C hang point.
  // (The flight firmware, a separate sketch, is unaffected by this.)

  return mpuFound && bmeFound;
}

// ---------------------------------------------------------------------
// [2] MPU6050 init - IMU LED solid = OK, error-blink = fault
// ---------------------------------------------------------------------
bool testMPU() {
  Serial.println();
  Serial.println("[2] MPU6050 (IMU) init");

  if (!mpu.begin(MPU6050_ADDR)) {
    Serial.println("    FAIL - not responding at 0x68.");
    Serial.println("           Check: AD0->GND, SDA/SCL wiring, power.");
    blinkLed(PIN_LED_IMU, 6, 80, 80);   // fast error blink
    digitalWrite(PIN_LED_IMU, LOW);
    return false;
  }

  // Raw reads are all we need to verify the sensor; sensible rocket ranges.
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("    PASS - MPU6050 alive (+/-8g, +/-500 deg/s).");
  digitalWrite(PIN_LED_IMU, HIGH);   // solid = IMU OK
  return true;
}

// ---------------------------------------------------------------------
// [3] BME280 init
// ---------------------------------------------------------------------
bool testBME() {
  Serial.println();
  Serial.println("[3] BME280 (pressure/temp/humidity) init");

  if (!bme.begin(BME280_ADDR)) {
    Serial.println("    FAIL - not responding at 0x77.");
    Serial.println("           Check: address 0x76 vs 0x77 (SDO strap), SDA/SCL, power.");
    Serial.println("           Confirm it's a BME280, not a BMP280.");
    return false;
  }

  Serial.println("    PASS - BME280 alive.");
  return true;
}

// ---------------------------------------------------------------------
// [4] SD card init + write/read-back - SD LED solid = OK, blink = fault
// ---------------------------------------------------------------------
bool testSD() {
  Serial.println();
  Serial.println("[4] SD card init + write/read-back");

  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("    FAIL - SD.begin() failed.");
    Serial.println("           Check: card inserted, FAT32, CS on D10, SPI wiring.");
    blinkLed(PIN_LED_SD, 6, 80, 80);
    digitalWrite(PIN_LED_SD, LOW);
    return false;
  }

  // Write one known line to a fresh file, then read it back and compare.
  String testLine = "bringup,";
  testLine += millis();
  testLine += ",OK";

  SD.remove(SD_TEST_FILE);   // start clean so the read-back is deterministic
  File w = SD.open(SD_TEST_FILE, FILE_WRITE);
  if (!w) {
    Serial.println("    FAIL - could not open file for writing.");
    blinkLed(PIN_LED_SD, 6, 80, 80);
    digitalWrite(PIN_LED_SD, LOW);
    return false;
  }
  w.println(testLine);
  w.println("Hello World");
  w.close();

  File r = SD.open(SD_TEST_FILE);
  if (!r) {
    Serial.println("    FAIL - could not re-open file for reading.");
    blinkLed(PIN_LED_SD, 6, 80, 80);
    digitalWrite(PIN_LED_SD, LOW);
    return false;
  }
  String readBack = r.readStringUntil('\n');
  readBack.trim();
  r.close();

  Serial.print("    wrote:     "); Serial.println(testLine);
  Serial.println("    also wrote: Hello World");
  Serial.print("    read back: "); Serial.println(readBack);

  if (readBack == testLine) {
    Serial.println("    PASS - write and read-back match.");
    digitalWrite(PIN_LED_SD, HIGH);   // solid = SD OK
    return true;
  }
  Serial.println("    FAIL - read-back did NOT match what was written.");
  blinkLed(PIN_LED_SD, 6, 80, 80);
  digitalWrite(PIN_LED_SD, LOW);
  return false;
}

// Write all four servos at once to a position along their own swing range:
// pos -1 = full negative swing, 0 = center, +1 = full positive swing.
void writeAllServosAt(float pos) {
  servoPitch.write(SERVO_CENTER_DEG     + pos * SERVO_TEST_SWING_DEG);
  servoYaw.write(SERVO_CENTER_DEG       + pos * SERVO_TEST_SWING_DEG);
  servoPara1.write(PARACHUTE_CLOSED_DEG + pos * PARACHUTE_TEST_SWING_DEG);
  servoPara2.write(PARACHUTE_CLOSED_DEG + pos * PARACHUTE_TEST_SWING_DEG);
}

// Move all four servos together from one normalized position to another, in
// lockstep, so they visibly turn at the same time instead of one at a time.
void sweepAllServosTo(float fromPos, float toPos) {
  int steps = SERVO_TEST_SWING_DEG;   // 1-degree resolution for the widest swing
  for (int i = 1; i <= steps; i++) {
    writeAllServosAt(fromPos + (toPos - fromPos) * ((float)i / steps));
    delay(SERVO_STEP_DELAY_MS);
  }
}

// ---------------------------------------------------------------------
// [5] Servo test (visual)
//
// All four servos are now wired: Pitch (D9), Yaw (D6), Parachute 1 (D3),
// Parachute 2 (D5). They move simultaneously - center -> +swing -> center
// -> -swing -> center - to confirm the code runs, the pins actually output
// PWM, and the power wiring is right.
//
// POWER: run servos from their OWN power rail, NOT the Arduino 5V pin
// (a servo's stall current can brown-out the board). That rail's ground
// MUST be tied to the Arduino ground, or the PWM signal has no shared
// reference and the servo won't move.
// ---------------------------------------------------------------------
void testServos() {
  Serial.println();
  Serial.println("[5] Servo test (all four servos, simultaneous)");
  Serial.println("    Power the servos from their own rail, NOT Arduino 5V (share grounds!).");
  Serial.println("    Testing Pitch (D9), Yaw (D6), Para1 (D3), Para2 (D5) together...");

  servoPitch.attach(PIN_SERVO_PITCH);
  servoYaw.attach(PIN_SERVO_YAW);
  servoPara1.attach(PIN_SERVO_PARACHUTE1);
  servoPara2.attach(PIN_SERVO_PARACHUTE2);

  writeAllServosAt(0);      delay(400);   // start centered
  sweepAllServosTo(0, 1);   delay(200);   // all swing positive together
  sweepAllServosTo(1, 0);   delay(200);   // back to center together
  sweepAllServosTo(0, -1);  delay(200);   // all swing negative together
  sweepAllServosTo(-1, 0);  delay(400);   // back to center together

  Serial.println("    Servos returned to center/closed.");
}

// ---------------------------------------------------------------------
// [6] Status LED test (visual) - confirm each LED is on the right pin
// ---------------------------------------------------------------------
void testLeds() {
  Serial.println();
  Serial.println("[6] Status LED test");

  Serial.print("    Blinking IMU LED on D"); Serial.print(PIN_LED_IMU);
  Serial.print(" - you should see it flash "); Serial.print(LED_TEST_BLINKS);
  Serial.println(" times.");
  blinkLed(PIN_LED_IMU, LED_TEST_BLINKS, 250, 250);

  Serial.print("    Blinking SD LED on D"); Serial.print(PIN_LED_SD);
  Serial.print(" - you should see it flash "); Serial.print(LED_TEST_BLINKS);
  Serial.println(" times.");
  blinkLed(PIN_LED_SD, LED_TEST_BLINKS, 250, 250);
}

// Solid LED = subsystem OK; slow blink = subsystem faulted. Called each loop.
void updateStatusLeds() {
  static bool phase = false;
  phase = !phase;
  digitalWrite(PIN_LED_IMU, g_mpuPass ? HIGH : (phase ? HIGH : LOW));
  digitalWrite(PIN_LED_SD,  g_sdPass  ? HIGH : (phase ? HIGH : LOW));
}

// =====================================================================
// setup() - run the full diagnostic sequence once
// =====================================================================
void setup() {
  Serial.begin(SERIAL_BAUD);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000) { ; }  // wait up to 3s for the monitor

  Serial.println();
  Serial.println("================================================");
  Serial.println(" TVC Flight Computer - Hardware Bring-Up");
  Serial.println("================================================");

  // LEDs start off.
  pinMode(PIN_LED_IMU, OUTPUT); digitalWrite(PIN_LED_IMU, LOW);
  pinMode(PIN_LED_SD,  OUTPUT); digitalWrite(PIN_LED_SD,  LOW);

  // Bring up I2C at the safe default speed first.
  Wire.begin();
  // A stalled I2C transaction (flaky sensor connection, bus left in a bad
  // state, etc.) can otherwise block forever - abort it after 25ms and
  // reset the bus so loop() (and the servo sweep) never freezes.
  // Board here is a Nano Every (megaAVR, not classic AVR) - requires
  // updating "Arduino megaAVR Boards" via Boards Manager to a version
  // that supports it. Commented out because it fails to compile as-is.
  // Wire.setWireTimeout(25000, true);
  Wire.setClock(I2C_CLOCK_DEFAULT);

  // Full suite - all subsystems active together.
  testLeds();
  g_i2cPass = testI2C();
  g_mpuPass = testMPU();   // sets IMU LED
  g_bmePass = testBME();
  g_sdPass  = testSD();    // sets SD LED
  // Servo test runs repeatedly from loop() (see below), not once here - that
  // way a servo plugged in after power-up still gets swept, instead of
  // missing a one-shot window that already ran during setup().

  // Start the sensor log fresh each power-up with a header row.
  Serial.println();
  Serial.println("[4b] Sensor log file init");
  if (!g_sdPass) {
    Serial.println("    SKIPPED - SD card test above already failed.");
  } else {
    SD.remove(SENSOR_LOG_FILE);
    File log = SD.open(SENSOR_LOG_FILE, FILE_WRITE);
    if (log) {
      log.println("millis_ms,accelX_mps2,accelY_mps2,accelZ_mps2,"
                   "gyroX_radps,gyroY_radps,gyroZ_radps,"
                   "tempC,pressureHPa,altitudeM,humidityPct");
      log.close();
      g_logPass = true;
      Serial.print("    PASS - "); Serial.print(SENSOR_LOG_FILE); Serial.println(" created with header row.");
    } else {
      Serial.print("    FAIL - could not open "); Serial.print(SENSOR_LOG_FILE); Serial.println(" for writing.");
    }
  }

  // Summary block.
  Serial.println();
  Serial.println("===== BRING-UP SUMMARY =====");
  Serial.print("I2C bus .............. "); Serial.println(g_i2cPass ? "PASS" : "FAIL");
  Serial.print("MPU6050 (0x68) ....... "); Serial.println(g_mpuPass ? "PASS" : "FAIL");
  Serial.print("BME280  (0x77) ....... "); Serial.println(g_bmePass ? "PASS" : "FAIL");
  Serial.print("SD card (CS D10) ..... "); Serial.println(g_sdPass ? "PASS" : "FAIL");
  Serial.print("Sensor log file ...... "); Serial.println(g_logPass ? "PASS" : "FAIL");
  Serial.println("Servos (D9/D6/D3/D5) . REPEATING in loop() (simultaneous)");
  Serial.println("LEDs (D2/D4) ......... TESTED (visual)");
  Serial.println("============================");
  Serial.println();
  Serial.println("Live readings follow - tilt/move the board to watch them change.");
}

// Append one CSV row of the current sensor readings to SENSOR_LOG_FILE.
void logSensorRow(const String &row) {
  if (!g_logPass) return;   // skip if the log file itself never got created
  File log = SD.open(SENSOR_LOG_FILE, FILE_WRITE);
  if (!log) return;
  log.println(row);
  log.close();
}

// =====================================================================
// loop() - repeat the servo sweep, stream live sensor readings, and log
//          each reading to the SD card
// =====================================================================
void loop() {
  // Repeats forever so a servo plugged in mid-run still gets exercised on
  // the next pass, instead of only sweeping once during setup().
  testServos();

  Serial.println();
  Serial.println("---- live readings ----");

  String logRow = String(millis());

  if (g_mpuPass) {
    Serial.println("  [reading MPU...]");   // hang-diagnosis marker
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    Serial.print("  Accel [m/s^2]  X: "); Serial.print(a.acceleration.x, 2);
    Serial.print("  Y: "); Serial.print(a.acceleration.y, 2);
    Serial.print("  Z: "); Serial.println(a.acceleration.z, 2);
    Serial.print("  Gyro  [rad/s]  X: "); Serial.print(g.gyro.x, 2);
    Serial.print("  Y: "); Serial.print(g.gyro.y, 2);
    Serial.print("  Z: "); Serial.println(g.gyro.z, 2);
    logRow += "," + String(a.acceleration.x, 2) + "," + String(a.acceleration.y, 2) + "," + String(a.acceleration.z, 2);
    logRow += "," + String(g.gyro.x, 2) + "," + String(g.gyro.y, 2) + "," + String(g.gyro.z, 2);
  } else {
    Serial.println("  MPU6050: not available (init failed)");
    logRow += ",NA,NA,NA,NA,NA,NA";
  }

  if (g_bmePass) {
    Serial.println("  [reading BME...]");   // hang-diagnosis marker
    float t   = bme.readTemperature();
    float p   = bme.readPressure() / 100.0f;
    float alt = bme.readAltitude(SEA_LEVEL_HPA);
    float h   = bme.readHumidity();
    Serial.print("  Temp: ");     Serial.print(t, 2);   Serial.print(" C");
    Serial.print("   Press: ");   Serial.print(p, 2);   Serial.print(" hPa");
    Serial.print("   Alt: ");     Serial.print(alt, 2); Serial.print(" m");
    Serial.print("   Humidity: ");Serial.print(h, 1);   Serial.println(" %");
    logRow += "," + String(t, 2) + "," + String(p, 2) + "," + String(alt, 2) + "," + String(h, 1);
  } else {
    Serial.println("  BME280: not available (init failed)");
    logRow += ",NA,NA,NA,NA";
  }

  Serial.println("  [writing SD log...]");   // hang-diagnosis marker
  logSensorRow(logRow);

  updateStatusLeds();   // keep faulted subsystems' LEDs blinking as a reminder
  delay(LOOP_STREAM_DELAY_MS);
}
