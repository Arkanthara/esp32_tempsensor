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

//#if defined(CONFIG_NETWORK_0_SSID) && defined(CONFIG_NETWORK_0_PWD)
//init_networks((char *) CONFIG_NETWORK_0_SSID, (char *) CONFIG_NETWORK_0_PWD);
//#endif

// #if defined(CONFIG_NETWORK_1_SSID) && defined(CONFIG_NETWORK_1_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_1_SSID;
// networks[networks_number].pwd = (char *) CONFIG_NETWORK_1_PWD;
// networks_number ++;
// #else
// #endif
// 
// #if defined(CONFIG_NETWORK_2_SSID) && defined(CONFIG_NETWORK_2_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_2_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_2_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_3_SSID) && defined(CONFIG_NETWORK_3_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_3_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_3_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_4_SSID) && defined(CONFIG_NETWORK_4_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_4_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_4_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_5_SSID) && defined(CONFIG_NETWORK_5_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_5_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_5_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_6_SSID) && defined(CONFIG_NETWORK_6_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_6_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_6_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_7_SSID) && defined(CONFIG_NETWORK_7_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_7_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_7_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_8_SSID) && defined(CONFIG_NETWORK_8_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_8_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_8_PWD;
// networks_number ++;
// #endif
// 
// #if defined(CONFIG_NETWORK_9_SSID) && defined(CONFIG_NETWORK_9_PWD)
// networks[networks_number].ssid = CONFIG_NETWORK_9_SSID;
// networks[networks_number].pwd = CONFIG_NETWORK_9_PWD;
// networks_number ++;
// #endif

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
	create_nvs("Test");
	write_nvs("Test", "2", 19);
	ESP_LOGI("NVS Init", "Result for key %s: %d", "2", read_nvs("Test", "2"));

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
			char buffer[6];
			int buffer_len = 6;

			// Start temperature sensor
			start_temp_sensor();

			// Read temerature
			read_temp_sensor(&temp);

			// Format float to string
			int error = snprintf(buffer, buffer_len, "%f", temp);
			if (error < 1)
			{
				ESP_LOGE("Convert", "Failed to convert float to string");
				stop_temp_sensor();
				http_cleanup(client);
				disconnect_wifi(netif);
				return;
			}

			// Send temperature to server
			http_post(client, buffer, buffer_len);
			
			// Print temp_sensor
			printf("Temperature's sensor: %s\n", buffer);

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
