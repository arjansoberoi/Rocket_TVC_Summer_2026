We (Sam Abiodun at Purdue, Griffin Cords at Cornell, James Mason at Michigan, Srimann Nambakkam at Texas, and Arjan Oberoi at SMU) started this project in the summer of 2026 with the goal of building and launching a rocket with thrust vector control (TVC), which adjusts the direction of the motor’s thrust to help control the rocket during flight. We worked on the onboard control software, sensor integration, servo control, and parachute deployment logic. One of our hardware modules fried before launch, pushing back our launch date and leaving flight testing unfinished.

The flight software is written in C++ for Arduino, with an Arduino Nano Every identified in our hardware configuration. We split the code into separate components for sensors, control calculations, servos, flight phases, data logging, and status LEDs, and a central controller brings them together. This structure let us work on individual parts while keeping the main program focused on starting the system and running the control loop.

The sensor code reads acceleration and rotation from an MPU6050 and uses a BME280 pressure sensor to estimate altitude relative to the launch point. At startup, it averages 500 readings to calculate gyroscope offsets and establish the ground altitude, giving later readings a reference. Separate proportional-integral-derivative (PID) controllers use the measured rotation rates to calculate pitch and yaw servo commands, with the goal of reducing rotation around those two axes. The servo code translates those commands into movement of the motor mount and includes configurable travel limits.

We also wrote a flight state machine, which tracks the rocket through ground, powered flight, coast, apogee, descent, and landing. It uses acceleration, elapsed time, and changes in altitude to decide when to move between phases, and it includes commands for two parachute deployment servos at apogee, the highest point of flight. The logging code records timestamped sensor readings to an SD card in CSV format, giving us a way to review what the sensors recorded during testing.

[Nosecone opening and closing](https://www.youtube.com/shorts/kx9xmp64Nqc)

[Gimbal moving](https://www.youtube.com/shorts/YIEfNCFN3Xc)

The [flight software](Arduino_Code_TVC26) captures our work on connecting sensor readings, control calculations, and hardware commands in one system. The launch remains postponed following the module failure, and the control settings and flight detection logic still need further testing before we can evaluate their performance in flight.
