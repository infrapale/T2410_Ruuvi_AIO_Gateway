#ifndef __MQTT_TASK_H__
#define __MQTT_TASK_H__
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "main.h"
#include "ruuvi_tag.h"

#define AIO_PUBL_NBR_OF     5

typedef enum
{
    UNIT_TEMPERATURE = 0,
    UNIT_HUMIDITY,
    UNIT_AIR_PRESSURE,
    UNIT_LIGHT,
    UNIT_LDR,
    UNIT_RSSI,
    UNIT_VOLTAGE
} unit_et;

typedef enum
{
    VALUE_TYPE_UNDEFINED = 0,
    VALUE_TYPE_FLOAT,
} value_type_et;


typedef struct
{
    Adafruit_MQTT_Publish *ada_mqtt_publ;
    char          mac[ MAC_ADDR_LEN];
    char          caption[CAPTION_LEN];
    ruuvi_tag_st  *ruuvi_ptr;
    bool          updated;
    unit_et       unit_type;
    uint32_t      publ_interval_ms;
    uint32_t      publ_next_ms;
} ruuvi_publ_st;

void task_connect_mqtt( void *pvParameters );

#endif