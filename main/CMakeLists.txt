set (INCLUDE_COMPONENTS
	.
	##../config
	)


idf_component_register(SRCS "esp32_tempsensor.c"
	INCLUDE_DIRS ${INCLUDE_COMPONENTS}
	REQUIRES nvs_flash http temperature connect freertos networkstorage)
