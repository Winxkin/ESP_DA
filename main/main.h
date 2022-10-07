#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*Freertos */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/*smartconfig wifi */
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

/*http request*/
//#include "protocol_examples_common.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

/*my libbrary*/
#include "delay.h"

/*smart config*/
#define WIFI_CONNECTED 1
#define WIFI_STATUS_SET 0

static int smartconfig_status;

static EventGroupHandle_t s_wifi_event_group;

static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *WIFI = "smartconfig wifi";

/*http*/
/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "example.com"
#define WEB_PORT "80"
#define WEB_PATH "/"
#define HTTP_ACCESS 1
#define HTTP_WAIT 0

static int http_status;

static const char *HTTP = "example";

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
                             "Host: " WEB_SERVER ":" WEB_PORT "\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";
#endif
