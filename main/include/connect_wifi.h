#ifndef CONNECT_WIFI_H_
#define CONNECT_WIFI_H_

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <esp_system.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <nvs_flash.h>

extern int wifi_connect_status;

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASS CONFIG_WIFI_PASSWORD
#define MAXIMUM_RETRY 15
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define NETIF_IP CONFIG_NETIF_IP
#define NETIF_GATEWAY CONFIG_NETIF_GATEWAY
#define NETIF_NETMASK CONFIG_NETIF_NETMASK

void connect_wifi(void);

#endif
