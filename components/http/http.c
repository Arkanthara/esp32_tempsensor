#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

char * buffer = NULL;
int buffer_len = 0;


// Function for treat event caused by http
esp_err_t http_event(esp_http_client_event_t * event)
{
	switch(event->event_id)
	{
		case HTTP_EVENT_ERROR:
			ESP_LOGE("HTTP Error", "Error: %s", strerror(esp_http_client_get_errno(event->client)));
			break;

		case HTTP_EVENT_ON_CONNECTED:
			ESP_LOGI("HTTP Status", "Connected");
			break;

		// When we got data, we register then in a buffer
		case HTTP_EVENT_ON_DATA:
			buffer_len += event->data_len;
			buffer = realloc(buffer, sizeof(char) * buffer_len);
			for (int i = 0; i < event->data_len; i ++)
			{
				buffer[buffer_len - event->data_len + i ] = ((char * ) event->data)[i];
			}
			break;

		// If we finish the exchange, we print data, and clean the buffer
		case HTTP_EVENT_ON_FINISH:
			ESP_LOGI("HTTP Data", "Http transfert is finished.\nThis is the data received:");
			write(1, buffer, buffer_len);
			free(buffer);
			buffer = NULL;
			buffer_len = 0;
			printf("\n");
			break;

		// If we don't read all data claimed, we read then and clean the buffer
		case HTTP_EVENT_DISCONNECTED:
			if (buffer_len != 0)
			{
				ESP_LOGE("HTTP Status", "Disconnected. We don't read this data:");
				write(1, buffer, buffer_len);
				free(buffer);
				buffer = NULL;
				buffer_len = 0;
				printf("\n");
			}
			else
			{
				ESP_LOGE("HTTP Status", "Disconnected");
			}
			break;

		default:
			break;
	}
	return ESP_OK;
}

// Function for read server response, and print then
void http_read(esp_http_client_handle_t client)
{
	ESP_ERROR_CHECK(esp_http_client_flush_response(client, NULL));
	if (esp_http_client_is_complete_data_received(client))
	{
		ESP_LOGI("HTTP Data", "Data have been received successfully.\nThis is the data received:");
		write(1, buffer, buffer_len);
		free(buffer);
		buffer = NULL;
		buffer_len = 0;
		printf("\n");
	}
	else
	{
		ESP_LOGE("HTTP Data", "Error: we didn't receive all data");
	}
}

// Function for send data to server
void http_write(esp_http_client_handle_t client, char * buffer, int buffer_len)
{
	int length_written = esp_http_client_write(client, buffer, buffer_len);
	if (length_written == -1)
	{
		ESP_LOGE("HTTP Data", "Error when trying write some data to the server");
	}
	else if (length_written < buffer_len)
	{
		ESP_LOGE("HTTP Data", "Error: we don't send %d data but %d data to the server", buffer_len, length_written);
	}
	else
	{
		ESP_LOGI("HTTP Data", "Message has been successfully send to server");
	}
}


// Function for fetch headers
void http_fetch_headers(esp_http_client_handle_t client)
{
	int error = esp_http_client_fetch_headers(client);
	if (error == -1)
	{
		ESP_LOGE("HTTP Fetch", "Failed to fecth headers");
	}
	else
	{
		http_read(client);
	}
}

void http_open(esp_http_client_handle_t client, int write_len)
{
	// We open the connection and write all the things to write
	int error = esp_http_client_open(client, write_len);
	if (error != ESP_OK)
	{
		ESP_LOGE("HTTP", "We can't open connection: %s", esp_err_to_name(error));
	}
}

void http_post(esp_http_client_handle_t client, char * buffer, int buffer_len)
{
	// I put -1 because there is a strange thing on the web with the '\n'

	// We open http connection and indicate that we want write a message of size buffer_len - 1
	int error = esp_http_client_open(client, buffer_len);
	if (error != ESP_OK)
	{
		ESP_LOGE("HTTP", "We can't open connection: %s", esp_err_to_name(error));
		return;
	}

        error = esp_http_client_set_header(client, "Content-Type", "application/json");
	if (error != ESP_OK)
        {
	        ESP_LOGE("HTTP Headers", "Error when trying set headers: %s", esp_err_to_name(error));
	        return;
	}
	
        error = esp_http_client_set_header(client, "User-Agent", "Sinbad-Le-Marin");
	if (error != ESP_OK)
        {
	        ESP_LOGE("HTTP Headers", "Error when trying set headers: %s", esp_err_to_name(error));
	        return;
	}

	// We write message
	http_write(client, buffer, buffer_len);

	// We fecth headers and read the server's response
	http_fetch_headers(client);

	// We close connection
	error = esp_http_client_close(client);
	if (error != ESP_OK)
	{
		ESP_LOGE("HTTP", "We can't close connection: %s", esp_err_to_name(error));
	}
}


// Function for initialize connection to server
esp_http_client_handle_t http_init(void)
{
	// We create the configuration for http connection
	esp_http_client_config_t config = {
		.url = CONFIG_URL,
		.event_handler = http_event,
		//		.transport_type = HTTP_TRANSPORT_OVER_SSL,
		.crt_bundle_attach = esp_crt_bundle_attach,
	};

	// We initialize the connection
	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL)
	{
		ESP_LOGE("HTTP Initialization", "Failed");
		return NULL;
	}

	// Set method to POST
	ESP_ERROR_CHECK(esp_http_client_set_method(client, HTTP_METHOD_POST));

	// Return esp_http_client_handle_t
	return client;
}

/*
void http_client_post(char * data)
  
{
  // Initialization of our connection... Note that we must put data in url
  char new_url[strlen(data) + strlen(CONFIG_URL)];
  strcat(new_url, CONFIG_URL);
  strcat(new_url, data);
  esp_http_client_config_t config = {
    .url = new_url,
    .event_handler = http_event,
    .crt_bundle_attach = esp_crt_bundle_attach,
  };

  // We initialize the connection
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL)
    {
      ESP_LOGE("HTTP Initialization", "Failed");
      return NULL;
    }

  // Set method to POST
  ESP_ERROR_CHECK(esp_http_client_set_method(client, HTTP_METHOD_POST));

  // Open the connection
  http_open(client, 0);

  // Fetch headers
  http_fetch_headers(client);

  // close the connection
  int error = esp_http_client_close(client);
  if (error != ESP_OK)
    {
      ESP_LOGE("HTTP Close", "Can't close connection... Error: %s", esp_err_to_name(error));
    }
  // Free resources
  ESP_ERROR_CHECK(esp_http_client_cleanup(client));
  
}
*/

// Function for free all resouces 
void http_cleanup(esp_http_client_handle_t client)
{
	// ESP_ERROR_CHECK(esp_http_client_close(client));
	ESP_ERROR_CHECK(esp_http_client_cleanup(client));
}


