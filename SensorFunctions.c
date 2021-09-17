/*
 * Sensor Functions 
 */

/* include headers */
#include "NAU7802.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int init_sensor(){
	int fd;
	int z, gain=128;
	fd = wiringPiI2CSetup(NAU7802_ADDR);

	/* initialize NAU7802 */
	if((z = NAU7802_init(fd)) != 1){
		printf("Powerup Fail : %d\n", z);
	}
	delay(200);

	/* enable NAU7802 */
	if((z = NAU7802_enable(fd)) != 1){
		printf("Enable Failed : %d\n", z);
	}
	delay(200);

	z = NAU7802_setGain(fd, gain);
	//printf("Gain set to : %d\n", z);
	delay(200);


	z = NAU7802_AVDDSourceSelect(fd, AVDD_INT);
	//printf("Source : %i\n", z);
	delay(200);

	z = NAU7802_setLDO(fd, V3_0);
	//printf("Voltage : %i\n", z);
	delay(2000);

        return fd;
}

int calibrate_sensor(int fd){
	int z, i=0;
	do{
	   z = NAU7802_calibrate(fd, CALMOD_GCS);
	   if(z != 0){
	      printf("CAL_ERR : %i\n", z);
	   }
	   ++i;
	   delay(200);
	}while(z && i<10);
	delay(1000);
        return z;
}

int read_adc(int fd){
   const int SHIFT4 = 4;
   while(!NAU7802_CR(fd));
   /* Use 4 bit shift to smooth out noise */
   return NAU7802_readADCS(fd,SHIFT4); 
}

double read_load(int fd){
   struct load_cal lc;
   NAU7802_init_load_cal(&lc);
   NAU7802_setLoadCalGain(&lc, 0.25);
   NAU7802_setShiftLoad(&lc, 0);
   NAU7802_calibrate(fd, CALMOD_GCS);
   return NAU7802_getLinearLoad(fd, &lc);
}

double read_average_load(int fd){
   struct load_cal lc;
   NAU7802_init_load_cal(&lc);
   NAU7802_setLoadCalGain(&lc, 0.25);
   NAU7802_setShiftLoad(&lc, 0);
   NAU7802_calibrate(fd, CALMOD_GCS);
   NAU7802_getLinearLoad(fd, &lc);
   NAU7802_tareLoad(fd, &lc);
   return NAU7802_getAvgLinearLoad(fd, &lc);
}


double convert_to_kilograms(double value){
	float kilograms = 0.0;
	kilograms = value/2.2;
	return kilograms;
}

int open_file(const char *fname){
    return open(fname,O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
}

int close_file(int fd){
   return close(fd);
}


int write_to_file(int fd,double dv){
  
  if(fd){
      char line[80];
      sprintf(line,"%lf %lu,\n",dv,(unsigned long)time(NULL));
      printf(line);
      return write(fd,line,strlen(line));	
  }
  else{
    return 0;
  }
}

