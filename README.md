# BLE-Glow-Hockey

This is a PSOC implementation of the Glow Hockey game. The PSOC 6 BLE development board is used as the game server to run the core logic. The user interface runs on an Android device. The game will support both two player and single player modes.

More than that, this game can interact with the real world via sensors and a FPGA data processor.

## Hardwares

There are totally 7 hardwares in this electrical system.

+ 2 Sensors
    : A color sensor and a multi-sensor chip GY-80. We use the gravity and activity sensor on it.
+ Cyclone IV FPGA develop board(EP4CE115F29C7N)
    : A FPGA with many kinds of port on it. We use some GPIO ports and a RS-232 serial port.
+ PC or Laptop
    : Connecting the FPGA and PSoC, transmit data between them.
+ Cypress PSoC6 BLE Pioneer develop board
    : Use as the game server.
+ 2 Android smart phones
    : Use as the game client.

## Directories

+ BLEServer
    : Game server code on PSoC6. Mostly **C**.
+ BLEClient
    : Game client app code on Android. All **Java**.
+ SerialTransmitter
    : Transmit data between serial ports. Runs on PC. **Python**.
+ SensorDriver
    : Process the signals from sensors and send them to the serial port(RS-232). **C** and **Verilog**.

Still developing. Welcome issues.
