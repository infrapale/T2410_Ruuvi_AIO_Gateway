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
#include <esp_task_wdt.h>
#include "wifi_task.h"
#include "ruuvi_tag.h"
#include "ruuvi_ble.h"
#include "mqtt_task.h"
#include "helpers.h"
#include "supervisor.h"


TaskHandle_t TaskRuuviScan;
TaskHandle_t TaskWiFi;
TaskHandle_t TaskConnectMqtt;
TaskHandle_t TaskSupervisor;


const int  WDT_TIMEOUT_SEC = 5;
const int  led_1 = 13;
const int  led_2 = 17;

extern super_st super[];

RuuviTag ruuvi_tag;

main_ctrl_st main_ctrl = { 
  .wifi_is_connected = false,
  .radio_is_available = false,
  .mqtt_is_connected = false,
  .ble_is_scanning = false,
};


// typedef struct {
//     uint32_t timeout_ms;        /**< TWDT timeout duration in milliseconds */
//     uint32_t idle_core_mask;    /**< Mask of the cores who's idle task should be subscribed on initialization */
//     bool trigger_panic;         /**< Trigger panic when timeout occurs */
// } esp_task_wdt_config_t;


const esp_task_wdt_config_t wdt_config = { 5000, 0x00000000, true};

void setup() {

  Serial.begin(115200); 
  io_initialize();
   
  xTaskCreatePinnedToCore(ruuvi_scan_task,"Scan",10000,NULL,1,&TaskRuuviScan,1);          
  delay(500); 
  xTaskCreatePinnedToCore(wifi_task_code,"WiFi",10000,NULL,1,&TaskWiFi,1);          
  delay(500); 
  xTaskCreatePinnedToCore(task_connect_mqtt,"MQTT",10000,NULL,1,&TaskConnectMqtt,1);          
  delay(500); 
  xTaskCreatePinnedToCore(super_task_code,"Super",10000,NULL,1,&TaskSupervisor,1);          
  delay(500); 
  printf("Free Heap: %d \n",xPortGetFreeHeapSize());

   if (io_wd_is_enabled())
    {
      Serial.printf("Set WD timeout = %d seconds\n", WDT_TIMEOUT_SEC);
      esp_task_wdt_init(&wdt_config); 
      esp_task_wdt_add(NULL); //add current thread to WDT watch
      esp_task_wdt_add(TaskSupervisor);
    }
}

void loop() 
{
  esp_task_wdt_reset();
  vTaskDelay(1000);
}

