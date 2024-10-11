#ifndef __AIO_MQTT_H__
#define __AIO_MQTT_H__

#include "main.h"
#include "aio_mqtt.h"
#include "RuuviTag.h"

#define NBR_MAIN_ZONES      4
#define NBR_SUB_ZONES       20
#define NBR_UNITS           5
#define MAIN_ZONE_LABEL_LEN 16
#define SUB_ZONE_LABEL_LEN  20
#define UNIT_LABEL_LEN      10
#define MEASURE_LABEL_LEN   16
#define AIO_PUBL_NBR_OF     3

typedef void (*mqtt_cb)(void);

typedef enum
{
    ZONE_VILLA_ASTRID = 0,
    ZONE_LILLA_ASTRID,
    ZONE_LAITURI,
    ZONE_TAMPERE
} main_zone_et;

typedef enum
{
    UNIT_TEMPERATURE = 0,
    UNIT_HUMIDITY,
    UNIT_AIR_PRESSURE,
    UNIT_LIGHT,
    UNIT_LDR
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



// typedef struct
// {
//   main_zone_et  main_zone_index;
//   char          sub_zone[SUB_ZONE_LABEL_LEN];
//   unit_et       unit_index;
//   float         value;
//   bool          active;
//   bool          updated;
//   uint32_t      publ_interval_ms;
//   uint32_t      publ_next_ms;
//   // mqtt_cb       cb;
// } value_st;



void aio_mqtt_initialize(void);

void aio_mqtt_stm();

bool aio_mqtt_is_updated(uint8_t sindx);


#endif