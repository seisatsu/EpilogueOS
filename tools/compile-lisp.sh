#!/bin/sh
set -e

usage() {
    cat >&2 <<EOF
$0 [-hv]
Create LispLibrary.cpp from lisp/*.lisp for inclusion in Arduino PROGMEM.
    -v  Verbose output
    -h  Show this help
EOF
    exit 1
}

while getopts hv options; do
    case $options in
        v) verbose=yes ;;

        h) usage ;;
        *) usage ;;
    esac
done

input=lisp/*.lisp
c=LispLibrary.cpp
h=LispLibrary.h

exec 3>&1  # Save handle to stdout.
e() {
    if [ -n "$verbose" ]; then
        echo "$@" >&3
    fi
    "$@"
}

e echo '// Warning: This is an automatically generated file.' > $h
e echo '//          Edits to this file may be lost.' >> $h
e echo '#ifndef LISP_LISP_H' >> $h
e echo '#define LISP_LISP_H' >> $h
e echo '' >> $h
e echo '#include <avr/pgmspace.h>' >> $h
e echo '' >> $h
e echo 'extern const char LispLibrary[] PROGMEM;' >> $h
e echo '#endif' >> $h

e echo '// Warning: This is an automatically generated file.' > $c
e echo '//          Edits to this file may be lost.' >> $c
e echo '#include "LispLibrary.h"' >> $c
e echo '' >> $c
e echo 'const char LispLibrary[] PROGMEM = ""' >> $c

# The extra space at the end of each line may help with separating uLisp
# functions from each other...
# TODO: Check if this is true.
e cat $input | e awk '{ print "\"" $0 " \""; }' >> $c

e echo ';' >> $c
