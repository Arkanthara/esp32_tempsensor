#include "networkstorage.h"
#include "esp_log.h"
#include "string.h"

extern NetworkStorage networks[10];
extern int networks_number;

void init_networks(char * ssid, char * pwd)
{
	if (strlen(ssid) >= 32 || strlen(pwd) >= 64)
	{
		ESP_LOGE("Network Storage", "Error: network ssid of network pwd is too long !");
		return;
	}
	strcpy(networks[networks_number].ssid, ssid);
	strcpy(networks[networks_number].pwd, pwd);
	networks_number ++;
}

/* I see that in menu config, when nothing is given, we have only the '\0', so the size is equal to 1 for ''....*/
void create_networks_table()
{
	/* Here, we suppose that we can have a network with no password... */
	if (sizeof(CONFIG_NETWORK_0_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_0_SSID, CONFIG_NETWORK_0_PWD);
	}

	if (sizeof(CONFIG_NETWORK_1_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_1_SSID, CONFIG_NETWORK_1_PWD);
	}

	if (sizeof(CONFIG_NETWORK_2_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_2_SSID, CONFIG_NETWORK_2_PWD);
	}

	if (sizeof(CONFIG_NETWORK_3_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_3_SSID, CONFIG_NETWORK_3_PWD);
	}

	if (sizeof(CONFIG_NETWORK_4_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_4_SSID, CONFIG_NETWORK_4_PWD);
	}

	if (sizeof(CONFIG_NETWORK_5_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_5_SSID, CONFIG_NETWORK_5_PWD);
	}

	if (sizeof(CONFIG_NETWORK_6_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_6_SSID, CONFIG_NETWORK_6_PWD);
	}

	if (sizeof(CONFIG_NETWORK_7_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_7_SSID, CONFIG_NETWORK_7_PWD);
	}

	if (sizeof(CONFIG_NETWORK_8_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_8_SSID, CONFIG_NETWORK_8_PWD);
	}

	if (sizeof(CONFIG_NETWORK_9_SSID) != 1)
	{
		init_networks(CONFIG_NETWORK_9_SSID, CONFIG_NETWORK_9_PWD);
	}
}

