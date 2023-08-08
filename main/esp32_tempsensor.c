#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "connect.h"
#include "temperature.h"
#include "http.h"
#include "networkstorage.h"
#include <unistd.h>
#include "freertos/task.h"

#define TIME_PERIOD 5000
#define STACK_SIZE 4096

#define WAIT_TIME pdMS_TO_TICKS(CONFIG_WAIT_TIME_SEND)

// Our global variables
bool task_2 = false;
bool quit = false;
bool is_connected = false;

int networks_number = 0;
NetworkStorage networks[10];
char mac[CONFIG_MAC_ADDR_SIZE * 3];


void app_main(void)
{
	// Initialize non volatile storage (nvs)
	int error = nvs_flash_init();
	if (error != ESP_OK)
	{
		ESP_LOGE("NVS Initialisation", "Failed to initialize non volatile storage");
		return;
	}
	create_networks_table();
	ESP_LOGI("CONFIG", "ssid: %s", networks[0].ssid);
	ESP_LOGI("CONFIG", "Number of networks registered: %d", networks_number);
	create_nvs("NetworkStorage");
	ESP_LOGI("NVS Init", "Result for key %s: %d", "0", read_nvs("NetworkStorage", 0));

	// Initialize wifi and connect wifi
	esp_netif_t * netif = init_wifi();

	// Init http connection
	esp_http_client_handle_t client = http_init();

	// Loop for send each five seconds the sensor's temperature
	while (1)
	{
		if (is_connected)
		{
			// Initialize variables
			float temp;
			char buffer[4 * CONFIG_MAC_ADDR_SIZE + 2];
			int buffer_len = 4 * CONFIG_MAC_ADDR_SIZE + 2;

			// Start temperature sensor
			start_temp_sensor();

			// Read temerature
			read_temp_sensor(&temp);

			// Format float to string
			int error = snprintf(buffer, buffer_len, "\"%s;%.2f\"", mac, temp);
			if (error < 1)
			{
				ESP_LOGE("Convert", "Failed to convert float to string");
				stop_temp_sensor();
				http_cleanup(client);
				disconnect_wifi(netif);
				return;
			}

			ESP_LOGI("HTTP Data", "This is the datas send to the server: %s", buffer);

			// Send temperature to server... Attention: the server don't accept char * with '\0' at the end !!! That's why I made buffer_len - 1....
			http_post(client, buffer, buffer_len - 1);
			
			// Print temp_sensor
			printf("Temperature's sensor: %.2f\n", temp);

			// Stop sensor
			stop_temp_sensor();
		}
		ESP_LOGI("Wait", "We are waiting a moment...");
		vTaskDelay(WAIT_TIME);
	}

	// Free resources of http
	http_cleanup(client);

	// Disconnect and free resources of wifi
	disconnect_wifi(netif);

}
