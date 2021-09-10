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


void read_test(int fd,int filed){
	const int SHIFT4 = 4;
	int adc_value;
	double load_value;
	struct load_cal lc;
	int i;
	NAU7802_init_load_cal(&lc);
	NAU7802_setLoadCalGain(&lc, 0.25);
	NAU7802_setShiftLoad(&lc, 0);
	NAU7802_calibrate(fd, CALMOD_GCS);
	NAU7802_getLinearLoad(fd, &lc);
	delay(5000);
	NAU7802_tareLoad(fd, &lc);
	for(i=0;i<10;i++){
		while(!NAU7802_CR(fd));
		/* Use 4 bit shift to smooth out noise */
		adc_value = NAU7802_readADCS(fd,SHIFT4); 
		printf("ADC : %i\n",adc_value) ;
	        load_value = NAU7802_getAvgLinearLoad(fd, &lc);
		write_to_file(filed,load_value);
		printf("Average Load : %+10.4f\n",load_value);
		delay(1000);
	}
}


int main(int argc, char **argv){
	int fd = -1;
	int filed = -1;
	printf("Initalize sensor \n");
	fd = init_sensor();
	printf("Calibrate sensor \n");
	calibrate_sensor(fd);
	printf("Wait for sensor to get ready \n");
	delay(500);
	filed = open_file("./test.tst");
	if(filed == -1){
	   printf("File open error\n");
	   exit(-1);
	}
	read_test(fd,filed);
	close_file(filed);
	return 0;
}
