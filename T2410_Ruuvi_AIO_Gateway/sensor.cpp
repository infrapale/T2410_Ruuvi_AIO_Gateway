#include "Arduino.h"
#include "main.h"
#include "RuuviTag.h"


extern RuuviTag      ruuvi_tag;
extern main_ctrl_st  main_ctrl;
// extern BLEScan       *pBLEScan;

void sensor_initialize(void)
{
    /// Define ruuvi tag data
    ruuvi_tag.add(String("e6:2c:8d:db:22:35"),"Jaakaappi yla");
    ruuvi_tag.add(String("ea:78:e2:12:36:f8"),"Jaakaappi keski");
    ruuvi_tag.add(String("ed:9a:ab:c6:30:72"),"Jaakaappi vihannes");    

}


void sensor_print_values(void)
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


