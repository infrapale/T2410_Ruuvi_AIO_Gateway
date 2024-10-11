#ifndef __MAIN_H__
#define __MAIN_H__

#define NBR_SENSORS               4       ///< Number of sensor values
#define BLE_VERBOSE               false

#define NBR_SENSORS               3       ///< Number of sensor values
#define CAPTION_LEN               40      ///< Length of value name
#define MAC_ADDR_LEN              18      ///< Length of the BLE MAC address string
#define MQTT_UPDATE_INTERVAL_ms   60000   ///< MQTT update interval, one value per time
#define PRINT_INTERVAL            10000
#define BLE_SCAN_INTERVAL         20000
#define BLE_SCAN_TIME_SEC         5
#define WDT_TIMEOUT               10



/// SSID Definitions
// #define  VILLA_ASTRID
// #define  H_MOKKULA
#define PIRPANA
#include "secrets.h"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    IO_USERNAME
#define AIO_KEY         IO_KEY
#define AIO_PUBLISH_INTERVAL_ms  60000


typedef struct 
{
    uint16_t  state;
    uint32_t  next_ble_scan_ms;
    uint32_t  next_print_ms;
    uint8_t   sensor_indx;
} main_ctrl_st;

#endif