/*
 * Test reading the 24 bit ADC output from the NAU7802
 * load amplifier.
 */

/* include headers */
#include "NAU7802.h"
#include "SensorFunctions.h"
#include "hx711.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>


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
	delay(500);
	NAU7802_tareLoad(fd, &lc);
	for(i=0;i<10;i++){
		while(!NAU7802_CR(fd));
		/* Use 4 bi shift to smooth out noise */
		adc_value = NAU7802_readADCS(fd,SHIFT4); 
		printf("ADC : %i\n",adc_value) ;
	        load_value = NAU7802_getLinearLoad(fd, &lc);
		printf("Load Value : %+10.4f\n",load_value);
		write_to_file(filed,load_value);
		delay(1000);
	}
}

void hx711_test(){
   int status;
   double value;
   int i;
   printf("hx711_test() starting \n");
   for(i=0;i<10;i++){
      printf("\n");
      value = hx711_read_sensor_data();
      printf("Value read:%+10.2f      ",value);
      value = hx711_process_sensor_data(value);
      printf("Average value:%+10.2f ",value);
      status = hx711_log_sensor_data(value);
      if(status < 0){
         printf("Error logging sensor data status : %d\n",status);
      }
      else{
	 printf("logged. \n");
      }
      sleep(1);
   }
}

int main(int argc, char **argv){
	
	/*int fd = -1;
	int filed = -1;
	fd = hx711_initialize();
	filed = open_file("./test.tst");
	if(filed == -1){
	   printf("File open error\n");
	   exit(-1);
	}
	read_test(fd,filed);
	close_file(filed);*/
	
	hx711_test();
	return 0;
}
