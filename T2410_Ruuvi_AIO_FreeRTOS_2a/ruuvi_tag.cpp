#include "Arduino.h"
#include "ruuvi_tag.h"
#include "main.h"
#include "helpers.h"


RuuviTag::RuuviTag(void)
{
    memset(ruuvi,0x00,sizeof(ruuvi));
    nbr_of = 0;
    _active_indx = -1;

    // BLEDevice::init("");
    // pBLEScan = BLEDevice::getScan();  //create new scan
    // pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    // pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
    // pBLEScan->setInterval(100);
    // pBLEScan->setWindow(99);  // less or equal setInterval value

}

int8_t RuuviTag::add(const char *mac_address, const char *loc)
{
    if (_active_indx < 0 )
    {
        _active_indx = 0;
        nbr_of = 1;
    }
    else
    {
        if  (_active_indx < MAX_NBR_RUUVI_TAG -1 ) 
        {
            _active_indx++;
            nbr_of++;
        }
        else
        {
            _active_indx = - 1;
        }
    }

    if (_active_indx >= 0) 
    {
        ruuvi[_active_indx].mac_addr = String(mac_address);
        strncpy(ruuvi[_active_indx].location, loc, RUUVI_LOCATION_LEN);
    }
    Serial.printf("add:  %d = %d  ",nbr_of, _active_indx);
    Serial.println(mac_address);
    Serial.println(ruuvi[_active_indx].mac_addr);
    return  _active_indx;
}


String *RuuviTag::get_addr(uint8_t indx)
{
    if (indx < nbr_of)
    {
        return &ruuvi[indx].mac_addr;  
    }
    else
    {
        return &undef_addr;
    }
}

uint8_t RuuviTag::get_index(String mac_addr)
{
    uint8_t ret_indx = NOT_A_RUUVI;
    uint8_t indx = 0;
    while((indx < nbr_of) && (ret_indx == NOT_A_RUUVI))
    {
        //Serial.printf("?? %d: ",indx);
        //Serial.print( mac_addr ); Serial.print(" == "); Serial.println(ruuvi[indx].mac_addr);
        if (mac_addr == ruuvi[indx].mac_addr)  ret_indx = indx;
        indx++;
    }
    return ret_indx;
}

int8_t RuuviTag::add_if_new(const char *mac_address, const char *loc)
{
    uint8_t rindx = NOT_A_RUUVI;
    rindx = get_index(mac_address);
    if (rindx == NOT_A_RUUVI) rindx = add(mac_address, loc);
    return rindx;
}

ruuvi_tag_st *RuuviTag::get_data_ptr(String mac_addr)
{
    uint8_t tag_indx =  get_index(mac_addr);
    if (tag_indx <= nbr_of) return (&ruuvi[tag_indx]);
    else return (NULL);
}

bool RuuviTag::is_a_defined_ruuvi(String mac_addr)
{
    return (get_index(mac_addr) <= nbr_of);
}


char *RuuviTag::get_location(uint8_t indx)
{
    if (indx < nbr_of)
    {
        return ruuvi[indx].location;  
    }
    else
    {
        return ruuvi[0].location;
    }
}

float RuuviTag::get_temperature(uint8_t indx)
{
    return ruuvi[indx].temp_fp;
}

float RuuviTag::get_humidity(uint8_t indx)
{
    return ruuvi[indx].humidity;
}

bool RuuviTag::get_updated(uint8_t indx)
{
    return ruuvi[indx].updated;
}

void RuuviTag::clr_updated(uint8_t indx)
{
    ruuvi[indx].updated = false;
}


/** 
 *  Decodes RUUVI raw data and arranges it in an array
 *  Decoding depends on the RuuviTag data version 
 *  @param[in] mac_addr = mac address from sensor
 *  @param[in] rssi = BLR signal strength
 */ 
void RuuviTag::decode_raw_data(String mac_addr, String raw_data, int rssi)
{
    for (uint8_t indx = 0; indx < nbr_of; indx++)
    {
        if(ruuvi[indx].mac_addr.indexOf(mac_addr)  >= 0 )
        {
            if(raw_data.substring(4, 6) == "03")
            {   
                ruuvi[indx].temperature = hexadecimalToDecimal(raw_data.substring(8, 10));
                ruuvi[indx].temp_fp = (float) ruuvi[indx].temperature + (float) hexadecimalToDecimal(raw_data.substring(10, 12)) / 100.0;
                ruuvi[indx].humidity = hexadecimalToDecimal(raw_data.substring(6, 8));
                //ruuvi[indx].pressure = hexadecimalToDecimal(raw_data.substring(12, 16))-50000;
        
                //ax = hexadecimalToDecimal(raw_data.substring(18, 22));
                //ay = hexadecimalToDecimal(raw_data.substring(22, 26));
                //az = hexadecimalToDecimal(raw_data.substring(26, 30));     
        
                //if(ax > 0xF000){ax = ax - (1 << 16);}
                //if(ay > 0xF000){ay = ay - (1 << 16);}
                //if (az > 0xF000){az = az - (1 << 16);}
        
                //ruuvi[indx].voltage_power = hexadecimalToDecimal(raw_data.substring(30, 34));
                //ruuvi[indx].voltage = (int)((voltage_power & 0x0b1111111111100000) >> 5) + 1600;
                //ruuvi[indx].power = (int)(voltage_power & 0x0b0000000000011111) - 40;
        
                ruuvi[indx].rssi = rssi;
        
                //ruuvi[indx].movement = hexadecimalToDecimal(raw_data.substring(34, 36));
                //ruuvi[indx].measurement = hexadecimalToDecimal(raw_data.substring(36, 40));
                ruuvi[indx].updated = true;
            }
            if(raw_data.substring(4, 6) == "05")
            {
                ruuvi[indx].temperature = hexadecimalToDecimal(raw_data.substring(6, 10));
                ruuvi[indx].temp_fp = (float) ruuvi[indx].temperature * 0.005;
                ruuvi[indx].humidity = hexadecimalToDecimal(raw_data.substring(10, 14)) *0.000025;
                ruuvi[indx].updated = true;        
            }

        
        }
        // Serial.printf("%20s ",ruuvi[indx].location);
        // Serial.printf("Temperature: %f ", ruuvi[indx].temp_fp);
        // Serial.printf("Humidity: %f\n", ruuvi[indx].humidity);
    }
}
