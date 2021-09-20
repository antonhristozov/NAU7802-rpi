/* AADL interface functions */

int hx711_initialize(void);
double hx711_read_sensor_data(void);
double hx711_process_sensor_data(double value);
int hx711_log_sensor_data(double value);
