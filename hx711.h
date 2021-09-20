/* AADL interface functions */

int hx711_initialize(void);
int hx711_read_sensor_data(void);
int hx711_process_sensor_data(void);
int hx711_log_sensor_data(double value);
