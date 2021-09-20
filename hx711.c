/* AADL interface functions */
#include <stdio.h>
#include "NAU7802.h"
#include "SensorFunctions.h"


int hx711_initialize(void){
   int fd = -1;
   printf("Initializing sensor\n\n");
   fd = init_sensor();
   calibrate_sensor(fd);
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
   load_value = read_load(fd);
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
