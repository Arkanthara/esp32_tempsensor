#include <string.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/semphr.h"
#include "networkstorage.h"

/* Current position in list of index in nvs */
int current = 0;

#define TIME_TO_WAIT pdMS_TO_TICKS(CONFIG_TIME_TO_WAIT)

/* Indicate if wifi is connected */
extern bool is_connected;
extern NetworkStorage networks[10];
extern int networks_number;
extern char mac[CONFIG_MAC_ADDR_SIZE * 3];

/* Function for connect wifi to a specific ssid */
void connect_wifi(int network)
{

	/* Create a config for wifi */
	wifi_config_t config = {};

	memset(config.sta.ssid, 0, sizeof(config.sta.ssid));
	memset(config.sta.password, 0, sizeof(config.sta.password));

	strcpy((char *)(config.sta.ssid), networks[network].ssid);
	strcpy((char *)(config.sta.password), networks[network].pwd);

	/* Set the configuration to wifi */
	int error = esp_wifi_set_config(WIFI_IF_STA, &config);	
	if (error != ESP_OK)
	{
		ESP_LOGE("Wifi Config", "Error: %s", esp_err_to_name(error));
		return;
	}

	/* connect to wifi */
	error  = esp_wifi_connect();
	if (error != ESP_OK)
	{
		ESP_LOGE("Wifi Connection", "Error: %s", esp_err_to_name(error));
		return;
	}
}

/* Function to look after disconnections */
void on_disconnection(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	/* We indicate to the main function that we aren't connected to the network */
 	is_connected = false;

	/* We indicate that we are disconnected */
 	ESP_LOGE("Connection Status", "Disconnected");

	/* If we've gone through the entire list of networks, we wait a moment before continuing */
	if (current == networks_number)
	{
		ESP_LOGI("Wifi Connection", "Wait a while before trying to reconnect....");
		vTaskDelay(TIME_TO_WAIT);
		current = 0;
	}

	/* We try to reconnect to network indicated by the current index in our list of networks */
	ESP_LOGI("Wifi Connection", "Trying to connect to: %s", networks[read_nvs("NetworkStorage", current)].ssid);
	connect_wifi(read_nvs("NetworkStorage", current));

	/* We increment the current index to connect to next network later if the connection didn't work */
	current ++;
}

void on_got_ip(void * event_handler_arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	/* We indicate that we have an ip address */
	ESP_LOGI("IP", "We have an ip address");

	/* We put the connected network to the begining of our 'list' in our nvs */
	pop_nvs("NetworkStorage", current - 1);

	/* We indicate to the main function that we are connected */
	is_connected = true;

	/* We load the mac address in global variable mac */
	uint8_t get_mac[CONFIG_MAC_ADDR_SIZE];
	ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,get_mac));
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", get_mac[0], get_mac[1], get_mac[2], get_mac[3], get_mac[4], get_mac[5]);
	ESP_LOGI("MAC", "Our MAC address is: %s", mac);
}

esp_netif_t * init_wifi(void)
{
	/* Initialize stack for tcp */
	ESP_ERROR_CHECK(esp_netif_init());


	/* Create event loop for handler's management. */
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	/* Create stack for wifi station... It initialize netif and register event handlers for default interfaces... */
	esp_netif_t * netif = esp_netif_create_default_wifi_sta();

	/* Attach handlers to an action. Is it necessary to create this handlers ??? */
	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, on_disconnection, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, NULL);
	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, on_disconnection, NULL);

	/* Initialize wifi */
	wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&init));

	/* Define that it's a wifi station */
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	/* Start wifi */
	ESP_ERROR_CHECK(esp_wifi_start());

	return netif;


}

/* Function for disallocate resouces */
void disconnect_wifi(esp_netif_t * netif)
{
	/* Unregistration of our handlers */
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, on_disconnection));
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_START, on_disconnection));

	/* Stop our wifi and disalocate resources */
	ESP_ERROR_CHECK(esp_wifi_stop());
	esp_netif_destroy_default_wifi(netif);
	ESP_ERROR_CHECK(esp_wifi_deinit());

	/* Destroy event loop */
	ESP_ERROR_CHECK(esp_event_loop_delete_default());
}
