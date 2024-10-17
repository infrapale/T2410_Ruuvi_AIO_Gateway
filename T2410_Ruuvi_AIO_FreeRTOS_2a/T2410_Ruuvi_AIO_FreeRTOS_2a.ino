/**
 * RuuviTag Adafruit IO Gateway
 * @author TomHöglund 2021 2024
 * 
 * https://github.com/ruuvi/ruuvi-sensor-protocols/blob/master/dataformat_03.md
 * https://tutorial.cytron.io/2020/01/15/send-sensors-data-to-adafruit-io-using-esp32/
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/index.html
 * https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/Adafruit_MQTT.h
 * 
 */
 

// #include <WiFi.h>
#include "main.h"
#include "FreeRTOSConfig.h"
#include "wifi_task.h"
#include "ruuvi_tag.h"
#include "ruuvi_ble.h"
#include "mqtt_task.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>


TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t TaskRuuviScan;
TaskHandle_t TaskWiFi;
TaskHandle_t TaskConnectMqtt;
SemaphoreHandle_t sema_radio = NULL;

const int   led_1 = 13;
const int   led_2 = 17;

// BLEDevice     ble_device;
// BLEScan       *pBLEScan;

RuuviTag ruuvi_tag;

main_ctrl_st main_ctrl = { 
  .wifi_is_connected = false,
  .radio_is_available = false,
  .mqtt_is_connected = false,
  .ble_is_scanning = false,
};

void ruuvi_sensor_initialize(void);

void setup() {
  BaseType_t rc;

  Serial.begin(115200); 
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);

  sema_radio = xSemaphoreCreateMutex();
  if (sema_radio = NULL)  { printf("Semaphore failure!!\n"); while(true); }
  else printf("sema_radio is OK\n");

  // rc = xSemaphoreTake(sema_radio, portMAX_DELAY);
  // Serial.printf("Semaphore sema_radio = %d\n", rc);

  ruuvi_sensor_initialize();
  // rc = xSemaphoreGive(sema_radio);
  
  SemaphoreHandle_t sema_radio;

  // xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);                         
  // delay(500); 

  // xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          
  // delay(500); 

  xTaskCreatePinnedToCore(ruuvi_scan_task,"Scan",10000,NULL,1,&TaskRuuviScan,1);          
  delay(500); 
  
  xTaskCreatePinnedToCore(wifi_task_code,"WiFi",10000,NULL,1,&TaskWiFi,1);          
  delay(500); 

  xTaskCreatePinnedToCore(task_connect_mqtt,"MQTT",10000,NULL,1,&TaskConnectMqtt,1);          
  delay(500); 

  printf("Free Heap: %d \n",xPortGetFreeHeapSize());

}

void Task1code( void * parameter ){
  Serial.print("Task1 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led_1, HIGH);
    //Serial.print("#");
    vTaskDelay(500);
    digitalWrite(led_1, LOW);
    vTaskDelay(500);
  } 
}

void Task2code( void * parameter ){
  Serial.print("Task2 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led_2, HIGH);
    //Serial.print("@");
    vTaskDelay(1500);
    digitalWrite(led_2, LOW);
    vTaskDelay(1500);
  }
}

void loop() {
  
}


void ruuvi_sensor_initialize(void)
{
    /// Define ruuvi tag data
    // ruuvi_tag.add("e6:2c:8d:db:22:35","Jaakaappi yla");
    // ruuvi_tag.add("ea:78:e2:12:36:f8","Jaakaappi keski");
    // ruuvi_tag.add("ed:9a:ab:c6:30:72","Jaakaappi vihannes");    

}

