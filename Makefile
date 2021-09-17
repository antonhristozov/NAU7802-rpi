CC= gcc
CFLAGS= -Wall -g -c
LIBS= -lwiringPi -lm
TARGETS= load test TestSensorFunctions

top: load test TestSensorFunctions

NAU7802.o: NAU7802.c NAU7802.h
	$(CC) $(CFLAGS) NAU7802.c

NAU7802_driver.o: NAU7802_driver.c
	$(CC) $(CFLAGS) NAU7802_driver.c

SensorFunctions.o: SensorFunctions.c
	$(CC) $(CFLAGS) SensorFunctions.c

hx711.o: hx711.c
		$(CC) $(CFLAGS) hx711.c

load: NAU7802.o NAU7802_driver.o
	$(CC) NAU7802.o NAU7802_driver.o \
		$(LIBS) -o load

TestSensorFunctions: NAU7802.o TestSensorFunctions.o SensorFunctions.o hx711.o
	$(CC) NAU7802.o SensorFunctions.o TestSensorFunctions.o hx711.o \
		$(LIBS) -o TestSensorFunctions

test.o: test.c
	$(CC) $(CFLAGS) test.c

test: test.o NAU7802.o
	$(CC) test.o NAU7802.o \
		$(LIBS) -o test

all: load test

clean:
	rm -f test.o NAU7802.o \
		NAU7802_driver.o \
		SensorFunctions.o \
		TestSensorFunctions.o \
		hx711.o \
		$(TARGETS)
