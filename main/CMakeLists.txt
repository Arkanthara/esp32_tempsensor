set (INCLUDE_COMPONENTS
		.
		../config
		)


idf_component_register(SRCS "esp32_tempsensor.c"
                    INCLUDE_DIRS INCLUDE_COMPONENTS
					REQUIRED http temperature connect freertos)
