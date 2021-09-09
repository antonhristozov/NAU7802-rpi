# NAU7802-rpi
C Library for NAU7802 on Raspberry PI

The NAU7802_driver.c is useful for testing functionality
of the NAU7802 and possibly some examples.

The test.c will read all 32 registers of the NAU7802 and
display them.

Use the compile.sh to compile load(NAU7802_driver.c)
and test.c or as an example.

Alternativele using the Makefile will give the same result:
make

Building individual targetscan happen with the following commands:
make load
make test

To remove the executablesi and intermediate object files use:
make clean

To execute just run the produced executables:
./test
./load number_of_test
