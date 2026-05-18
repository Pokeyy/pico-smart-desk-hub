#ifndef WIFI_H 
#define WIFI_H
#include "pico/cyw43_arch.h"
#include "event_groups.h"

#define WIFI_CONNECTED_BIT          0x01
#define WIFI_FAILED_BIT             0x02

extern EventGroupHandle_t wifi_group;

void wifi_task(void *pvParameters);

#endif