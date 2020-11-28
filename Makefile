CC= gcc
CFLAGS= -Wall -g -c
LIBS= -lwiringPi -lm
TARGETS= load test

top: load

NAU7802.o: NAU7802.c NAU7802.h
	$(CC) $(CFLAGS) NAU7802.c

NAU7802_driver.o: NAU7802_driver.c
	$(CC) $(CFLAGS) NAU7802_driver.c

load: NAU7802.o NAU7802_driver.o
	$(CC) NAU7802.o NAU7802_driver.o \
		$(LIBS) -o load

test.o: test.c
	$(CC) $(CFLAGS) test.c

test: test.o NAU7802.o
	$(CC) test.o NAU7802.o \
		$(LIBS) -o test

all: load test

clean:
	rm -f test.o NAU7802.o \
		NAU7802_driver.o \
		$(TARGETS)
