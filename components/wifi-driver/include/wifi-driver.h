#include "esp_wifi.h"

void init_sta_mode();
void init_ap_mode();
void print_ip();
void init_wifi();
wifi_ap_record_t* scan_wifi_networks();   