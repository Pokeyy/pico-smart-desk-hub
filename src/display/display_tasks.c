#include "display/display_tasks.h"
#include "drivers/ST7735_TFT.h"
#include "weather/weather.h"


TaskHandle_t xDisplayTaskHandle = NULL;

void button_callback(uint gpio, uint32_t events) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_press_time < 300)
        return;
    else {
        last_press_time = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        vTaskNotifyGiveFromISR(xDisplayTaskHandle, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    // add direction later
}

void input_init() {
    gpio_init(SCREEN_SW_PREV);
    gpio_set_dir(SCREEN_SW_PREV, GPIO_IN);
    gpio_pull_up(SCREEN_SW_PREV);

    gpio_init(SCREEN_SW_NEXT);
    gpio_set_dir(SCREEN_SW_NEXT, GPIO_IN);
    gpio_pull_up(SCREEN_SW_NEXT);

    gpio_set_irq_enabled_with_callback(SCREEN_SW_PREV, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled(SCREEN_SW_NEXT, GPIO_IRQ_EDGE_FALL, true);
}

void draw_screens_task (void *pvParameters) {
    screen_type_t current_screen = SCREEN_MAIN;
    weather_data_t weather_data = {0};
    vTaskDelay(pdMS_TO_TICKS(1000));
    draw_main_screen();

     while (true) {
        uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

        if (notified > 0) {
            printf("Display task notified, switching screen\n");
            //vTaskDelay(pdMS_TO_TICKS(500));
           // printf("filling white\n");
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
                case SCREEN_CLOCK:
                    // draw time zone 
                    break;
            }
        }
        switch(current_screen) {
            case SCREEN_MAIN:
                 // check main updates
                break;
            case SCREEN_WEATHER:
                //printf("About to receive from queue\n");
                xQueueReceive(weather_queue, &weather_data, 0);
                for (int i = 0; i < 3; i++) {
                    char line[24];
                    snprintf(line, sizeof(line), "%s %dF/%dF", weather_data.temps_days[i], weather_data.temps_max[i], weather_data.temps_min[i]);
                    draw_string(2, 55 + (i * 20), line, ST7735_BLACK, ST7735_WHITE, 1);
                }
                break;
            case SCREEN_CLOCK:
                // fetch rtc data
                break;
        }
    }
}

void draw_weather_screen() {
    draw_string(55, 10, "NWS", ST7735_BLUE, ST7735_WHITE, 3);
    draw_string(25, 32, "Nguyen Weather Service", ST7735_BLACK, ST7735_WHITE, 1);
    
    draw_bitmap(100, 45, ICON_WEATHER_SUN, ST7735_YELLOW, ST7735_WHITE, 3);

    draw_bitmap(100, 70, ICON_WEATHER_SUN, ST7735_YELLOW, ST7735_WHITE, 3);

    draw_bitmap(100, 95, ICON_WEATHER_SUN, ST7735_YELLOW, ST7735_WHITE, 3);
}

void draw_main_screen() {
    draw_string(25, 10, "PEANEMIS", ST7735_BLUE, ST7735_WHITE, 3);
    draw_string(25, 32, "CONTACT FRANCIS NGUYEN", ST7735_BLACK, ST7735_WHITE, 1);
}