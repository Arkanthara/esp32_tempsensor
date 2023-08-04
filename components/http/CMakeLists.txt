idf_component_register( SRCS "http.c"
	INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
	REQUIRES esp_http_client mbedtls)
