#include <WiFi.h>
#include "main.h"
#include "secrets.h"

const char* ssid     = WIFI_SSID;            //Main Router      
const char* password = WIFI_PASS;            //Main Router Password

WiFiClient client;
extern main_ctrl_st main_ctrl;
extern SemaphoreHandle_t sema_radio;


void wifi_task_code( void *pvParameters ){
    uint8_t state = 0;
    uint8_t retries = 6;

    for (;;)
    {
        printf("WiFi state: %d\n", state);
        switch(state)
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
                    digitalWrite(PIN_LED_WHITE, LOW);
                    vTaskDelay(1000);
                    if (--retries == 0) state = 3;
                    else Serial.println("Waiting for WiFi"); 
                }
                else {
                    digitalWrite(PIN_LED_WHITE, HIGH);
                    Serial.println("Connected to WiFi");
                    main_ctrl.wifi_is_connected = true;
                    main_ctrl.radio_is_available = true;
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
                digitalWrite(PIN_LED_RED, HIGH);
                vTaskDelay(1000);  //        while (1); 
                break;
        }
    }
}
