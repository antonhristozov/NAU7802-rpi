/*
 * This is a quick way to check the value in all
 * 32 registers of the NAU7802.
 */

#include "NAU7802.h"
#include <stdio.h>

int
main(int argc, char **argv){
	int fd, i;
	fd = wiringPiI2CSetup(NAU7802_ADDR);
	for(i=0; i<32; i++){
		printf("REG%X:%x ", i, wiringPiI2CReadReg8(fd, i));
		if(i==15)
			printf("\n");
	}
	printf("\n");
	return 0;
}
	
