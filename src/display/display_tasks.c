#include "display/display_tasks.h"
#include "drivers/ST7735_TFT.h"
#include "weather/weather.h"


TaskHandle_t xDisplayTaskHandle = NULL;


void draw_screens_task (void *pvParameters) {
    screen_type_t current_screen = SCREEN_MAIN;
    weather_data_t weather_data = {0};
    vTaskDelay(pdMS_TO_TICKS(1000));
    draw_main_screen();

     while (true) {
        uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

        if (notified > 0) {
            printf("Display task notified, switching screen\n");
            vTaskDelay(pdMS_TO_TICKS(500));
            printf("filling white\n");
            current_screen = (current_screen + 1) % SCREEN_COUNT;
            fill_screen(ST7735_WHITE);

            switch (current_screen) {
                case SCREEN_MAIN:
                    printf("drawing main screen\n");
                    draw_main_screen();
                    break;
                case SCREEN_WEATHER:
                    printf("drawing weather screen\n");
                    draw_weather_screen();
                    printf("weather screen drawn\n");
                    break;
            }
        }
        switch(current_screen) {
            case SCREEN_MAIN:
                 // check main updates
                break;
            case SCREEN_WEATHER:
                if(xQueueReceive(weather_queue, &weather_data, 0) == pdTRUE) {
                    printf("Got weather data\n");
                    for (int i = 0; i < 3; i++) {
                        char line[24];
                        snprintf(line, sizeof(line), "D%d %dF/%dF", i+1, weather_data.temps_max[i], weather_data.temps_min[i]);
                        draw_string(10, 55 + (i * 20), line, ST7735_BLACK, ST7735_WHITE, 1);
                    }
                }
                // else {
                //     printf("queue empty\n");
                // }
                break;
        }
    }
}

void draw_weather_screen() {
    draw_string(55, 10, "NWS", ST7735_BLUE, ST7735_WHITE, 3);
    draw_string(25, 32, "Nguyen Weather Service", ST7735_BLACK, ST7735_WHITE, 1);
    
    draw_bitmap(75, 45, ICON_WEATHER_SUN, ST7735_YELLOW, ST7735_WHITE, 3);

    draw_bitmap(75, 70, ICON_WEATHER_SUN, ST7735_YELLOW, ST7735_WHITE, 3);

    draw_bitmap(75, 95, ICON_WEATHER_SUN, ST7735_YELLOW, ST7735_WHITE, 3);
}

void draw_main_screen() {
    draw_string(55, 10, "GUYKNOWBALLOGIST", ST7735_BLUE, ST7735_WHITE, 3);
    draw_string(25, 32, "CONTACT FRANCIS NGUYEN", ST7735_BLACK, ST7735_WHITE, 1);
}