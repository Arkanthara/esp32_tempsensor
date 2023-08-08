idf_component_register( SRCS "connect.c"
	INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}
	REQUIRES esp_wifi esp_netif freertos networkstorage)
