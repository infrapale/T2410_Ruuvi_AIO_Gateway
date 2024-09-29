/******************************************************************************
  T2410_Ruuvi_Serial_Gateway
  - MCU:    ESP32 (Dev kit)
  - HW:     -
  - Author: Tom Höglund  infrapale@gmail.com
  - Github: https://github.com/infrapale/T2410_Ruuvi_AIO_Gateway.git
*******************************************************************************  
This application is scanning BLE for finding Ruuvi sensors defined in a list.
- Scan BLE advertisements
- Save sensor values when a defined Ruuvi sensor was found
- Print sensor values one sensor at a time
- Apply a delay between printouts
*******************************************************************************  
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
******************************************************************************/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "main.h"
#include "RuuviTag.h"
#include "helpers.h"

#define CAPTION_LEN               40      ///< Length of value name
#define MAC_ADDR_LEN              18      ///< Length of the BLE MAC address string
#define MQTT_UPDATE_INTERVAL_ms   60000   ///< MQTT update interval, one value per time
#define PRINT_INTERVAL            10000
#define BLE_SCAN_INTERVAL         20000
#define BLE_SCAN_TIME_SEC         5
#define WDT_TIMEOUT               10


RuuviTag      ruuvi_tag;
main_ctrl_st  main_ctrl;
BLEScan       *pBLEScan;

// Function Prototypes
void print_ruuvi_values(void);

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


void setup() 
{
    Serial.begin(115200);
    Serial.println("Scanning...");
    main_ctrl.state = 0;
    main_ctrl.next_print_ms = millis() + PRINT_INTERVAL;
    main_ctrl.next_ble_scan_ms = millis();  // + BLE_SCAN_INTERVAL ;
    main_ctrl.sensor_indx = 0; 
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();  //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value

    /// Define ruuvi tag data
    ruuvi_tag.add(String("e6:2c:8d:db:22:35"),"Jaakaappi yla");
    ruuvi_tag.add(String("ea:78:e2:12:36:f8"),"Jaakaappi keski");
    ruuvi_tag.add(String("ed:9a:ab:c6:30:72"),"Jaakaappi vihannes");    
}

void loop() 
{
    switch(main_ctrl.state)
    {
        case 0:
            main_ctrl.state = 10;
            break;
        case 10:
            if (millis() > main_ctrl.next_ble_scan_ms)
            {
                main_ctrl.next_ble_scan_ms = millis() + BLE_SCAN_INTERVAL ;
                BLEScanResults *foundDevices = pBLEScan->start(BLE_SCAN_TIME_SEC, false);
                if (BLE_VERBOSE) Serial.printf("Devices found: %d \n", foundDevices->getCount());
                pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
            }
            main_ctrl.state = 20;
            break;
        case 20:
            if (millis() > main_ctrl.next_print_ms)
            {
                main_ctrl.next_print_ms = millis() + PRINT_INTERVAL;
                print_ruuvi_values();
            }
            main_ctrl.state = 10;
            break;
        case 30:
            main_ctrl.state = 10;
            break;
    }
}

void print_ruuvi_values(void)
{
    // Serial.printf("print_ruuvi_task: %d \n ", main_ctrl.sensor_indx);
    if( ruuvi_tag.get_updated(main_ctrl.sensor_indx))
    {
        Serial.printf("%-20s ", ruuvi_tag.get_location(main_ctrl.sensor_indx));
        Serial.printf("Temperature: %6.1f ", ruuvi_tag.get_temperature(main_ctrl.sensor_indx));
        Serial.printf("Humidity: %6.1f\n", ruuvi_tag.get_humidity(main_ctrl.sensor_indx));
        ruuvi_tag.clr_updated(main_ctrl.sensor_indx);
    }
    main_ctrl.sensor_indx++;
    if (main_ctrl.sensor_indx >= MAX_NBR_RUUVI_TAG) main_ctrl.sensor_indx = 0;
}
        
