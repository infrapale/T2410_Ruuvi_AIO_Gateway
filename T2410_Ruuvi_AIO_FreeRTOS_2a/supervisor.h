#ifndef __SUPERVISOR_H__
#define __SUPERVISOR_H__
#include "Arduino.h"
#include "main.h"


typedef enum 
{
    SUPER_RUUVI_INDX = 0,
    SUPER_WIFI_INDX,
    SUPER_MQTT_INDX,
    SUPER_NBR_OF
} super_indx_et;

typedef struct
{
    char  label[8];
    uint16_t  cnt;
    uint16_t  cnt_limit;
} super_st;

void super_task_code( void *pvParameters );
void super_clr_task_cntr(super_indx_et sindx);


#endif