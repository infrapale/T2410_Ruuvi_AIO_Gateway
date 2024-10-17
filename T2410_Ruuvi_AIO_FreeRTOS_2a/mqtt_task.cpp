#include <WiFi.h>
#include  "main.h"
#include "mqtt_task.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "ruuvi_tag.h"
#include "secrets.h"

#define MQTT_UPDATE_INTERVAL_ms   60000   ///< MQTT update interval, one value per time


extern main_ctrl_st main_ctrl;
extern WiFiClient client;
extern RuuviTag ruuvi_tag;
// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.

const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);


Adafruit_MQTT_Client aio_mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish ruuvi_e6_temp  = Adafruit_MQTT_Publish(&aio_mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-e6-temp");
Adafruit_MQTT_Publish ruuvi_ea_temp  = Adafruit_MQTT_Publish(&aio_mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-temp");
Adafruit_MQTT_Publish ruuvi_ed_temp  = Adafruit_MQTT_Publish(&aio_mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ed-temp");

ruuvi_publ_st ruuvi_publ[AIO_PUBL_NBR_OF ] =
{
    { &ruuvi_e6_temp, "e6:2c:8d:db:22:35", "Ruuvi E6 Temp", NULL, false, UNIT_TEMPERATURE, 60000,0}, 
    { &ruuvi_ea_temp, "ea:78:e2:12:36:f8", "Ruuvi EA Temp", NULL, false, UNIT_TEMPERATURE, 60000,0}, 
    { &ruuvi_ed_temp, "ed:9a:ab:c6:30:72", "Ruuvi ED Temp", NULL, false, UNIT_TEMPERATURE, 60000,0}, 
};

Adafruit_MQTT_Publish *aio_publ[AIO_PUBL_NBR_OF] =
{
    &ruuvi_e6_temp,
    &ruuvi_ea_temp,
    &ruuvi_ed_temp
};




void task_connect_mqtt( void *pvParameters )
{
    uint8_t       state;
    int8_t        ret;
    uint8_t       retries = 3;
    BaseType_t    rc;
    esp_err_t     er;
    uint8_t       wifi_timeout;
    uint8_t       mqtt_timeout;
    uint8_t       sensor_indx = 0;
    uint8_t       interval_sec;
    ruuvi_tag_st  *rptr;
    
    state = 0;
    for (uint8_t pindx = 0; pindx < AIO_PUBL_NBR_OF; pindx++)
    {
       ruuvi_tag.add_if_new(ruuvi_publ[pindx].mac, ruuvi_publ[pindx].caption);
    }
    
    for (uint8_t pindx = 0; pindx < AIO_PUBL_NBR_OF; pindx++)
    {
        ruuvi_publ[pindx].ruuvi_ptr = ruuvi_tag.get_data_ptr(String (ruuvi_publ[pindx].mac));
    }


    // er = esp_task_wdt_add(nullptr);
    // assert(er == ESP_OK);
    // mqtt.subscribe(&at_home_state);
    
    for (;;)
    { 
        printf("MQTT state: %d\n", state);
        switch(state) {
            case 0: // initial
                sensor_indx  = 0;
                state++;
                break;
            case 1: //Re-run MQTT action
                wifi_timeout = 10;
                mqtt_timeout = 10;
                state++;
                break;
                
            case 2: // waiting for WiFi
                if (main_ctrl.wifi_is_connected && main_ctrl.radio_is_available)
                {
                    main_ctrl.radio_is_available = false;
                    state = 10;
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
                rptr = ruuvi_publ[sensor_indx].ruuvi_ptr;
                printf("MQTT Publish: %s %f\n", ruuvi_publ[sensor_indx].caption,  rptr->temp_fp);
                ruuvi_publ[sensor_indx].ada_mqtt_publ->publish(rptr->temp_fp);      
                if(++sensor_indx >= NBR_SENSORS) sensor_indx = 0;         
                vTaskDelay(2000);
                state = 30;
                break;    
            case 30:
                mqtt.disconnect(); 
                main_ctrl.radio_is_available = true;
                vTaskDelay(20000);
                state = 2;
                break;    
/*
            case 3: // WiFi is available
                if (mqtt.connected())
                {
                    printf("MQTT was already connected\n");
                    state++;
                }
                else 
                {                
                    printf("Connecting to MQTT…\n ");  
                    if (WiFi.status() != WL_CONNECTED)
                    {
                        printf("WiFi is not connected\n ");  
                        state = 99;  //restart
                    }
                    else
                    {
                        ret = mqtt.connect();
                        if (ret != 0) {    // connect will return 0 for connected
                            printf("%s\n",mqtt.connectErrorString(ret));
                            printf("Retrying MQTT connection…\n");
                            mqtt.disconnect();          
                            if (--mqtt_timeout == 0) state = 6;
                        }
                        else {
                            // esp_task_wdt_reset();
                            vTaskDelay(100);
                            state++;
                        }
                    }
                }
                break;
            case 4: // MQTT is connected
                printf("MQTT is Connected!\n"); 
                main_ctrl.mqtt_is_connected = true;
                //rc = xSemaphoreGive(sema_mqtt_avail);
                //esp_task_wdt_reset();
                state++;
                vTaskDelay(100);
                break;
            case 5: // MQTT actions
                if (mqtt.connected())
                {
                    rptr = ruuvi_publ[sensor_indx].ruuvi_ptr;
                    
                    {
                        printf("MQTT Publish: %s %f\n", ruuvi_publ[sensor_indx].caption,  rptr->temp_fp);
                        // sensor[sensor_indx].ada_mqtt_publ->publish(*sensor[sensor_indx].data_ptr);      
                    }
                    if(++sensor_indx >= NBR_SENSORS) sensor_indx = 0;
                    state++;
                    interval_sec = 15;

                } else 
                {
                    interval_sec = 60;
                    state = 99;
                }
                //esp_task_wdt_reset();
                vTaskDelay(1000);
                break;

            case 6: 
                //rc = xSemaphoreGive(sema_wifi_avail);
                state++;
                break;

            case 7: //Wait for next MQTT update
                if(--interval_sec == 0) state = 1;
                //mqtt.processPackets(1000);
                //esp_task_wdt_reset();
                vTaskDelay(1000);          
                break;

            case 99: //
                printf("Retry limit reached -> WDT reset…\n");
                //rc = xSemaphoreGive(sema_wifi_avail);
                //rc = xSemaphoreGive(sema_mqtt_avail);
                vTaskDelay(10000);
                break;
*/                
            default:
                printf("Fatal error: incorrect MQTT state -> WDT reset…%d\n",state);
                vTaskDelay(100);
                state = 1;
                break;           
        }
      //vTaskDelay(1000);
    }
}

