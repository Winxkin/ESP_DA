#include "main.h"

/*---Smartconfig wifi---*/
static void smartconfig_example_task(void *parm);

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 3, NULL);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        ESP_LOGI(WIFI, "Scan done");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        ESP_LOGI(WIFI, "Found channel");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        ESP_LOGI(WIFI, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        uint8_t rvd_data[33] = {0};

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
        {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(WIFI, "SSID:%s", ssid);
        ESP_LOGI(WIFI, "PASSWORD:%s", password);
        /*get password and SSID here*/

        /*----------------------------*/
        if (evt->type == SC_TYPE_ESPTOUCH_V2)
        {
            ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
            ESP_LOGI(WIFI, "RVD_DATA:");
            for (int i = 0; i < 33; i++)
            {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void smartconfig_example_task(void *parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if (uxBits & CONNECTED_BIT)
        {
            ESP_LOGI(WIFI, "WiFi Connected to ap");
        }
        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(WIFI, "smartconfig over");
            esp_smartconfig_stop();
            smartconfig_status = WIFI_CONNECTED;
            vTaskDelete(NULL);
        }
    }
}

/*---Smartconfig wifi---*/

/*---http---*/
static void http_get_task(void *pvParameters)
{
    if (http_status == HTTP_ACCESS)
    {
        const struct addrinfo hints = {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM,
        };
        struct addrinfo *res;
        struct in_addr *addr;
        int s, r;
        char recv_buf[64];

        while (1)
        {
            int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

            if (err != 0 || res == NULL)
            {
                ESP_LOGE(HTTP, "DNS lookup failed err=%d res=%p", err, res);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }

            /* Code to print the resolved IP.

               Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
            addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
            ESP_LOGI(HTTP, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

            s = socket(res->ai_family, res->ai_socktype, 0);
            if (s < 0)
            {
                ESP_LOGE(HTTP, "... Failed to allocate socket.");
                freeaddrinfo(res);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            ESP_LOGI(HTTP, "... allocated socket");

            if (connect(s, res->ai_addr, res->ai_addrlen) != 0)
            {
                ESP_LOGE(HTTP, "... socket connect failed errno=%d", errno);
                close(s);
                freeaddrinfo(res);
                vTaskDelay(4000 / portTICK_PERIOD_MS);
                continue;
            }

            ESP_LOGI(HTTP, "... connected");
            freeaddrinfo(res);

            if (write(s, REQUEST, strlen(REQUEST)) < 0)
            {
                ESP_LOGE(HTTP, "... socket send failed");
                close(s);
                vTaskDelay(4000 / portTICK_PERIOD_MS);
                continue;
            }
            ESP_LOGI(HTTP, "... socket send success");

            struct timeval receiving_timeout;
            receiving_timeout.tv_sec = 5;
            receiving_timeout.tv_usec = 0;
            if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                           sizeof(receiving_timeout)) < 0)
            {
                ESP_LOGE(HTTP, "... failed to set socket receiving timeout");
                close(s);
                vTaskDelay(4000 / portTICK_PERIOD_MS);
                continue;
            }
            ESP_LOGI(HTTP, "... set socket receiving timeout success");

            /* Read HTTP response */
            do
            {
                bzero(recv_buf, sizeof(recv_buf));
                r = read(s, recv_buf, sizeof(recv_buf) - 1);
                for (int i = 0; i < r; i++)
                {
                    putchar(recv_buf[i]);
                }
            } while (r > 0);

            ESP_LOGI(HTTP, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
            close(s);
            for (int countdown = 10; countdown >= 0; countdown--)
            {
                ESP_LOGI(HTTP, "%d... ", countdown);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            ESP_LOGI(HTTP, "Starting again!");
        }
    }
}

/*---http---*/

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    smartconfig_status = WIFI_STATUS_SET;
    http_status = HTTP_WAIT;
    initialise_wifi();
    while (1)
    {
        if (smartconfig_status == WIFI_CONNECTED)
        {
            // xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
            break;
        }
    }
    while (1)
    {
        ESP_LOGI(WIFI, "do something");
        delay_s(10);
    }
}