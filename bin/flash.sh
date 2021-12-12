#!/bin/sh
# Tested with arduino-1.8.16 and arduino-esp32 2.0.1 on December 11th, 2021.

dot_arduino=~/.arduino15
esp32=$dot_arduino/packages/esp32

build_path=/tmp/arduino_build

esp32_version=$(basename $esp32/hardware/esp32/*) # Tested with 2.0.1
esptool_version=$(basename $esp32/tools/esptool_py/*) # Tested with 3.1.0

python3 \
    $esp32/tools/esptool_py/$esptool_version/esptool.py \
    --chip esp32 \
    --port /dev/ttyUSB0 \
    --baud 921600 \
    --before default_reset \
    --after hard_reset \
    write_flash \
    -z \
    --flash_mode dio \
    --flash_freq 80m \
    --flash_size 4MB \
    0xe000 \
    $esp32/hardware/esp32/$esp32_version/tools/partitions/boot_app0.bin \
    0x1000 \
    $build_path/MalkuthOS.ino.bootloader.bin \
    0x10000 \
    $build_path/MalkuthOS.ino.bin \
    0x8000 \
    $build_path/MalkuthOS.ino.partitions.bin
