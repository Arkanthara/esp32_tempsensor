#include "networkstorage.h"
#include "esp_log.h"
#include "string.h"
#include "nvs.h"

extern NetworkStorage networks[10];
extern int networks_number;


/* Function for create the networks table */
void create_nvs(char * name)
{
	/* We begin by opening the nvs */
	nvs_handle_t myhandle;
	ESP_ERROR_CHECK(nvs_open(name, NVS_READWRITE, &myhandle));

	/* Then, foreach network in networks tab, we add it into the nvs. (We add as key and value the index...) */
	for (int i = 0; i < networks_number; i++)
	{
		/* Here the size is 2 because of the '\0' to the end of the char *.... */
		char key[2];

		/* We parse int to char * for create the key used into our nvs, and we stock the index as key and value... */
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

	/* We apply changes to our nvs and we close the connection to our nvs */
	ESP_ERROR_CHECK(nvs_commit(myhandle));
	nvs_close(myhandle);

}

/* This function is used for read the value stored in the nvs to the index given */
int read_nvs(char * name, int key_given)
{
	/* We convert the key into char * */
	int8_t value = -1;
	char key[2];
	if (snprintf(key, 2, "%d", key_given) < 0)
	{
		ESP_LOGE("NVS Read", "Can't convert int to char *");
	}

	/* We open, read and close our nvs */
	else
	{
		nvs_handle_t myhandle;
		ESP_ERROR_CHECK(nvs_open(name, NVS_READONLY, &myhandle));
		ESP_ERROR_CHECK(nvs_get_i8(myhandle, key, &value));
		nvs_close(myhandle);
	}
	return (int) value;

}

/* Function for taking the element with value given, putting it to the begining and shifting the other elements */
int pop_nvs(char * name, int value)
{
	/* We initialise our index to -1 (it's the value for indicate an error....) */
	int index = -1;

	/* We open our nvs */
	nvs_handle_t myhandle;
	ESP_ERROR_CHECK(nvs_open(name, NVS_READWRITE, &myhandle));

	/* We search our element with the value given */
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

	/* If we find it, we shift all the other elements */
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

	/* Finally, we indicate that the value given is at the begining (so with the lower key) */
	if (index != -1)
	{
		ESP_ERROR_CHECK(nvs_set_i8(myhandle, "0\0", (int8_t) value));
		ESP_ERROR_CHECK(nvs_commit(myhandle));
	}
	nvs_close(myhandle);
	return index;
}

/* Function for add a network to the netwoks table */
void init_networks(char * ssid, char * pwd)
{
	/* We verify that the ssid and pwd aren't too long */
	if (strlen(ssid) >= 32 || strlen(pwd) >= 64)
	{
		ESP_LOGE("Network Storage", "Error: network ssid of network pwd is too long !");
	}

	/* Else, we add ssid and pwd into the networks table */
	else
	{
		strcpy(networks[networks_number].ssid, ssid);
		strcpy(networks[networks_number].pwd, pwd);
		networks_number ++;
	}
}

/* I see that in menu config, when nothing is given, we have only the '\0', so the size is equal to 1 for '' .... */
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
