#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "list.h"
#include "connect.h"
#include "temperature.h"
#include "http.h"
#include "networkstorage.h"
#include <unistd.h>
#include "freertos/task.h"

#define TIME_PERIOD 5000
#define STACK_SIZE 4096

// Our task
TaskHandle_t Task_1 = NULL;
TaskHandle_t Task_2 = NULL;

// Our global variables
bool task_2 = false;
bool quit = false;

networks_number = 0;
NetworkStorage networks[10];

#if defined(CONFIG_NETWORK_0_SSID) && defined(CONFIG_NETWORK_0_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_0_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_0_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_1_SSID) && defined(CONFIG_NETWORK_1_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_1_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_1_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_2_SSID) && defined(CONFIG_NETWORK_2_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_2_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_2_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_3_SSID) && defined(CONFIG_NETWORK_3_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_3_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_3_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_4_SSID) && defined(CONFIG_NETWORK_4_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_4_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_4_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_5_SSID) && defined(CONFIG_NETWORK_5_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_5_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_5_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_6_SSID) && defined(CONFIG_NETWORK_6_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_6_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_6_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_7_SSID) && defined(CONFIG_NETWORK_7_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_7_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_7_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_8_SSID) && defined(CONFIG_NETWORK_8_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_8_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_8_PWD;
networks_number ++;
#endif

#if defined(CONFIG_NETWORK_9_SSID) && defined(CONFIG_NETWORK_9_PWD)
networks[networks_number].ssid = CONFIG_NETWORK_9_SSID;
networks[networks_number].pwd = CONFIG_NETWORK_9_PWD;
networks_number ++;
#endif


void vTask_1(void * parameter)
{

	// Initialize wifi and connect wifi
	esp_netif_t * netif = init_wifi();

	// Scan networks
	// scan_wifi();
	// scan_wifi(NULL, false);

	// Init http connection
	esp_http_client_handle_t client = http_init();

	// Initialize time
	// It's a variable that holds the time at which the task was last unblocked
	// The variable is automatically updated within vTaskDelayUntil().
	TickType_t time = xTaskGetTickCount();

	// Frequency
	const TickType_t freq = TIME_PERIOD / portTICK_PERIOD_MS;

	// Loop for send each five seconds the sensor's temperature
	while (1)
	{
		if (quit)
		{
			break;
		}
		else if (task_2)
		{
			vTaskResume(Task_2);
			vTaskSuspend(NULL);
		}
		else
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

			// Wait the time indicated by macro TIME_PERIOD
			vTaskDelayUntil(&time, freq);
		}
	}

	// Free resources of http
	http_cleanup(client);

	// Disconnect and free resources of wifi
	disconnect_wifi(netif);

	// Destroy list of networks
	list_destroy(head);

	// Destroy the task
	vTaskDelete(NULL);
}

void vTask_2(void * parameters)
{
	while (1)
	{
		if (quit)
		{
			break;
		}
		else if (!task_2)
		{
			vTaskSuspend(NULL);
		}
		else
		{
//			vTaskSuspend(Task_1);
			connect_wifi_no_init();
			task_2 = false;
			vTaskResume(Task_1);
			vTaskSuspend(NULL);
		}
	}
	vTaskDelete(NULL);
}

void app_main(void)
{
	// Initialize non volatile storage (nvs)
	int error = nvs_flash_init();
	if (error != ESP_OK)
	{
		ESP_LOGE("NVS Initialisation", "Failed to initialize non volatile storage");
		return;
	}

//	ESP_ERROR_CHECK(esp_http_client_set_header(client, "content-type", "text/plain"));

	error = xTaskCreate(vTask_2, "CONNECT", STACK_SIZE, NULL, tskIDLE_PRIORITY, &Task_2);
	configASSERT(Task_2);
	error = xTaskCreate(vTask_1, "LOOP", STACK_SIZE, NULL, tskIDLE_PRIORITY, &Task_1);
	configASSERT(Task_1);

}
