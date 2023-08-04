set(INCLUDE_COMPONENT_DIR
	${CMAKE_CURRENT_LIST_DIR})
list(APPEND INCLUDE_COMPONENT_DIR
	../../config)

idf_component_register( SRCS "connect.c"
	INCLUDE_DIRS ${INCLUDE_COMPONENT_DIR}
	REQUIRES esp_wifi esp_netif freertos chained_list)
