/*
 * Sensor Functions 
 * Prototype Headers 
 */
int init_sensor();
int calibrate_sensor(int fd);
float convert_to_pounds(int value);
int open_file(const char *fname);
int close_file(int fd);
int write_to_file(int fd,double dv);
