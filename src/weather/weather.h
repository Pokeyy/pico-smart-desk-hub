
#ifndef WEATHER_H
#define WEATHER_H

#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/altcp.h"
#include "FreeRTOS.h"
#include "queue.h"

#define HOST "api.open-meteo.com"
#define URL_REQUEST "/v1/forecast?latitude=34.22&longitude=-119.04" \
                    "&daily=temperature_2m_max,temperature_2m_min"  \
                    "&temperature_unit=fahrenheit&forecast_days=3"  \
                    "&timezone=America%2FLos_Angeles"
                    
#define WIFI_RETRY_DELAY_MS                 5000
#define FETCH_INTERVAL_MS                   1800000

typedef enum {
    WEATHER_OK              =  0,
    WEATHER_ERR_HTTP        = -1,
    WEATHER_ERR_NO_JSON     = -2,
    WEATHER_ERR_PARSE       = -3,
    WEATHER_ERR_MISSING     = -4,
} weather_err_t;

typedef struct {
    int temps_max[3];
    int temps_min[3];
} weather_data_t;

extern QueueHandle_t weather_queue;

weather_err_t fetch_weather(weather_data_t *data);
void draw_weather_screen();
void weather_task (void *pvParameters);
err_t my_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err);
#endif