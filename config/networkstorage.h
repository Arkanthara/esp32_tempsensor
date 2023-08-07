#ifndef __NETWORKSTORAGE_H__
#define __NETWORKSTORAGE_H__

typedef struct NetworkStorage
{
	char ssid[32];
	char pwd[64];
}

extern NetworkStorage networks[10];
extern int networks_number;

#endif
