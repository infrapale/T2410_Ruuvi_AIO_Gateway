/**

  https://github.com/adafruit/Adafruit_MQTT_Library
  https://io.adafruit.com/api/docs/mqtt.html#adafruit-io-mqtt-api

 */
#include <Arduino.h>
#include "main.h"
#include <stdint.h>
#include "stdio.h"
// #include "hardware/uart.h"
#include "secrets.h"
#include <WiFi.h>
#include "wifi_sm.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "aio_mqtt.h"
#include "atask.h"


typedef struct 
{
  int8_t      connected;
  // aio_subs_et subs_indx;
  uint16_t    conn_faults;
  uint8_t     at_home;

} aio_mqtt_ctrl_st;


aio_mqtt_ctrl_st aio_mqtt_ctrl =
{
  .connected = false,
  .conn_faults = 0,
  .at_home = 0,
};


// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;
const char PHOTOCELL_FEED[] PROGMEM = AIO_USERNAME "/feeds/photocell";
const char AIO_FEED_TRE_TEMP_ID[] PROGMEM = AIO_USERNAME "/feeds/home-tampere.tampere-indoor-temperature";


// RTC_PCF8563 rtc;
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish ruuvi_e6_temp  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-e6-temp");
Adafruit_MQTT_Publish ruuvi_ea_temp  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-temp");
Adafruit_MQTT_Publish ruuvi_ed_temp  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ed-temp");

ruuvi_publ_st ruuvi_publ[AIO_PUBL_NBR_OF ] =
{
    { &ruuvi_e6_temp, "E6:2C:8D:DB:22:35", "Ruuvi E6 Temp", NULL, false, UNIT_TEMPERATURE, 60000,0}, 
    { &ruuvi_ea_temp, "EA:78:E2:12:36:F8", "Ruuvi EA Temp", NULL, false, UNIT_TEMPERATURE, 60000,0}, 
    { &ruuvi_ed_temp, "ED:9A:AB:C6:30:72", "Ruuvi ED Temp", NULL, false, UNIT_TEMPERATURE, 60000,0}, 
};

Adafruit_MQTT_Publish *aio_publ[AIO_PUBL_NBR_OF] =
{
    &ruuvi_e6_temp,
    &ruuvi_ea_temp,
    &ruuvi_ed_temp
};

void dummy_mqtt_cb(void){}
void cb_time_feed(uint32_t feed_time);


extern Adafruit_MQTT_Subscribe *aio_subs[];
extern Adafruit_MQTT_Publish *aio_publ[];
extern Adafruit_MQTT_Client aio_mqtt;
extern char zone_main_label[NBR_MAIN_ZONES][MAIN_ZONE_LABEL_LEN];
extern char unit_label[NBR_UNITS][UNIT_LABEL_LEN];
extern char measure_label[NBR_UNITS][MEASURE_LABEL_LEN];


//                                  123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st aio_mqtt_task        =   {"AIO MQTT SM    ", 1000,   0,     0,  255,    0,   0, aio_mqtt_stm };


Adafruit_MQTT_Subscribe *aio_subscription;

void aio_mqtt_initialize(void)
{
    //aio_mqtt_ctrl.aindx = atask_add_new(&aio_mqtt_task);   // do not actually run as a task
    aio_mqtt_task.state = 0;
    aio_mqtt_task.prev_state = 255;
}



int8_t aio_mqtt_connect() {
    Serial.print(F("Connecting to Adafruit IO… "));
    int8_t ret;
    if ((ret = aio_mqtt.connect()) != 0) 
    {
        switch (ret) 
        {
            case 1: Serial.println(F("Wrong protocol")); break;
            case 2: Serial.println(F("ID rejected")); break;
            case 3: Serial.println(F("Server unavail")); break;
            case 4: Serial.println(F("Bad user/pass")); break;
            case 5: Serial.println(F("Not authed")); break;
            case 6: Serial.println(F("Failed to subscribe")); break;
            default: Serial.println(F("Connection failed")); break;
        }

        if(ret >= 0) aio_mqtt.disconnect();
        Serial.println(F("Retrying connection…"));
        aio_mqtt_ctrl.conn_faults++;
        // Watchdog.reset();
    }
    else 
    {
      Serial.println(F("Adafruit IO Connected!"));
    }
    
    return ret;
}


void cb_dummy(double tmp) {}


void aio_mqtt_stm(void)
{
    if ( aio_mqtt_task.prev_state != aio_mqtt_task.state)
    {
        Serial.print(F("aio_mqtt_stm state= "));
        Serial.print(aio_mqtt_task.prev_state);
        Serial.print(F(" --> "));
        Serial.println(aio_mqtt_task.state);
        aio_mqtt_task.prev_state = aio_mqtt_task.state;
    }

    switch(aio_mqtt_task.state)
    {
        case 0:
          aio_mqtt_task.state = 10;
          break;
        case 10:
            WiFi.begin(WIFI_SSID, WIFI_PASS);
            Serial.println(F("setup - wifi begin .. "));
            if (WiFi.status() != WL_CONNECTED)  Serial.print(F("."));
            else 
            {
              Serial.println(F("wifi connected"));
              aio_mqtt_task.state = 20;
            }
            break;
        case 20:
            aio_mqtt_task.state = 30;
            break;
        case 30:





            aio_mqtt_task.state = 40;
            break;
        case 40:
            aio_mqtt_ctrl.connected =  aio_mqtt_connect();
            if (aio_mqtt_ctrl.connected == 0) 
            {
              aio_mqtt_task.state = 50;
              aio_mqtt_ctrl.conn_faults = 0;
            }
            break;
        case 50:
            aio_mqtt.processPackets(10000);
            aio_mqtt_task.state = 55;
            break;
        case 55:
            if(! aio_mqtt.ping()) 
            {
              aio_mqtt.disconnect();
              aio_mqtt_task.state = 10;
            }
            break;
        case 60:
            aio_mqtt_task.state = 100;
            break;
        case 100:
            aio_mqtt_ctrl.connected =  aio_mqtt_connect();
            if (aio_mqtt_ctrl.connected == 0) 
            {
              aio_mqtt_task.state = 105;
            }
            break;
        case 105:
            aio_mqtt_task.state = 110;
            aio_mqtt_ctrl.conn_faults = 0;
            break;
        case 110:
            aio_mqtt.processPackets(10000);
            aio_mqtt_task.state = 120;
            break;
        case 120:
            if(! aio_mqtt.ping()) 
            {
              aio_mqtt.disconnect();
              aio_mqtt_task.state = 100;
            }
            else 
            {
              aio_mqtt_task.state = 110;
            }
            break;
    }
}
