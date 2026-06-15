#ifndef DISPLAY_TASKS_H
#define DISPLAY_TASKS_H

#include "FreeRTOS.h"
#include "task.h"

#ifndef SCREEN_SW_PREV 
    #define SCREEN_SW_PREV             14
#endif

#ifndef SCREEN_SW_NEXT
    #define SCREEN_SW_NEXT             15
#endif


typedef enum {
    SCREEN_MAIN         =   0,
    SCREEN_WEATHER      =   1,
    SCREEN_CLOCK        =   2,
    //SCREEN_MUSIC        =   3,
    SCREEN_COUNT            
} screen_type_t;

extern TaskHandle_t xDisplayTaskHandle;

void button_callback(uint gpio, uint32_t events);

void input_init();

void draw_screens_task (void *pvParameters);

void draw_weather_screen();

void draw_clock_screen();

void draw_main_screen();

#endif