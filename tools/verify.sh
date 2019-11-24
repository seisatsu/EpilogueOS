#!/bin/bash
set -e

. tools/config.sh

usage() {
    cat >&2 <<EOF
$0 [-cgOvh]
Compile and link your Sketch
    -c  Compile only; don't link
    -g  Create debug symbols for compiled program
    -O  Optimize compiled program
    -v  Verbose output
    -h  Show this help
EOF
    exit 1
}

want_link=yes
while getopts cgOvh options; do
    case $options in
        c) want_link= ;;
        g) create_debug_symbols=yes ;;
        O) should_optimize=yes ;;

        v) verbose=yes ;;

        h) usage ;;
        *) usage ;;
    esac
done

bin=/bin
gcc=$bin/avr-gcc
gxx=$bin/avr-g++
ar=$bin/avr-gcc-ar
objcopy=$bin/avr-objcopy
size=$bin/avr-size

avr=$hardware/arduino/avr

# Arduino "core" source directories
core=$avr/cores/arduino
mega=$avr/variants/mega

# App source directories
eeprom=$avr/libraries/EEPROM/src
spi=$avr/libraries/SPI/src
freertos=$libraries/FreeRTOS/src
stl=$libraries/ArduinoSTL/src

if [ -n "$should_optimize" ]; then
    C_O="-Os -flto -ffunction-sections -fdata-sections -fno-fat-lto-objects"
    LD_O="-flto"
fi

if [ -n "$create_debug_symbols" ]; then
    DEBUG=-g
fi

if [ -n "$should_optimize" ] && [ -n "$create_debug_symbols" ]; then
    build_dir="$build_dir"/relwithdebinfo
elif [ -n "$should_optimize" ]; then
    build_dir="$build_dir"/release
elif [ -n "$create_debug_symbols" ]; then
    build_dir="$build_dir"/debug
else
    build_dir="$build_dir"/normal
fi

COMMON_FLAGS="-c -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR -I$core -I$mega"
CORE_ASMFLAGS="$COMMON_FLAGS -x assembler-with-cpp"
CORE_CFLAGS="$COMMON_FLAGS $C_O $DEBUG -w -std=gnu11"
CORE_CXXFLAGS="$COMMON_FLAGS $C_O $DEBUG -w -std=gnu++11 -fpermissive -fno-exceptions -fno-threadsafe-statics -Wno-error=narrowing -x c++"
APP_CXXFLAGS="$CORE_CXXFLAGS -I$spi -I$eeprom -I$freertos -I$stl"

LDFLAGS="$LD_O -fdiagnostics-color -w -fuse-linker-plugin -Wl,--gc-sections,--relax -mmcu=atmega2560 -L. -lm"

CORE_ASMS="wiring_pulse.S"
CORE_CS="WInterrupts.c wiring.c wiring_analog.c hooks.c wiring_digital.c wiring_pulse.c wiring_shift.c"
CORE_CXXS="HardwareSerial1.cpp CDC.cpp HardwareSerial0.cpp HardwareSerial.cpp HardwareSerial2.cpp HardwareSerial3.cpp IPAddress.cpp PluggableUSB.cpp Print.cpp Stream.cpp Tone.cpp USBCore.cpp WMath.cpp WString.cpp abi.cpp main.cpp new.cpp"

MALKUTH_CXXS="MalkuthOS.cpp LispLibrary.cpp"
SPI_CXXS="SPI.cpp"
FREERTOS_CXXS="croutine.c event_groups.c heap_3.c list.c port.c queue.c stream_buffer.c tasks.c timers.c variantHooks.cpp"
STL_CXXS="ArduinoSTL.cpp abi/abi.cpp algorithm.cpp associative_base.cpp bitset.cpp char_traits.cpp complex.cpp del_op.cpp del_opnt.cpp del_ops.cpp del_opv.cpp del_opvnt.cpp del_opvs.cpp deque.cpp eh_alloc.cpp eh_globals.cpp exception.cpp func_exception.cpp iomanip.cpp ios.cpp iostream.cpp istream.cpp iterator.cpp limits.cpp list.cpp locale.cpp map.cpp new_handler.cpp new_op.cpp new_opnt.cpp new_opv.cpp new_opvnt.cpp numeric.cpp ostream.cpp ostream_helpers.cpp queue.cpp set.cpp sstream.cpp stack.cpp stdexcept.cpp streambuf.cpp string.cpp support.cpp typeinfo.cpp utility.cpp valarray.cpp vector.cpp"
#STL_CXXS=""

e() {
    if [ -n "$verbose" ]; then
        echo "$@"
    fi
    "$@"
}

if [ ! -e $build_dir ]; then
    e mkdir -p $build_dir
fi
cd $build_dir

changed=
for f in $CORE_ASMS; do
    src=$core/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gcc $CORE_ASMFLAGS $src -o $dst
        changed=yes
    fi
    CORE_OBJS="$CORE_OBJS $dst"
