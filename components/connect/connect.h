#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "esp_netif.h"

esp_netif_t * init_wifi(void);
void disconnect_wifi(esp_netif_t * netif);
void scan_wifi();

#endif
