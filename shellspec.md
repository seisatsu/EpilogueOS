MalkuthOS Shell Specification v1
================================

Process Management
------------------

Processes are identified by a number. The current shell process is always zero, with available numbers being assigned to new processes lowest first, and recycled when processes are killed.

Processes have the following attributes:
* State: Running or Paused.
* PID: The number identifying the process.
* Function: Can be a lisp function or REPL. (REPL is an interactive shell.) Includes function name and arguments, possibly truncated.

Global variables, parameters, and constants are erased upon reset or reboot. Environment variables however are saved between resets and reboots.

* (ps [process(es)...]) : If process or list of processes is given, list their information. Otherwise, list information for every process.
* (spawn [function/block] [args]) : Spawn the given function and args as a new process, or spawn a block of code as a new process. If no function/block or args are given, spawn a new REPL.
* (kill [process(es)...]) : If process or list of processes is given, kill them. Otherwise, kill the current process. If the current process is the shell process, also spawn a new one in the foreground.
* (priority process(es)... [newpriority]) : If newpriority is given, set it as the new priority of the given process or processes, and return it. Otherwise, return the current priority of the given process(es). If multiple, return a list.
* (fg process) : Move the given process into the foreground.
* (killall) : Spawn and foreground a new shell process and kill all running processes. Do not delete transient global variables, parameters, and constants.
* (reset) : Same as killall, but DOES delete transient global variables, parameters, and constants.
* (reboot) : Physically power-cycle the device. All transient global variables, parameters, and constants will be lost.
* Ctrl-Z : Pause the current foreground process and return to the shell process. If the current process is the shell process, spin it as a paused process and spawn a new shell process, foregrounding that.
* Ctrl-B : Move the current foreground process into the background without pausing it, then return to the shell process. If the current process is the shell process, spin it off as a background process and spawn a new shell process, foregrounding that.
* Ctrl-C : Kill the current foreground process. If the process is the shell, also spawn a new shell in the foreground.
* Ctrl-K : Shortcut for (killall).
* Ctrl-X Three Times : Shortcut for (reset).
* Ctrl-R Three Times : Shortcut for (reboot).

Filesystem Management
---------------------

Filesystem roots are /#0/ for EEPROM and /$0/ for SDCARD. Multiple of each will be supported later. Functions below take full or relative filename paths. EEPROM does not support directories, however. SDCARD has a fat32 filesystem.

Persistent environment variables are for device configuration, and last between reboots. They are stored in /#0/ (EEPROM) as filenames starting with %, their values contained within. The first time each is read after boot they are pulled into RAM, and read from RAM thereafter to decrease EEPROM access. When they are written, they are written to both RAM and EEPROM.

* (with-eeprom args) : Work with a file on the EEPROM without encryption.
* (with-sdcard args) : Work with a file on the SDCARD without encryption.
* (with-eeprom-crypto args) : Work with a file on the EEPROM with encryption. Filenames of encrypted files internally start with an invisible exclamation point.
* (with-sdcard-crypto args) : Work with a file on the SDCARD with encryption. Filenames of encrypted files internally start with an invisible exclamation point.
* (encrypted filename) : Return T if filename is an encrypted file, NIL otherwise.
* (ls [path]) : List the contents of the given or current path.
* (lsr [path]) : List the contents of the given or current path recursively.
* (cd [path]) : Change directory. Can take a full or relative path. If path is ommitted, change to configured default root directory.
* (pwd) : Return the current working directory.
* (env [variable] [value]) : List persistent environment variables stored in /#0/.ENV, or just return the value of the selected persistent environment variable. Or, if variable and value are both given, set the variable to the new value and return it.
* (cp [filename(s)...] newpath) : Copy the listed filename(s) to the new path, which may be a directory or a new filename inside a directory. Will NOT create the destination parent directory if it doesn't exist. Not recursive.
* (cpp [filename(s)...] newpath) : Like cp, but WILL create the destination parent directory if it doesn't exist.
* (cpr [filename(s)...] newpath) : Like cp, but IS recursive.
* (cppr [filename(s)...] newpath) : Like both cpp and cpr.
* (rm [filename(s)...]) : Delete the selected filenames. Not recursive.
* (rmr [filename(s)...]) : Like rm but recursive.
* (rmdir [dirname(s)...] : Delete the listed directory name(s). Directories must be empty, otherwise use rmr.
* (touch [filename(s)...]) : Create the selected filename(s). Will NOT create the destination parent directory if it doesn't exist.
* (touchp [filename(s)...]) : Like touch, but WILL create the destination parent directory if it doesn't exist.
* (mkdir [filename(s)...]) : Create the selected directory name(s). Will NOT create the destination parent directory if it doesn't exist.
* (mkdirp [filename(s)...]) : Like mkdir, but WILL create the destination parent directory if it doesn't exist.
