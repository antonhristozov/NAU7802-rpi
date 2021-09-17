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

int process_thread(void){
   int status = 0;
   /* Read array of data from shared memoryi and find aaverage */
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
