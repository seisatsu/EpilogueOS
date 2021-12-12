# MalkuthOS
An ESP8266/ESP32 FreeRTOS-based mesh-networking operating system based on uLisp and FreeRTOS.
Currently in Pre-Alpha.

MalkuthOS will offer multiple shell sessions as separate FreeRTOS tasks, as well as user and filesystem management.

The end goal is a general purpose LoRa mesh networking solution.


## Scripts
These assume the user is running 64-bit Intel Linux, and that the Arduino IDE 1.8 and the Arduino ESP32 module v2.0 have been installed.

- `bin/compile.sh`: This file needs to be modified to suit your setup, but it will do a complete compile of Malkuth using the Arduino toolkit but without needing the Arduino IDE open.
- `bin/flash.sh`: Flashes the last compiled build onto an ESP32 on `/dev/ttyUSB0` without needing the Arduino IDE open. Potentially only needs [arduino-esp32](https://github.com/espressif/arduino-esp32) and does not need the Arduino IDE to run, but this is untested.
- `bin/serial.sh`: Opens a serial port terminal on `/dev/ttyUSB0` without needing the Arduino IDE open. Can optionally use either pyserial or GNU screen.
- `bin/test.py`: A unit test suite. Requires the ESP32 to be on `/dev/ttyUSB0`.
