# TVC_Bringup — Arduino IDE Setup Requirements

Everything needed to compile and upload `TVC_Bringup.ino`. Follow top to bottom.

## 0. Get the code
Clone or download this repo, then open the sketch:
- Repo: https://github.com/arjansoberoi/Rocket_TVC_Summer_2026
- Open **`TVC_Bringup/TVC_Bringup.ino`** in the Arduino IDE
  (`Config.h` opens automatically in a second tab — leave it next to the `.ino`).

## 1. Arduino IDE
- **Arduino IDE 2.x** (recommended) or 1.8.19+ — https://www.arduino.cc/en/software

## 2. Board support package
> **TODO(team): confirm the exact board.** This is the only step that depends on the model.

| Board | What to install | Where |
|-------|-----------------|-------|
| Arduino **Uno** | *Arduino AVR Boards* — already built into the IDE, nothing to install | — |
| Arduino **Nano Every** | *Arduino megaAVR Boards* | Tools ▸ Board ▸ Boards Manager, search **"megaAVR"**, Install |

Then set:
- **Tools ▸ Board ▸** your board
- **Tools ▸ Port ▸** the COM port that appears when the board is plugged in

## 3. Libraries (Tools ▸ Manage Libraries…)
Search each **exact name** and install:
- **Adafruit MPU6050**
- **Adafruit BME280 Library**
- **Adafruit Unified Sensor**

When the IDE asks *"Install missing dependencies?"* click **Install All** — that also
pulls in **Adafruit BusIO**, which the two sensor libraries need.

Built into the IDE (no install needed): **Wire, SPI, SD, Servo**.

## 4. SD card
- Format the microSD card as **FAT32** before inserting it.

## 5. Upload + view output
- Click **Upload** (→ arrow).
- Open **Serial Monitor** and set the baud rate (bottom-right) to **115200**.
- You should see the I2C scan, PASS/FAIL lines, the servo/LED tests, the summary
  block, then continuous live readings.

---

## Quick checklist
- [ ] Arduino IDE installed
- [ ] Board package for the actual board (Uno = built-in, Nano Every = megaAVR)
- [ ] Adafruit MPU6050
- [ ] Adafruit BME280 Library
- [ ] Adafruit Unified Sensor
- [ ] Adafruit BusIO (auto-installed with the above — click "Install All")
- [ ] microSD card formatted FAT32
- [ ] Serial Monitor set to 115200 baud
