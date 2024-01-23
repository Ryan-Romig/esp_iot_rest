#include "../components/config-manager/include/config-manager.h"
#include "../components/rest-server/include/rest-server.h"
#include "../components/wifi-driver/include/wifi-driver.h"
#include "../components/smart-home/include/smart-home-wrapper.h"
#include "Global.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <string.h>
static const char* TAG = "MAIN";

void restart_device()
{
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("*", ESP_LOG_DEBUG);

    init_nvs();
    ESP_LOGE(TAG, "NVS INITIALIZED");
    init_wifi();
    ESP_LOGE(TAG, "WIFI INITIALIZED");
    init_sta_mode();
    ESP_LOGE(TAG, "STA INITIALIZED");
    start_wifi_server();
    ESP_LOGE(TAG, "REST INITIALIZED");
    init_smart_home();
    ESP_LOGE(TAG, "MATTER INITIALIZED");
}