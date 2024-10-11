#include "Arduino.h"
#include "main.h"
#include "atask.h"
#include <WiFi.h>
#include "wifi_sm.h"


typedef struct
{
    bool is_connected;
} wifi_sm_st;

wifi_sm_st wifi_sm;

const char* ssid     = WIFI_SSID;            //Main Router      
const char* password = WIFI_PASS;            //Main Router Password



//                                          123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st wifi_task_handle        =        {"WiFI SM       ", 1000,   0,     0,  255,    0,   1,  wifi_sm_task };


void wifi_sm_initialize(void)
{
    wifi_sm.is_connected = false;
}



void wifi_sm_task( void){
    uint8_t state;
    int8_t  ret;
    uint8_t retries = 6;
    BaseType_t rc;
    esp_err_t er;

    state = 0;
    // er = esp_task_wdt_add(nullptr);
    // assert(er == ESP_OK);
    
    for (;;)
    {
        //printf("WiFi state: %d\n", state);
        switch(wifi_task_handle.state )
        {
            case 0:   // initial
                if (WiFi.status() != WL_CONNECTED){
                    Serial.println("Connecting WiFi");       
                    WiFi.begin(ssid, password); 
                    retries = 6;
                    state++;
                }
                else state = 2;
                vTaskDelay(1000);
                break;
            case 1:   // Check for the connection
                if (WiFi.status() != WL_CONNECTED) {
                    //digitalWrite(LED_YELLOW,LOW);
                    vTaskDelay(1000);
                    if (--retries == 0) state = 3;
                    else Serial.println("Waiting for WiFi"); 
                }
                else {
                    // digitalWrite(LED_YELLOW,HIGH);
                    Serial.println("Connected to WiFi");
                    state = 2;
                }             
                //esp_task_wdt_reset();
                vTaskDelay(1000);
                break;
            case 2:   // 
                if (WiFi.status() != WL_CONNECTED) state = 0;
                //esp_task_wdt_reset();
                vTaskDelay(1000);
                break;
            case 3:   // 
                Serial.println("WiFi Retry limit reached -> WDT reset…");
                vTaskDelay(1000);  //        while (1); 
                break;
        }
    }
}

