#include "driver/temp_sensor.h"


// Start the sensor
void start_temp_sensor(void)
{
	temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(temp_sensor_set_config(temp_sensor));
	ESP_ERROR_CHECK(temp_sensor_start());
}

// Read the temperature of the sensor
void read_temp_sensor(float * result)
{
	ESP_ERROR_CHECK(temp_sensor_read_celsius(result));
}

// Stop the sensor
void stop_temp_sensor(void)
{
	ESP_ERROR_CHECK(temp_sensor_stop());
}