done
for f in $CORE_CS; do
    src=$core/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gcc $CORE_CFLAGS $src -o $dst
        changed=yes
    fi
    CORE_OBJS="$CORE_OBJS $dst"
done
for f in $CORE_CXXS; do
    src=$core/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gxx $CORE_CXXFLAGS $src -o $dst
        changed=yes
    fi
    CORE_OBJS="$CORE_OBJS $dst"
done
if [ -n "$changed" ]; then
    e $ar rcs core.a $CORE_OBJS
fi

for f in $MALKUTH_CXXS; do
    src=$malkuth/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gxx $APP_CXXFLAGS $src -o $dst
        changed=yes
    fi
    APP_OBJS="$APP_OBJS $dst"
done
for f in $SPI_CXXS; do
    src=$spi/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gxx $APP_CXXFLAGS $src -o $dst
        changed=yes
    fi
    APP_OBJS="$APP_OBJS $dst"
done
for f in $FREERTOS_CXXS; do
    src=$freertos/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gxx $APP_CXXFLAGS $freertos/$f -o $dst
        changed=yes
    fi
    APP_OBJS="$APP_OBJS $dst"
done
for f in $STL_CXXS; do
    src=$stl/$f
    dst=$f.o
    dir=${f//\/*/}
    if [ $dst != $dir.o ] && [ ! -e $dir ]; then
        e mkdir -p $dir
    fi
    if [ ! -e $dst ] || [ $src -nt $dst ]; then
        e $gxx $APP_CXXFLAGS $stl/$f -o $dst
        changed=yes
    fi
    APP_OBJS="$APP_OBJS $dst"
done

OBJS="$APP_OBJS core.a"

BIN=MalkuthOS.cpp.elf
EEP=MalkuthOS/cpp.eep
HEX=MalkuthOS/cpp.hex

if [ -n "$want_link" ]; then
    if [ -n "$changed" ]; then
        should_link=yes
    fi

    if [ -z "$should_link" ]; then
        for f in $OBJS; do
            if [ $f -nt $BIN ]; then
                should_link=yes
                break
            fi
        done
    fi
fi

if [ -n "$should_link" ]; then
    if [ ! -e MalkuthOS ]; then
        e mkdir -p MalkuthOS
    fi

    e $gxx $LDFLAGS -o $BIN $OBJS

    e $objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $BIN $EEP

    e $objcopy -O ihex -R .eeprom $BIN $HEX
fi

awk '
function err(msg) {
    print "[31m" msg "[0m" > "/dev/stderr";
}
function parse_hex(str) {
    x = 0;
    for (i = 1; i <= length(str); i++) {
        c = substr(str, i, 1);
        x = x * 16 + hex[c];
    }
    return x;
}

BEGIN {
    for (i = 0; i < 16; i++) {
        hex[sprintf("%X", i)] = i;
    }

    eeprom_size = 248 * 1024;
    program_storage = 0;
}

{
    off = parse_hex(substr($0, 4, 4));
    data = (length() - 10) / 2;
    end = off + data - 1;
    if (end > program_storage) {
        program_storage = end;
    }
}

END {
    eeprom_percent = int(100 * program_storage / eeprom_size);

    # The value in program_storage ends up being how many bytes are flashed to
    # the EEPROM, but is for some reason not the number reported by Arduino
    # IDE'\'' Verify output.

    print "Sketch uses " program_storage " bytes (" eeprom_percent "%) of program storage space. Maximum is " eeprom_size " bytes.";

    if (program_storage >= eeprom_size) {
        err("Not enough memory; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing your footprint.");
        err("Error compiling for board Arduino/Genuino Mega or Mega 2560.");
        exit 1;
    }
}
' < $HEX

e $size -A $BIN | awk '
function warn(msg) {
    print "[33m" msg "[0m" > "/dev/stderr";
}
function err(msg) {
    print "[31m" msg "[0m" > "/dev/stderr";
}

BEGIN {
    ram_size = 8192;
    globals_storage = 0;
}

/(data|bss)/ {
    globals_storage += $2;
}

END {
    ram_percent = int(100 * globals_storage / ram_size);
    locals_storage = ram_size - globals_storage;

    print "Global variables use " globals_storage " bytes (" ram_percent "%) of dynamic memory, leaving " locals_storage " for local varibles. Maximum is " ram_size " bytes.";

    # Not sure at what levels this warning appears. Picking an arbitrary number
    # here.
    low_memory_threshold = 2 * 1024;
    size_error = 0;

    if (locals_storage <= 0) {
        err("Sketch too big; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing it.");
        err("Error compiling for board Arduino/Genuino Mega or Mega 2560.");
        exit 1;
    }

    if (locals_storage < low_memory_threshold) {
        warn("Low memory available, stability problems may occur.");
    }
}
'
