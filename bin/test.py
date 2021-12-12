#!/usr/bin/env python3

from serial import Serial
import unittest

PORT = '/dev/ttyUSB0'
BAUD = 9600

# We open a connection to the serial port once and use it for all tests.
# Here is our global variable for keeping a reference to the open connection.
serial = None

def open_serial():
    global serial
    serial = Serial(PORT, BAUD, timeout=1)

def close_serial():
    global serial
    serial.close()

def write(message):
    serial.write(bytes(message, 'utf-8'))

def readline():
    line = serial.readline()
    return str(line, encoding='utf-8').strip()

class TestLispCommands(unittest.TestCase):
    @classmethod
    def setUpClass(_cls):
        open_serial()

    @classmethod
    def tearDownClass(_cls):
        close_serial()

    def test_ps(self):
        write('(ps)')
        _cmd_echo = readline()
        result = readline()
        self.assertEqual(result, '((0 t "_SHELL_" "/"))')

if __name__ == '__main__':
    unittest.main()
