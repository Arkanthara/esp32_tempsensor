#ifndef __HTTP_H__
#define __HTTP_H__

#include "esp_http_client.h"

esp_http_client_handle_t http_init(void);
void http_post(esp_http_client_handle_t client, char * buffer, int buffer_len);
void http_cleanup(esp_http_client_handle_t client);

#endif
