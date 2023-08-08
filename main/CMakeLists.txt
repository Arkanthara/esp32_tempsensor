set (INCLUDE_COMPONENTS
	.
	)


idf_component_register(SRCS "esp32_tempsensor.c"
	INCLUDE_DIRS ${INCLUDE_COMPONENTS}
	REQUIRES nvs_flash freertos http temperature connect networkstorage)
