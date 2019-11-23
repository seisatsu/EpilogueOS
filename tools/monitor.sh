. tools/config.sh

tput reset
rlwrap -I -apassword123 minicom -D $board
kill -9 %
