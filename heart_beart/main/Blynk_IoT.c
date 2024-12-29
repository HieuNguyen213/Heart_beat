#include "Blynk_IoT.h"

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_CONNECTED) {
            printf("Connected to Wi-Fi network: %s\n", STA_SSID);
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            printf("Disconnected from Wi-Fi network: %s\n", STA_SSID);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
    }
}

esp_err_t blynk_http_request(const char* url) {
    esp_http_client_config_t config = {
        .url = url,
        .skip_cert_common_name_check = true, // Bỏ qua xác thực SSL
        .use_global_ca_store = false, // Không sử dụng CA store
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        printf("HTTP request succeeded: %s\n", url);
    } else {
        printf("HTTP request failed: %s\n", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    return err;
}

void initialize_wifi(void) {
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize Wi-Fi in STA mode
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

    // Register Wi-Fi and IP event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    // Configure Wi-Fi STA
    wifi_config_t sta_config = {
        .sta = {
            .ssid = STA_SSID,
            .password = STA_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    // Configure DNS
    esp_netif_dns_info_t dns_info;
    dns_info.ip.u_addr.ip4.addr = esp_ip4addr_aton("8.8.8.8");
    dns_info.ip.type = IPADDR_TYPE_V4;
    esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), ESP_NETIF_DNS_MAIN, &dns_info);
}

void send_data_to_blynk(int k, int l) {
    char url_v0[256];
    snprintf(url_v0, sizeof(url_v0), "http://blynk.cloud/external/api/update?token=%s&pin=V0&value=%d", BLYNK_AUTH_TOKEN, k);
    printf("Sending HTTP request to: %s\n", url_v0);
    if (blynk_http_request(url_v0) != ESP_OK) {
        printf("Failed to send data to V0\n");
        return;
    }

    char url_v1[256];
    snprintf(url_v1, sizeof(url_v1), "http://blynk.cloud/external/api/update?token=%s&pin=V1&value=%d", BLYNK_AUTH_TOKEN, l);
    printf("Sending HTTP request to: %s\n", url_v1);
    if (blynk_http_request(url_v1) != ESP_OK) {
        printf("Failed to send data to V1\n");
        return;
    }
}
