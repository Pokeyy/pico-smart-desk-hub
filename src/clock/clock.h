#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>
#include "pico/cyw43_arch.h"
#include "pico/aon_timer.h"
#include "lwip/apps/sntp.h"
#include "wifi/wifi.h"

void clock_init();
void sntp_init();
void set_system_time(uint32_t sec);
#endif