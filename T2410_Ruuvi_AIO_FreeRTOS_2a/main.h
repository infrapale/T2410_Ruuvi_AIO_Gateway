#ifndef __MAIN_H__
#define __MAIN_H__

/// SSID Definitions
#define   VILLA_ASTRID
//#define   H_MOKKULA
//#define   PIRPANA
//#define   LILLA_ASTRID

#define CAPTION_LEN               40      ///< Length of value name
#define MAC_ADDR_LEN              18      ///< Length of the BLE MAC address string
#define BLE_VERBOSE               true
#define BLE_SCAN_TIME_SEC         5

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    IO_USERNAME
#define AIO_KEY         IO_KEY
#define AIO_PUBLISH_INTERVAL_ms  60000

#define PIN_LED_WHITE     33
#define PIN_LED_YELLOW    25
#define PIN_LED_BLUE      26 
#define PIN_LED_RED       27
#define PIN_WD_DISABLE    24

typedef struct
{
    bool  wifi_is_connected;
    bool  radio_is_available;
    bool  mqtt_is_connected;
    bool  ble_is_scanning;
} main_ctrl_st;

#endif