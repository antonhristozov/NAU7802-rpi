/*
 * Test reading the 24 bit ADC output from the NAU7802
 * load amplifier.
 */

/* include headers */
#include "NAU7802.h"
#include "SensorFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


void read_test(int fd){
	/* read value from adc */
	for(;;){
		while(!NAU7802_CR(fd));
		printf("ADC : %i\n", NAU7802_readADC(fd));
		delay(1000);
	}
}


int
main(int argc, char **argv){
	int fd = -1;
	printf("Initalize sensor \n");
	fd = init_sensor();
	printf("Calibrate sensor \n");
	calibrate_sensor(fd);
	printf("Wait for sensor to get ready \n");
	delay(500);
	read_test(fd);
	return 0;
}
