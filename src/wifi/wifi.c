#include "wifi/wifi.h"
#include "FreeRTOS.h"
#include "task.h"

EventGroupHandle_t wifi_group;

void wifi_task(void *pvParameters) {
    printf("Starting WTTR.in Pico W client...\n");

    int cy43_count_MAX = 5;
    int init_success = 0;
    for(int i = 0; i < cy43_count_MAX; ++i) {
        if (!cyw43_arch_init())
        {
            init_success = 1;
            printf("cy43 initialized successfully\n");
            break;
        }
        else {
            printf("cy43 failed to intiialize.. trying again\n");
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
        
        
    }

    if(init_success == 0) {
        xEventGroupSetBits(wifi_group, 0x02);
        vTaskDelete(NULL);
    }

    cyw43_arch_enable_sta_mode();

    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD,
                                              CYW43_AUTH_WPA2_AES_PSK, 10000)) {
                printf("Wi-Fi connect failed, retrying...\n");
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
    xEventGroupSetBits(wifi_group, 0x01);
    printf("Wi-Fi connected!\n");

    
    while (true) {
        if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
            xEventGroupClearBits(wifi_group, 0x01);
            printf("Wi-Fi was lost, attempting to reconnect..\n");
            
            if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD,
                                            CYW43_AUTH_WPA2_AES_PSK, 30000) == 0) {
                xEventGroupSetBits(wifi_group, 0x01);
                printf("Wi-Fi reconnected!\n");                      
            }
            else {
                printf("Reconnecting failed, trying a few more times..\n");
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
        }
    }
}
