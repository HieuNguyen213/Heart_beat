#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_http_client.h"

#define BLYNK_AUTH_TOKEN "yGipMRfBMomh62rkfJxrzQ0XlSxm2rxN"
#define STA_SSID "so12 ngo9"
#define STA_PASSWORD "21032003"

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data); 
esp_err_t blynk_http_request(const char* url); 
void initialize_wifi(void); 
void send_data_to_blynk(int heart_beat, int SpO2);