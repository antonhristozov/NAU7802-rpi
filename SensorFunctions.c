/*
 * Sensor Functions 
 */

/* include headers */
#include "NAU7802.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int init_sensor(){
	int fd;
	int z, gain=128;
	fd = wiringPiI2CSetup(NAU7802_ADDR);

	/* initialize NAU7802 */
	if((z = NAU7802_init(fd)) == 1)
		printf("Powerup Normal : %d\n", z);
	else
		printf("Powerup Fail : %d\n", z);
	delay(200);

	/* enable NAU7802 */
	if((z = NAU7802_enable(fd)) == 1)
		printf("Enabled : %d\n", z);
	else
		printf("Enable Failed : %d\n", z);
	delay(200);

	z = NAU7802_setGain(fd, gain);
	printf("Gain set to : %d\n", z);
	delay(200);


	z = NAU7802_AVDDSourceSelect(fd, AVDD_INT);
	printf("Source : %i\n", z);
	delay(200);

	z = NAU7802_setLDO(fd, V3_0);
	printf("Voltage : %i\n", z);
	delay(2000);

        return fd;
}

int calibrate_sensor(int fd){
	int z, i=0;
	do{
	   z = NAU7802_calibrate(fd, CALMOD_GCS);
	   printf("CAL_ERR : %i\n", z);
	   ++i;
	   delay(200);
	}while(z && i<10);
	delay(1000);
        return z;
}

