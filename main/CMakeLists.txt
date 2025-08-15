set(srcs main.c
	     ./src/connect_wifi.c)
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS .
                    ./include)
