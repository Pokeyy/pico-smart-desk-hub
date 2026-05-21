#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp.h"
#include "config/lwipopts.h"
#include "lib/http_client_util.h"
#include "weather.h"
#include "lib/cJSON.h"
#include "drivers/ST7735_TFT.h"
#include "wifi/wifi.h"
#include "FreeRTOS.h"
#include "task.h"

static char buffer_weather[2048];
static int buffer_index = 0;

void weather_task(void *pvParameters) {
    int attempts;
    xEventGroupWaitBits(wifi_group, 0x01, pdFALSE, pdTRUE, portMAX_DELAY);
    while (true) {
        attempts = 0;
        while (attempts < 5) {
            weather_data_t weather_data;
            if (fetch_weather(&weather_data) == WEATHER_OK) {
                xQueueOverwrite(weather_queue, &weather_data);
                break;
            }
            else {
                attempts++;
                vTaskDelay(pdMS_TO_TICKS(3000));
            }
            
        }
        vTaskDelay(pdMS_TO_TICKS(1800000));
    }
}

weather_err_t fetch_weather(weather_data_t *data)
{
    buffer_index = 0;
    memset(buffer_weather, 0, sizeof(buffer_weather));

    EXAMPLE_HTTP_REQUEST_T req = {0};
    req.hostname = HOST;
    req.url      = URL_REQUEST;
    req.headers_fn = NULL;
    req.recv_fn  = my_recv_fn;

    int http_result = http_client_request_sync(cyw43_arch_async_context(), &req);
    if (http_result != 0)
        printf("HTTP returned non-zero: %d\n", http_result);

    buffer_weather[buffer_index < sizeof(buffer_weather)
                   ? buffer_index
                   : sizeof(buffer_weather) - 1] = '\0';

    printf("FINAL BUFFER SIZE: %d\n", buffer_index);

    // Find the JSON object (skip any HTTP framing just in case)
    char *json_start = strchr(buffer_weather, '{');
    if (!json_start)
    {
        printf("No JSON found\n");
        printf("RAW: %.120s\n", buffer_weather);
        return WEATHER_ERR_NO_JSON;
    }

    cJSON *root = cJSON_Parse(json_start);
    if (!root)
    {
        printf("JSON parse failed\n");
        printf("RAW: %.120s\n", json_start);
        return WEATHER_ERR_PARSE;
    }

    // Navigate: root -> "daily" -> "temperature_2m_max" / "temperature_2m_min"
    cJSON *daily = cJSON_GetObjectItem(root, "daily");
    if (!cJSON_IsObject(daily))
    {
        printf("'daily' object missing\n");
        cJSON_Delete(root);
        return WEATHER_ERR_MISSING;
    }

    cJSON *maxArr = cJSON_GetObjectItem(daily, "temperature_2m_max");
    cJSON *minArr = cJSON_GetObjectItem(daily, "temperature_2m_min");

    if (!cJSON_IsArray(maxArr) || !cJSON_IsArray(minArr))
    {
        printf("temperature arrays missing\n");
        cJSON_Delete(root);
        return WEATHER_ERR_HTTP;
    }

    for (int i = 0; i < 3; i++)
    {
        cJSON *maxItem = cJSON_GetArrayItem(maxArr, i);
        cJSON *minItem = cJSON_GetArrayItem(minArr, i);

        // Open-Meteo returns numbers, not strings
        data->temps_max[i] = cJSON_IsNumber(maxItem) ? (int)maxItem->valuedouble : -1;
        data->temps_min[i] = cJSON_IsNumber(minItem) ? (int)minItem->valuedouble : -1;

        printf("Day %d: High %dF / Low %dF\n", i, data->temps_max[i], data->temps_min[i]);
    }

    
    cJSON_Delete(root);
    return 0;  // don't use `result` from http call — it may be non-zero on clean close
}

// void draw_weather_task(void *pvParameters)
// {
//     printf("the_weather called\n");
//     int temps_max[3], temps_min[3];
//     char temp_str[16];

//     draw_weather_screen();
//     xEventGroupWaitBits(wifi_group, 0x01, pdFALSE, pdTRUE, portMAX_DELAY);
//     while (true)
//     {

//         int attempt = 0;
//         int result = -1;


//         draw_string(5, 100, "Updating...", ST7735_BLACK, ST7735_WHITE, 1);
//         while (attempt < 5)
//         {
//             result = fetch_weather(data->temps_max, temps_min);
//             if (result == WEATHER_OK)
//                 break;
//             attempt++;
//             if (attempt < 5) vTaskDelay(pdMS_TO_TICKS(2000)); // only sleep if actually retrying
//         }
//         fill_rectangle(5, 100, 85, 9, ST7735_WHITE);

//         if (result == WEATHER_OK)
//         {
//             printf("Stack HWM: %d words\n", uxTaskGetStackHighWaterMark(NULL));
//             for (int i = 0; i < 3; i++)
//             {
//                 char line[24];
//                 snprintf(line, sizeof(line), "D%d %dF/%dF", i+1, temps_max[i], temps_min[i]);
//                 draw_string(10, 55 + (i * 20), line, ST7735_BLACK, ST7735_WHITE, 1);
//             }
//         }
//         else
//         {
//             switch (result) {
//                 case WEATHER_ERR_HTTP:    printf("HTTP request failed\n");   break;
//                 case WEATHER_ERR_NO_JSON: printf("No JSON in response\n");   break;
//                 case WEATHER_ERR_PARSE:   printf("JSON parse failed\n");     break;
//                 case WEATHER_ERR_MISSING: printf("Missing expected field\n"); break;
//             }
//         }

//         printf("Sleeping...\n\n");
//         vTaskDelay(pdMS_TO_TICKS(FETCH_INTERVAL_MS));
//     }

//     cyw43_arch_deinit();
// }

err_t my_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    if (!p) return ERR_OK;

    int remaining = sizeof(buffer_weather) - buffer_index - 1;

    if (remaining <= 0)
    {
        printf("BUFFER FULL - DROPPING PACKET\n");
        pbuf_free(p);
        return ERR_OK;
    }

    int len = p->tot_len;
    if (len > remaining)
        len = remaining;

    pbuf_copy_partial(p, buffer_weather + buffer_index, len, 0);

    buffer_index += len;

    printf("RX chunk: %d bytes (total buffer: %d)\n",
           len, buffer_index);

    pbuf_free(p);
    return ERR_OK;
}

