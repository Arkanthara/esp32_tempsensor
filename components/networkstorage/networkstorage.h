#ifndef __NETWORKSTORAGE_H__
#define __NETWORKSTORAGE_H__

typedef struct NetworkStorage
{
	// I have added + 1 because of the terminal byte
	char ssid[33];
	char pwd[65];
}NetworkStorage;

void init_networks(char * ssid, char * pwd);

void create_networks_table();

void create_nvs(char * name);

int read_nvs(char * name, int key);

void write_nvs(char * name, char * key, int value);

int pop_nvs(char * name, int value);

#endif
