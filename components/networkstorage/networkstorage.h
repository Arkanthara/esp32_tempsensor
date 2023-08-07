#ifndef __NETWORKSTORAGE_H__
#define __NETWORKSTORAGE_H__

typedef struct NetworkStorage
{
	// I have added + 1 because of the terminal byte
	char ssid[33];
	char pwd[65];
}NetworkStorage;

void init_networks(char * ssid, char * pwd);

#endif