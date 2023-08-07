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
