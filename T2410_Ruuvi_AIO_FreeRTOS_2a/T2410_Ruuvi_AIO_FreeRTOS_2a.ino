/**
 * RuuviTag Adafruit IO Gateway
 * @author TomHöglund 2021 2024
 * 
 * Git:  https://github.com/infrapale/T2410_Ruuvi_AIO_Gateway
 * 
 *      Start WiFI
 *          |
 *          V
 *      Scan BLE and
 *      collect defined <-------
 *      Ruuvi data             |
 *          |                  |
 *          V                  |
 *      Publish data to AIO   --
 *
 *
 *
 * L I N K S
 * https://github.com/ruuvi/ruuvi-sensor-protocols/blob/master/dataformat_03.md
 * https://tutorial.cytron.io/2020/01/15/send-sensors-data-to-adafruit-io-using-esp32/
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html
 * https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/Adafruit_MQTT.h
 * 
 */

#include "main.h"
#include "FreeRTOSConfig.h"
#include "wifi_task.h"
#include "ruuvi_tag.h"
#include "ruuvi_ble.h"
#include "mqtt_task.h"


TaskHandle_t TaskRuuviScan;
TaskHandle_t TaskWiFi;
TaskHandle_t TaskConnectMqtt;


const int   led_1 = 13;
const int   led_2 = 17;

RuuviTag ruuvi_tag;

main_ctrl_st main_ctrl = { 
  .wifi_is_connected = false,
  .radio_is_available = false,
  .mqtt_is_connected = false,
  .ble_is_scanning = false,
};


void setup() {

  Serial.begin(115200); 
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);

   
  xTaskCreatePinnedToCore(ruuvi_scan_task,"Scan",10000,NULL,1,&TaskRuuviScan,1);          
  delay(500); 
  xTaskCreatePinnedToCore(wifi_task_code,"WiFi",10000,NULL,1,&TaskWiFi,1);          
  delay(500); 
  xTaskCreatePinnedToCore(task_connect_mqtt,"MQTT",10000,NULL,1,&TaskConnectMqtt,1);          
  delay(500); 
  printf("Free Heap: %d \n",xPortGetFreeHeapSize());
}

void loop() 
{
  
}

