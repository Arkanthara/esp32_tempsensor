#include <string.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/semphr.h"
#include "global.h"
#include "list.h"

// Indicate the current try for a reconnection
int current_try_for_reconnection = 0;

// Semaphore used for waiting an ip address
SemaphoreHandle_t semaphore = NULL;

// Indicate if wifi is starting
bool wifi_start = false;
extern bool is_connected;

// Function for connect wifi to a specific ssid
void connect_wifi(Item * item)
{

	// Create a config for wifi
	wifi_config_t config = {};

	memset(config.sta.ssid, 0, sizeof(config.sta.ssid));
	memset(config.sta.password, 0, sizeof(config.sta.password));
	strncpy((char *)(config.sta.ssid), item->data->ssid, item->data->ssid_len);
	strncpy((char *)(config.sta.password), item->data->password, item->data->password_len);

	// Set the configuration to wifi
	int error = esp_wifi_set_config(WIFI_IF_STA, &config);
	if (error == ESP_ERR_WIFI_PASSWORD)
	{
		// It don't detect wrong password ...
		ESP_LOGE("Wifi Config", "Wrong password");
		return;
	}
	else if (error != ESP_OK)
	{
		ESP_LOGE("Wifi Config", "Error: %s", esp_err_to_name(error));
		return;
	}

	// connect to wifi
	error  = esp_wifi_connect();
	if (error == ESP_ERR_WIFI_SSID)
	{
		ESP_LOGE("Wifi Connect", "Wrong ssid");
	}
	else if (error == ESP_ERR_WIFI_CONN)
	{
		ESP_LOGE("Wifi Connect", "Internal error has made crash the connection");
	}
	else if (error != ESP_OK)
	{
		ESP_LOGE("Wifi Connect", "Error when trying to connect to network: %s", esp_err_to_name(error));
	}

}


void connect_wifi_no_init(void)
{
	// ESP_ERROR_CHECK(esp_wifi_stop());

	// ESP_ERROR_CHECK(esp_wifi_start());

	item = head->head;

	connect_wifi(item);

	// Wait semaphore which indicate that we have ip address (add TickType_t cast ???)
	if (xSemaphoreTake(semaphore, CONFIG_WAIT_TIME / portTICK_PERIOD_MS) != pdTRUE)
	{
		ESP_LOGE("Semaphore", "Semaphore don't giving up");
	}
}


// Function for scan and print networks
Item * scan_wifi(Item * item, bool search)
{
	// We scan networks
	int error = esp_wifi_scan_start(NULL, true);
	if (error != ESP_OK && error != ESP_ERR_WIFI_NOT_STARTED)
	{
		ESP_LOGE("Wifi Scan", "Failed");
		return NULL;
	}
	else if (error == ESP_ERR_WIFI_NOT_STARTED)
	{
		ESP_LOGE("Wifi Scan", "Wifi stop, so we can't scan network");
		return NULL;
	}
	uint16_t num_wifi = CONFIG_SCAN_MAX;
	wifi_ap_record_t wifi[CONFIG_SCAN_MAX];
	uint16_t ap_found = 0;

	// We get result
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num_wifi, wifi));

	// We ask how many network are found
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_found));

	// If we just want to scan wifi, we print result
	if (!search)
	{
		// We print result
		ESP_LOGI("Wifi Scan", "Scan result");
		for (int i = 0; i < (int) ap_found && i < CONFIG_SCAN_MAX; i++)
		{
			ESP_LOGI("Wifi Scan", "SSID: %s, len: %d", (char *) wifi[i].ssid, strlen((char *) wifi[i].ssid));

		}
	}

	// Else we search by priority order if an item has a ssid like in scan wifi
	else
	{
		while (item != NULL)
		{
			for (int i = 0; i < (int) ap_found && i < CONFIG_SCAN_MAX; i++)
			{
				if (strcmp((char *) wifi[i].ssid, item->data->ssid) == 0)
				{
					ESP_LOGI("Wifi Scan", "SSID found: %s", item->data->ssid);
					return item;
				}
			}
			item = item->next;
		}
	}
	return NULL;

}
void on_connection(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	current_try_for_reconnection = 0;
}

void on_disconnection(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
 	is_connected = false;
 	ESP_LOGE("Connection Status", "Disconnected");
 	if (current_try_for_reconnection < CONFIG_TRY_RECONNECT)
 	{
 		if (current_try_for_reconnection == 0)
 		{
 			item = head->head;
 			item = scan_wifi(item, true);
 			if (item != NULL)
 			{
 				item_print(item);
 				current_try_for_reconnection ++;
 				connect_wifi(item);
 			}
 			else
 			{
 				ESP_LOGE("Wifi Scan", "We don't find a network available with a known password");
 				current_try_for_reconnection ++;
 			}
 		}
 		else if (item->next != NULL)
 		{
 			item = item->next;
 			item = scan_wifi(item, true);
 			if (item != NULL)
 			{
 				item_print(item);
 				current_try_for_reconnection ++;
 				connect_wifi(item);
 			}
 			else
 			{
 				ESP_LOGE("Wifi Scan", "Retrying connect to wifi...");
 				item = head->head;
 				item = scan_wifi(item, true);
 				if (item != NULL)
 				{
 					item_print(item);
 					current_try_for_reconnection ++;
 					connect_wifi(item);
 				}
 				else
 				{
 					ESP_LOGE("Wifi Scan", "We don't find a network available with a known password");
 					current_try_for_reconnection ++;
 				}
 			}
 		}
 		else
 		{
 			item = head->head;
 			item = scan_wifi(item, true);
 			if (item != NULL)
 			{
 				item_print(item);
 				current_try_for_reconnection ++;
 				connect_wifi(item);
 			}
 			else
 			{
 				ESP_LOGE("Wifi Scan", "We don't find a network available with a known password");
 				current_try_for_reconnection ++;
 			}
 		}
 	}
 	else
 	{
 		ESP_LOGE("Connection Status", "Connection failed");
 	}
}

