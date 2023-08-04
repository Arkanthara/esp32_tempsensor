#ifndef __HTTP_H__
#define __HTTP_H__

#include "esp_http_client.h"

esp_http_client_handle_t http_init(void);
void http_read(esp_http_client_handle_t client);
void http_write(esp_http_client_handle_t client, char * buffer, int buffer_len);
void http_cleanup(esp_http_client_handle_t client);
void http_post(esp_http_client_handle_t client, char * buffer, int buffer_len);
void http_fetch_headers(esp_http_client_handle_t client);
void http_open(esp_http_client_handle_t client, int write_len);
// void http_client_post(char * data);
#endif
