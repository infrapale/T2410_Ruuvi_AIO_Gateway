/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>
#include "main.h"


#include "RuuviTag.h"
// #include "config.h"
#include "helpers.h"

#define CAPTION_LEN               40      ///< Length of value name
#define MAC_ADDR_LEN              18      ///< Length of the BLE MAC address string
#define MQTT_UPDATE_INTERVAL_ms   60000   ///< MQTT update interval, one value per time

#define WDT_TIMEOUT           10

RuuviTag  ruuvi_tag;

uint16_t scan_time = 5;  //In seconds
BLEScan *pBLEScan;

uint8_t pl[255];

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

          {
              String RawData = "";
              Serial.print("(|==== Ruuvi ");
              Serial.print(mac_addr);
              Serial.print(" : ");
              Serial.printf("Length & data: %d ", strManufacturerData.length());
              for (int i = 0; i < strManufacturerData.length(); i++) {
                // Serial.printf("[%02X]", cManufacturerData[i]);
                RawData +=  u8_to_hex(cManufacturerData[i]);
              }
              Serial.print(RawData);
              Serial.printf("\n");
 
              ruuvi_tag.decode_raw_data(mac_addr, RawData, advertisedDevice.getRSSI());
          }
        }
    }
};


void setup() 
{
    Serial.begin(115200);
    Serial.println("Scanning...");

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();  //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value

    /// Refine ruuvi tag data
    ruuvi_tag.add(String("e6:2c:8d:db:22:35"),"Jaakaappi yla");
    ruuvi_tag.add(String("ea:78:e2:12:36:f8"),"Jaakaappi keski");
    ruuvi_tag.add(String("ed:9a:ab:c6:30:72"),"Jaakaappi vihannes");

    /// Refine sensors
    // strncpy(sensor[0].caption,ruuvi_tag.ruuvi[0].location,CAPTION_LEN-RUUVI_LOCATION_LEN);
    // strcat(sensor[0].caption, "_temperature");

    // strncpy(sensor[1].caption,ruuvi_tag.ruuvi[1].location,CAPTION_LEN-RUUVI_LOCATION_LEN);
    // strcat(sensor[1].caption, "_temperature");
    
    // strncpy(sensor[2].caption,ruuvi_tag.ruuvi[2].location,CAPTION_LEN-RUUVI_LOCATION_LEN);
    // strcat(sensor[2].caption, "_temperature");
    
    // strncpy(sensor[3].caption,ruuvi_tag.ruuvi[0].location,CAPTION_LEN-RUUVI_LOCATION_LEN);
    // strcat(sensor[3].caption, "_humidity");
    
    // sensor[0].data_ptr = &ruuvi_tag.ruuvi[0].temp_fp;
    // sensor[1].data_ptr = &ruuvi_tag.ruuvi[1].temp_fp;
    // sensor[2].data_ptr = &ruuvi_tag.ruuvi[2].temp_fp;
    // sensor[3].data_ptr = &ruuvi_tag.ruuvi[0].humidity;

    // sensor[0].updated_ptr = &ruuvi_tag.ruuvi[0].updated;
    // sensor[1].updated_ptr = &ruuvi_tag.ruuvi[1].updated;
    // sensor[2].updated_ptr = &ruuvi_tag.ruuvi[2].updated;
    // sensor[3].updated_ptr = &ruuvi_tag.ruuvi[0].updated;
      

    
}

void loop() 
{
    BLEScanResults *foundDevices = pBLEScan->start(scan_time, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices->getCount());
    Serial.println("Scan done!");
    pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
    delay(2000);
}
