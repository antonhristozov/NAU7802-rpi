#!/bin/sh -x
echo "Creating executables:"
gcc -Wall -o load NAU7802_driver.c NAU7802.c -lwiringPi -lm
gcc -Wall -o test test.c NAU7802.c -lwiringPi
gcc -Wall -o TestSensorfunctions TestSensorFunctions.c SensorFunctions.c NAU7802.c -lwiringPi -lm

