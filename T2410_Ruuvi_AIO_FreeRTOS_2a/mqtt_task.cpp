#include <WiFi.h>
#include  "main.h"
#include "mqtt_task.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "ruuvi_tag.h"
#include "secrets.h"


extern main_ctrl_st main_ctrl;
extern WiFiClient client;
extern RuuviTag ruuvi_tag;
// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.

const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish ruuvi_e6_temp  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-e6-temp");
Adafruit_MQTT_Publish ruuvi_ea_temp  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-temp");
Adafruit_MQTT_Publish ruuvi_ea_rssi  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-rssi");
Adafruit_MQTT_Publish ruuvi_ea_bat   = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-bat");
Adafruit_MQTT_Publish ruuvi_ed_temp  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ed-temp");

ruuvi_publ_st ruuvi_publ[AIO_PUBL_NBR_OF ] =
{
    { &ruuvi_e6_temp, "e6:2c:8d:db:22:35", "Ruuvi E6 Temp", NULL, false, UNIT_TEMPERATURE, 60000,   0}, 
    { &ruuvi_ea_temp, "ea:78:e2:12:36:f8", "Ruuvi EA Temp", NULL, false, UNIT_TEMPERATURE, 60000,   0}, 
    { &ruuvi_ea_rssi, "ea:78:e2:12:36:f8", "Ruuvi EA RSSI", NULL, false, UNIT_RSSI,        300000,  0}, 
    { &ruuvi_ea_bat,  "ea:78:e2:12:36:f8", "Ruuvi EA Bat ", NULL, false, UNIT_VOLTAGE,     60000,   0}, 
    { &ruuvi_ed_temp, "ed:9a:ab:c6:30:72", "Ruuvi ED Temp", NULL, false, UNIT_TEMPERATURE, 60000,   0}, 
};


void task_connect_mqtt( void *pvParameters )
{
    uint8_t       state;
    int8_t        ret;
    uint8_t       sensor_indx = 0;
    ruuvi_tag_st  *rptr;
    float         fval;
    
    state = 0;
    for (uint8_t pindx = 0; pindx < AIO_PUBL_NBR_OF; pindx++)
    {
       ruuvi_tag.add_if_new(ruuvi_publ[pindx].mac, ruuvi_publ[pindx].caption);
    }
    
    for (uint8_t pindx = 0; pindx < AIO_PUBL_NBR_OF; pindx++)
    {
        ruuvi_publ[pindx].ruuvi_ptr = ruuvi_tag.get_data_ptr(String (ruuvi_publ[pindx].mac));
    }

    
    for (;;)
    { 
        printf("MQTT state: %d\n", state);
        switch(state) {
            case 0: // initial
                sensor_indx  = 0;
                state++;
                break;
            case 1: //Re-run MQTT action
                state++;
                break;
                
            case 2: // waiting for WiFi
                if (main_ctrl.wifi_is_connected && main_ctrl.radio_is_available)
                {
                    main_ctrl.radio_is_available = false;
                    state = 10;
                    digitalWrite(PIN_LED_YELLOW, HIGH);
                }                  
                vTaskDelay(1000);
                break;


            case 10:
                if (mqtt.connected())
                {
                    printf("MQTT was already connected\n");
                    main_ctrl.radio_is_available = false;
                    state = 20;
                }
                else 
                {
                    ret = mqtt.connect();              
                    if (ret != 0) {    // connect will return 0 for connected
                        printf("%s\nRetrying MQTT connection…\n",mqtt.connectErrorString(ret));
                        mqtt.disconnect();
                        main_ctrl.radio_is_available = true;
                        digitalWrite(PIN_LED_YELLOW, LOW);
                        state = 2;
                    }
                    else
                    {
                        main_ctrl.radio_is_available = false;
                        state = 20;
                    }
                }
                vTaskDelay(100);
                break;    
            case 20:  // MQTT is connected
                if (millis() >ruuvi_publ[sensor_indx].publ_next_ms)
                {
                    ruuvi_publ[sensor_indx].publ_next_ms = millis() + ruuvi_publ[sensor_indx].publ_interval_ms;
                    rptr = ruuvi_publ[sensor_indx].ruuvi_ptr;
                    switch(ruuvi_publ[sensor_indx].unit_type)
                    {
                        case UNIT_TEMPERATURE:
                            fval =  rptr->temp_fp;
                            break;
                        case UNIT_HUMIDITY:
                            fval = rptr->humidity;
                            break;
                        case UNIT_AIR_PRESSURE:
                            fval = (float)rptr->pressure;
                            break;
                        case UNIT_LIGHT:
                            fval = 0.0;
                            break;
                        case UNIT_LDR:
                            fval = 0.0;
                            break;
                        case UNIT_RSSI:
                            fval = (float)rptr->rssi;
                            break;
                        case UNIT_VOLTAGE:
                            fval = rptr->voltage;
                            break;


                    }
                    printf("MQTT Publish: %s %f\n", ruuvi_publ[sensor_indx].caption,  fval);
                    ruuvi_publ[sensor_indx].ada_mqtt_publ->publish(fval);      
                    vTaskDelay(5000);
                }
                else
                {
                    vTaskDelay(1000);
                }
                if(++sensor_indx >=  AIO_PUBL_NBR_OF) sensor_indx = 0;         
                state = 30;
                break;    
            case 30:
                mqtt.disconnect(); 
                main_ctrl.radio_is_available = true;
                digitalWrite(PIN_LED_YELLOW, LOW);
                vTaskDelay(1000);
                state = 2;
                break;    

            default:
                printf("Fatal error: incorrect MQTT state -> WDT reset…%d\n",state);
                vTaskDelay(100);
                state = 1;
                break;           
        }
      //vTaskDelay(100);
    }
}

