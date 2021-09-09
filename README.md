# NAU7802-rpi
C Library for NAU7802 on Raspberry PI

The NAU7802_driver.c is useful for testing functionality
of the NAU7802 and possibly some examples.

The test.c will read all 32 registers of the NAU7802 and
display them.

TestSensorFunctions shows tyical use of the intermediate 
layer functions part of SensorFunctions.c.

Use the compile.sh to compile load(NAU7802_driver.c)
, test.c and TestSensorFunctions.c.

Alternatively using the Makefile will produce the same result:
make

Building individual targets can happen through the following commands:
make load
make test
make TestSensorFunctions

To remove the executables and intermediate object files use:
make clean

To execute just run one of the produced executables:
./test
./load number_of_test
./TestSensorFunctions
