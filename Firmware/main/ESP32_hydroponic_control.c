#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "driver/gpio.h"
#include "led_strip.h"

#include "html_page.h"

static const char *TAG = "websrv";
#define GPIO_LED 2




/* HTML page to serve */
// const char* html_page =
//     "<!DOCTYPE html><html><head><title>Hydroponic Control Web Server</title></head><body>"
//     "<h1>Hydroponic Control Web Server</h1>"

//     "<p><a href=\"/led/on\"><button>LED ON</button></a></p>"

//     "<p><a href=\"/led/off\"><button>LED OFF</button></a></p>"

//     "<form action=\"/submit\" method=\"GET\">"
//     "<label for=\"name\">Enter text:</label>"
//     "<input type=\"text\" id=\"name\" name=\"name\">"
//     "<input type=\"submit\" value=\"Send\">"
//     "</form>"

//     "</body></html>";

//extern int var1, var2, var3;
int var1 = 10;
int var2 = 20;
int var3 = 30;

/* URI handlers */
esp_err_t root_get_handler(httpd_req_t *req) {

    char page_buffer[1024];
    snprintf(page_buffer, sizeof(page_buffer), html_page, var1, var2, var3); //add variables


    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, page_buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void return_to_main_page(httpd_req_t *req)
{
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
}


led_strip_handle_t led_strip;
led_strip_config_t strip_config = {
    .strip_gpio_num = 48,
    .max_leds = 1, // Number of LEDs in the strip
    .led_model = LED_MODEL_WS2812,
    .flags.invert_out = false,
};

led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 10 * 1000 * 1000, // 10MHz resolution
    .mem_block_symbols = 64,
    .flags.with_dma = false,
};

esp_err_t led_on_handler(httpd_req_t *req) {
    gpio_set_level(GPIO_LED, 1);
    led_strip_set_pixel(led_strip, 0, 255, 0, 0); // Red
    led_strip_refresh(led_strip); // Wait up to 100ms for refresh

    ESP_LOGI(TAG, "LED is ON");

    // httpd_resp_set_status(req, "303 See Other");
    // httpd_resp_set_hdr(req, "Location", "/");
    // httpd_resp_send(req, NULL, 0);
    return_to_main_page(req);

    return ESP_OK;
}

esp_err_t led_off_handler(httpd_req_t *req) {
    gpio_set_level(GPIO_LED, 0);
    led_strip_clear(led_strip);
    
    ESP_LOGI(TAG, "LED is OFF");

    // httpd_resp_set_status(req, "303 See Other");
    // httpd_resp_set_hdr(req, "Location", "/");
    // httpd_resp_send(req, NULL, 0);
    return_to_main_page(req);
    return ESP_OK;
}


esp_err_t submit_handler(httpd_req_t *req) {
    char buf[100];
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        httpd_req_get_url_query_str(req, buf, buf_len);
        char param[32];
        if (httpd_query_key_value(buf, "name", param, sizeof(param)) == ESP_OK) {
            ESP_LOGI(TAG, "Received input: %s", param);
        }
    }
    //httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);

    return_to_main_page(req);
    return ESP_OK;
}


void register_uri_handlers(httpd_handle_t server) {
    httpd_uri_t root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = NULL
    };
    // httpd_uri_t led = {
    //     .uri       = "/led/*",
    //     .method    = HTTP_GET,
    //     .handler   = led_handler,
    //     .user_ctx  = NULL
    // };

    httpd_uri_t led_on = {
    .uri       = "/led/on",
    .method    = HTTP_GET,
    .handler   = led_on_handler,
    .user_ctx  = NULL
    };

    httpd_uri_t led_off = {
        .uri       = "/led/off",
        .method    = HTTP_GET,
        .handler   = led_off_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t submit = {
        .uri       = "/submit",
        .method    = HTTP_GET,
        .handler   = submit_handler,
        .user_ctx  = NULL
    };

    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &led_on);
    httpd_register_uri_handler(server, &led_off);
    httpd_register_uri_handler(server, &submit);
    //httpd_register_uri_handler(server, &led);
}

httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "Starting server on port %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        register_uri_handlers(server);
    }
    return server;
}

void stop_webserver(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}

static void wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Hunters Iphone",
            .password = "pelicans"
        },
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
    ESP_LOGI(TAG, "Connecting to Wi-Fi SSID:%s", wifi_config.sta.ssid);
    /* Optionally wait for IP event here */
}


void app_main(void) {
    nvs_flash_init();
    gpio_reset_pin(GPIO_LED);
    gpio_set_direction(GPIO_LED, GPIO_MODE_OUTPUT);

    led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    
    wifi_init_sta();

    /* delay or wait for Wi-Fi connection and IP obtainment before starting server */
    vTaskDelay(pdMS_TO_TICKS(5000));

    httpd_handle_t server = start_webserver();

    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip_info);
    ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info.ip));

    if(ip_info.ip.addr == 0)
    {

        while(ip_info.ip.addr == 0)
        {
            ESP_LOGI(TAG, "Error with internet connection, trying again");
            esp_wifi_disconnect();
            vTaskDelay(pdMS_TO_TICKS(1000));
            esp_wifi_stop();

            vTaskDelay(pdMS_TO_TICKS(1000));  // short delay before retry
            esp_wifi_start();
            vTaskDelay(pdMS_TO_TICKS(1000));
            esp_wifi_connect();

            esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
            esp_netif_get_ip_info(netif, &ip_info);
            ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info.ip));
        }
        
    }

    // Keep running
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    // Optional cleanup:
    // stop_webserver(server);
    // esp_wifi_stop();
}
