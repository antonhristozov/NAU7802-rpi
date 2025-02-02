/*
 * Sensor Functions 
 * Prototype Headers 
 */
int init_sensor(void);
int calibrate_sensor(int fd);
int read_adc(int fd);
double read_load(int fd);
double read_average_load(int fd);
double convert_to_kilograms(double value);
int open_file(const char *fname);
int close_file(int fd);
int write_to_file(int fd,double dv);
