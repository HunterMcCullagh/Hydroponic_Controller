idf_component_register(SRCS "ESP32_hydroponic_control.c" "html_page.c" 
                    INCLUDE_DIRS "."
                    REQUIRES led_strip esp_http_server esp_wifi esp_netif nvs_flash driver)