void on_got_ip(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	// We give the semaphore up because we can use wifi now
	ESP_LOGI("IP", "We have an ip address");
	is_connected = true;
	uint8_t get_mac[CONFIG_MAC_ADDR_SIZE];
	ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,get_mac));
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", get_mac[0], get_mac[1], get_mac[2], get_mac[3], get_mac[4], get_mac[5]);
	// sscanf(get_mac, "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
	// strncpy(mac, (MACSTR, MAC2STR(get_mac)), CONFIG_MAC_ADDR_SIZE);
	//		ESP_LOGI("MAC", "Our mac address is: %02x:%02x:%02x:%02x:%02x:%02x",get_mac[0],get_mac[1],get_mac[2],get_mac[3],get_mac[4],get_mac[5]);
	ESP_LOGI("MAC", "Our MAC address is: %s", mac);
	if (xSemaphoreGive(semaphore) != pdTRUE)
	{
		ESP_LOGE("Semaphore", "We can't give semaphore up");
	}
}

esp_netif_t * init_wifi(void)
{
	// I had to modify the file heap_tlsf.c like described here: https://github.com/espressif/esp-idf/issues/9087

	// Create binary semaphore
	semaphore = xSemaphoreCreateBinary();
	if (semaphore == NULL)
	{
		ESP_LOGE("Semaphore", "Creation of semaphore failed");
		return NULL;
	}

	// Initialize stack for tcp
	ESP_ERROR_CHECK(esp_netif_init());


	// Create event loop for handler's management.
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Create stack for wifi station... It initialize netif and register event handlers for default interfaces...
	esp_netif_t * netif = esp_netif_create_default_wifi_sta();

	// Attach handlers to an action. Is it necessary to create this handlers ???
	esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, on_disconnection, NULL, NULL);
	esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, on_connection, NULL, NULL);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, NULL, NULL);

	// Initialize wifi
	wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&init));

	// Define that it's a wifi station
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// Replace item at the beginning of the list
	item = head->head;

	// Indicate that wifi is starting
	wifi_start = true;

	// Start wifi
	ESP_ERROR_CHECK(esp_wifi_start());

	// Try to connect to our network
	scan(item);
	esp_wifi_connect();

	// Wait semaphore which indicate that we have ip address (add TickType_t cast ???)
	if (xSemaphoreTake(semaphore, CONFIG_WAIT_TIME / portTICK_PERIOD_MS) != pdTRUE)
	{
		ESP_LOGE("Semaphore", "Semaphore don't giving up");
	}

	// Indicate that wifi isn't starting
	wifi_start = false;

	return netif;


}


// Function for disallocate resouces
void disconnect_wifi(esp_netif_t * netif)
{
	ESP_ERR_CHECK(esp_event_handler_instance_unregister(on_disconnection));
	ESP_ERR_CHECK(esp_event_handler_instance_unregister(on_connection));
	ESP_ERR_CHECK(esp_event_handler_instance_unregister(on_got_ip));

	ESP_ERROR_CHECK(esp_wifi_stop());
	esp_netif_destroy_default_wifi(netif);
	ESP_ERROR_CHECK(esp_wifi_deinit());

	// We destroy event loop at the end because if we destroy it before, we couldn't see messages status...
	ESP_ERROR_CHECK(esp_event_loop_delete_default());
}

/*
esp_netif_t * connect_wifi(void)
{
	// I had to modify the file heap_tlsf.c like described here: https://github.com/espressif/esp-idf/issues/9087

	list_add(head, "WIFI_Mobile", "428fdcf3d44d5e92a54d1ca5579d21416be03291895184d724abf652f24a");
	list_add(head, "Test", "JbAeJdA!");
	list_print(head);

	// Create binary semaphore
	semaphore = xSemaphoreCreateBinary();
	if (semaphore == NULL)
	{
		ESP_LOGE("Semaphore", "Creation of semaphore failed");
		return NULL;
	}

	// Initialize stack for tcp
	ESP_ERROR_CHECK(esp_netif_init());


	// Create event loop for handler's management.
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Create stack for wifi station... It initialize netif and register event handlers for default interfaces...
	esp_netif_t * netif = esp_netif_create_default_wifi_sta();

	// Attach handlers to an action. Is it necessary to create this handlers ???
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL, NULL);
	esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL, NULL);

	// Initialize wifi
	wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&init));

	// Define that it's a wifi station
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// Create a config for wifi
	wifi_config_t config = {
		.sta = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASSWORD,
		},
	};

	// Set the configuration to wifi
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));

	// Start wifi
	ESP_ERROR_CHECK(esp_wifi_start());

	// Wait semaphore which indicate that we have ip address (add TickType_t cast ???)
	if (xSemaphoreTake(semaphore, CONFIG_WAIT_TIME / portTICK_PERIOD_MS) != pdTRUE)
	{
		ESP_LOGE("Semaphore", "Semaphore don't giving up");
		return netif;
	}

	return netif;


}
*/

