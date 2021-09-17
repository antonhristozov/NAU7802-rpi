/* AADL interface functions */
#include <stdio.h>
#include "SensorFunctions.h"

int read_thread(void){
   static int first_call = 0;
   int status = 0;
   static int fd = 0;
   double load_value = 0.0;
   if(first_call == 0){
	fd = init_sensor();
	printf("Calibrate sensor \n");
	calibrate_sensor(fd);
        first_call = 1;
   }
   load_value = read_load(fd);
   load_value = convert_to_kilograms(load_value);
   /* Place value in shared memory */
   return status;
}

#define LEN  10

int process_thread(void){
   int status = 0;
   static double array[LEN] = {0.0};
   static unsigned int index = 0;
   double value=0.0;
   double sum = 0;
   int i;
   /* Read a value from shared memory and find average */
   array[index] = value;
   index ++;
   if(index >= LEN){
      index = 0;
   }
   sum = 0.0;
   for(i=0;i<LEN;i++){
      sum += array[i];
   }
   value = sum/LEN;
   /* Place resut value in shared memory */
   return status;
}

int log_to_file_thread(double value){
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
