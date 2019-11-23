#!/bin/sh
set -e

. config.sh

usage() {
    cat >&2 <<EOF
$0 [-h]
Upload your latest optimized compiled program to your Mega 2560
    -h  Show this help.
EOF
    exit 1
}

while getopts h options; do
    case $options in
        h) usage ;;
    esac
done

bin=$hardware/tools/avr/bin
etc=$hardware/tools/avr/etc

avrdude=$bin/avrdude

# Assume we are uploading the last optimized-without-debug-symbols build.
build_dir="$build_dir"/release

if ! [ -e $build_dir ]; then
    echo "Could not find a Sketch to upload" >&2
    echo "To create one, run './verify.sh -O'" >&2
    exit 1
fi

cd $build_dir

e() {
    if [ -n "$verbose" ]; then
        echo "$@"
    fi
    "$@"
}

BIN=MalkuthOS.ino.elf
EEP=MalkuthOS/ino.eep
HEX=MalkuthOS/ino.hex

e $avrdude -C$etc/avrdude.conf -patmega2560 -cwiring -P$board -b115200 -D -Uflash:w:$HEX:i > /dev/null
