/* AADL interface functions */
#include <stdio.h>
#include "NAU7802.h"
#include "SensorFunctions.h"

static struct load_cal lc;

int hx711_initialize(void){
   int fd = -1;
   printf("Initializing sensor\n\n");
   fd = init_sensor();
   calibrate_sensor(fd);
   NAU7802_init_load_cal(&lc);
   NAU7802_setLoadCalGain(&lc, 0.25);
   NAU7802_setShiftLoad(&lc, 0);
   NAU7802_calibrate(fd, CALMOD_GCS);
   NAU7802_getLinearLoad(fd, &lc);
   delay(500);
   NAU7802_tareLoad(fd, &lc);
   return fd;
}

double hx711_read_sensor_data(void){
   static int first_call = 0;
   static int fd = 0;
   double load_value = 0.0;
   if(first_call == 0){
	fd = hx711_initialize();
        first_call = 1;
   }
   while(!NAU7802_CR(fd));
   load_value = NAU7802_readADC(fd); 
   load_value = NAU7802_getLinearLoad(fd, &lc);
   load_value = convert_to_kilograms(load_value);
   /* Place value in shared memory */
   return load_value;
}

#define LEN  10

double hx711_process_sensor_data(double value){
   static double array[LEN] = {0.0};
   static unsigned int index = 0;
   static int count = 0;
   double sum = 0;
   int i;
   /* Read a value from shared memory and find average */
   array[index] = value;
   index ++;
   if(index >= LEN){
      index = 0;
   }
   if(count < LEN){
      count++;
   }
   sum = 0.0;
   for(i=0;i<count;i++){
      sum += array[i];
   }
   value = sum/count;
   /* Place resut value in shared memory */
   return value;
}

int hx711_log_sensor_data(double value){
   static int first_call = 0;
   int status = 0;
   static int fd = 0;
   /* Read value in shared memory */
   if(first_call == 0){
      fd = open_file("weight_sensor.log");
      first_call = 1;
   }
   if(status == 0){
     status = write_to_file(fd,value);
   }
   return status;
}
