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
	const int SHIFT4 = 4;
	int adc_value;
	double load_value;
	struct load_cal lc;
	NAU7802_init_load_cal(&lc);
	NAU7802_setLoadCalGain(&lc, 0.25);
	NAU7802_setShiftLoad(&lc, 0);
	/* read value from adc */
	for(;;){
		while(!NAU7802_CR(fd));
		/* Use 4 bit shift to smooth out noise */
		adc_value = NAU7802_readADCS(fd,SHIFT4); 
		printf("ADC : %i\n",adc_value) ;
		delay(1000);
	        load_value = NAU7802_getLinearLoad(fd, &lc);
		printf("Load : %+10.4f\n",load_value);
		delay(1000);
	}
}


int main(int argc, char **argv){
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
