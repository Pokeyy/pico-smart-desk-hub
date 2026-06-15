#include "clock/clock.h"

void clock_init() {
    cyw43_arch_lwip_begin();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();
    cyw43_arch_lwip_end();
}



void set_system_time(uint32_t sec) {
    time_t adjusted_sec = sec - (7 * 3600);
    struct tm *time_info = localtime((time_t *)&adjusted_sec);
    aon_timer_start_calendar(time_info);
}