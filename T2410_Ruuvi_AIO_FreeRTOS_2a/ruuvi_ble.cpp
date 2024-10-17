
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "ruuvi_tag.h"
#include "main.h"
#include "helpers.h"

extern main_ctrl_st main_ctrl;
extern RuuviTag ruuvi_tag;
extern SemaphoreHandle_t sema_radio;


BLEDevice     ble_device;
BLEScan       *pBLEScan;


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks 
{
  void onResult(BLEAdvertisedDevice advertisedDevice) 
  {
      bool is_a_ruuvi = false;
      char dev_name[40];
      String DevName;
      if (BLE_VERBOSE) Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      String mac_addr = advertisedDevice.getAddress().toString().c_str();
      is_a_ruuvi = ruuvi_tag.is_a_defined_ruuvi(mac_addr);

      if ((advertisedDevice.haveManufacturerData() == true) && is_a_ruuvi)
      {
          String strManufacturerData = advertisedDevice.getManufacturerData();

          uint8_t cManufacturerData[100];
          memcpy(cManufacturerData, strManufacturerData.c_str(), strManufacturerData.length());
          String RawData = "";
          for (int i = 0; i < strManufacturerData.length(); i++) 
          {
            // Serial.printf("[%02X]", cManufacturerData[i]);
            RawData +=  u8_to_hex(cManufacturerData[i]);
          }
          if (BLE_VERBOSE)
          {
              Serial.print("(|==== Ruuvi ");
              Serial.print(mac_addr);
              Serial.print(" : ");
              Serial.printf("Length & data: %d ", strManufacturerData.length());
              Serial.print(RawData);
              Serial.printf("\n");
 
          }
          ruuvi_tag.decode_raw_data(mac_addr, RawData, advertisedDevice.getRSSI());
        }
    }
};


void ruuvi_ble_initialize(void)
{
    //BLEDevice::init("");
    ble_device.init("");
    //pBLEScan = BLEDevice::getScan();  //create new scan
    pBLEScan = ble_device.getScan();  //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value
}

void ruuvi_scan_task( void *pvParameters)
{
    static uint8_t state = 0;
    BaseType_t rc;
    esp_err_t er;
    BLEScanResults *foundDevices;
    ruuvi_ble_initialize();

    for (;;)
    {
        printf("Ruuvi BLE state: %d\n", state);
        switch(state )
        {
            case 0:
                state = 10;
                break;
            case 10:
                // rc = xSemaphoreTake(sema_radio, (TickType_t) 0 );
                // if (rc == pdPASS) state= 30;
                if (main_ctrl.radio_is_available)
                {
                    main_ctrl.radio_is_available = false; 
                    state = 30;
                }
                vTaskDelay(1000);
                break;
            case 30:
                foundDevices = pBLEScan->start(BLE_SCAN_TIME_SEC, false);
                if (BLE_VERBOSE) Serial.printf("Devices found: %d \n", foundDevices->getCount());
                pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
                vTaskDelay(1000);
                state = 40;
                break;
            case 40:
                main_ctrl.radio_is_available = true;
                vTaskDelay(10000);
                state = 10;
                break;
        }
    }
}


