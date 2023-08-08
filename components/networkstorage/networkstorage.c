#include "networkstorage.h"
#include "esp_log.h"
#include "string.h"
#include "nvs.h"

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

void create_nvs(char * name)
{
	nvs_handle_t myhandle;
	ESP_ERROR_CHECK(nvs_open(name, NVS_READWRITE, &myhandle));
	for (int i = 0; i < networks_number; i++)
	{
		char key[2];
		if (snprintf(key, 2, "%d", i) < 0)
		{
			ESP_LOGE("NVS Init", "Error when trying parse int to char *");
		}
		else
		{
			ESP_ERROR_CHECK(nvs_set_i8(myhandle, key, i));
			ESP_LOGI("NVS Init", "We write in nvs: %s: %d", key, i);
		}
	}
	ESP_ERROR_CHECK(nvs_commit(myhandle));
	nvs_close(myhandle);

}

int read_nvs(char * name, int key_given)
{
	int8_t value = -1;
	char key[2];
	if (snprintf(key, 2, "%d", key_given) < 0)
	{
		ESP_LOGE("NVS Read", "Can't convert int to char *");
	}
	else
	{
		nvs_handle_t myhandle;
		ESP_ERROR_CHECK(nvs_open(name, NVS_READONLY, &myhandle));
		ESP_ERROR_CHECK(nvs_get_i8(myhandle, key, &value));
		nvs_close(myhandle);
	}
	return (int) value;

}

void write_nvs(char * name, char * key, int value)
{
	nvs_handle_t myhandle;
	ESP_ERROR_CHECK(nvs_open(name, NVS_READWRITE, &myhandle));
	ESP_ERROR_CHECK(nvs_set_i8(myhandle, key, (int8_t) value));
	ESP_ERROR_CHECK(nvs_commit(myhandle));
	nvs_close(myhandle);
}

/* Function for taking the element with value given, putting it to the begining and shifting the other elements */
int pop_nvs(char * name, int value)
{
	int index = -1;
	nvs_handle_t myhandle;
	ESP_ERROR_CHECK(nvs_open(name, NVS_READWRITE, &myhandle));
	
	for (int i = 0; i < networks_number; i++)
	{
		char key[2];
		if (snprintf(key, 2, "%d", i) < 0)
		{
			ESP_LOGE("NVS Pop", "Can't convert int to char *");
			index = -1;
			break;
		}
		else
		{
			int8_t result;
			ESP_ERROR_CHECK(nvs_get_i8(myhandle, key, &result));
			if ((int) result == value)
			{
				index = i;
				break;
			}
		}
	}

	for (int i = index; i > 0; i--)
	{
		char key[2];
		char key_2[2];
		if ((snprintf(key, 2, "%d", i) < 0) || (snprintf(key_2, 2, "%d", i - 1) < 0))
		{
			ESP_LOGE("NVS Pop", "Critical error: Can't convert int to char *");
			index = -1;
			break;
		}
		else
		{
			int8_t value_2;
			ESP_ERROR_CHECK(nvs_get_i8(myhandle, key_2, &value_2));
			ESP_ERROR_CHECK(nvs_set_i8(myhandle, key, value_2));
		}
	}
	if (index != -1)
	{
		ESP_ERROR_CHECK(nvs_set_i8(myhandle, "0\0", (int8_t) value));
		ESP_ERROR_CHECK(nvs_commit(myhandle));
	}
	nvs_close(myhandle);
	return index;
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

