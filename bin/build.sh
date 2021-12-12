#!/bin/sh
# Tested with arduino-1.8.16 and arduino-esp32 2.0.1 on December 11th, 2021.

arduino=~/Source/arduino-1.8.16
arduino_ide_version=10816
dot_arduino=~/.arduino15
libraries=~/Arduino/libraries
malkuth=~/Source/MalkuthOS

build_path=/tmp/arduino_build
cache_path=/tmp/arduino_cache

esp32=$dot_arduino/packages/esp32

mkdir -p $build_path $cache_path

gcc_version=$(basename $esp32/tools/xtensa-esp32-elf-gcc/*) # Tested with gcc8_4_0-esp-2021r2
esptool_version=$(basename $esp32/tools/esptool_py/*) # Tested with 3.1.0
mkspiffs_version=$(basename $esp32/tools/mkspiffs/*) # Tested with 0.2.3
mklittlefs_version=$(basename $esp32/tools/mklittlefs/*) # Tested with 3.0.0-gnu12-dc7f933

$arduino/arduino-builder \
    -compile \
    -logger=machine \
    -hardware $arduino/hardware \
    -hardware $dot_arduino/packages \
    -tools $arduino/tools-builder \
    -tools $arduino/hardware/tools/avr \
    -tools $dot_arduino/packages \
    -built-in-libraries $arduino/libraries \
    -libraries $libraries \
    -fqbn=esp32:esp32:esp32:PSRAM=disabled,PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,LoopCore=1,EventsCore=1,DebugLevel=none \
    -vid-pid=10C4_EA60 \
    -ide-version=$arduino_ide_version \
    -build-path $build_path \
    -warnings=none \
    -build-cache $cache_path \
    -prefs=build.warn_data_percentage=75 \
    -prefs=runtime.tools.riscv32-esp-elf-gcc.path=$esp32/tools/riscv32-esp-elf-gcc/$gcc_version \
    -prefs=runtime.tools.riscv32-esp-elf-gcc-$gcc_version.path=$esp32/tools/riscv32-esp-elf-gcc/$gcc_version \
    -prefs=runtime.tools.xtensa-esp32-elf-gcc.path=$esp32/tools/xtensa-esp32-elf-gcc/$gcc_version \
    -prefs=runtime.tools.xtensa-esp32-elf-gcc-$gcc_version.path=$esp32/tools/xtensa-esp32-elf-gcc/$gcc_version \
    -prefs=runtime.tools.xtensa-esp32s2-elf-gcc.path=$esp32/tools/xtensa-esp32s2-elf-gcc/$gcc_version \
    -prefs=runtime.tools.xtensa-esp32s2-elf-gcc-$gcc_version.path=$esp32/tools/xtensa-esp32s2-elf-gcc/$gcc_version \
    -prefs=runtime.tools.esptool_py.path=$esp32/tools/esptool_py/$esptool_version \
    -prefs=runtime.tools.esptool_py-$esptool_version.path=$esp32/tools/esptool_py/$esptool_version \
    -prefs=runtime.tools.mkspiffs.path=$esp32/tools/mkspiffs/$mkspiffs_version \
    -prefs=runtime.tools.mkspiffs-$mkspiffs_version.path=$esp32/tools/mkspiffs/$mkspiffs_version \
    -prefs=runtime.tools.mklittlefs.path=$esp32/tools/mklittlefs/$mklittlefs_version \
    -prefs=runtime.tools.mklittlefs-$mklittlefs_version.path=$esp32/tools/mklittlefs/$mklittlefs_version \
    $malkuth/MalkuthOS.ino
