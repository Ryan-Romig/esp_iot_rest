#include "time-server.h"
#include "../../main/Global.h"
#include "esp_log.h"
#include "sntp.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
static const char* TAG = "TIME SERVER";
void time_sync_notification_cb(struct timeval* tv)
{
    // Handle time synchronization notification
    ESP_LOGI(TAG, "Time synchronized. Epoch time: %ld", tv->tv_sec);
    // Additional code here if needed
    isTimeUpdated = 1;
    get_local_time();
}
void initialize_sntp(void)
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}

void start_time_server_task(void)
{
    initialize_sntp();
    while (1) {
        vTaskDelay(900000 / portTICK_PERIOD_MS);
    }
}

struct timeval get_local_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ESP_LOGI(TAG, "Time: %ld", tv.tv_sec);
    return tv;
}
time_t get_epoch_time(void) {
    return time(NULL);
}