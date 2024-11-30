#include "main.h"
#include <esp_task_wdt.h>
#include "supervisor.h"
#include "helpers.h"

super_st super[SUPER_NBR_OF] =
{   //                     12345678
    [SUPER_RUUVI_INDX] = {"Ruuvi  ", 0, 60},
    [SUPER_WIFI_INDX]  = {"WiFi   ", 0, 60},
    [SUPER_MQTT_INDX]  = {"MQTT   ", 0, 60},

};

void super_print_state(void)
{
  Serial.print("Super State: ");
  for (uint8_t tindx = 0; tindx < SUPER_NBR_OF; tindx++)
  {
    Serial.printf(" %s = %d/%d", super[tindx].label, super[tindx].cnt, super[tindx].cnt_limit );
  }
  Serial.println();
}

void super_task_code( void *pvParameters ){
    uint8_t state = 0;
    bool clear_wd;

    // if (io_wd_is_enabled())
    // {
    //   Serial.printf("Add Supervisor task to WD\n");
    //   //esp_task_wdt_add(NULL); //add current thread to WDT watch
    // }

    for (;;)
    {
        clear_wd = true;
        switch(state)
        {
            case 0:
              state = 10;
              break;

            case 10:
              for (uint8_t tindx = 0; tindx < SUPER_NBR_OF; tindx++)
              {
                if(super[tindx].cnt++ > super[tindx].cnt_limit)
                {
                  Serial.printf("Super: Task counter limit exceeded %s %d\n",super[tindx].label, super[tindx].cnt );
                  clear_wd = false;
                  digitalWrite(PIN_LED_RED, HIGH);
                  state = 100;
                  break;
                }
              }
              break;
            case 100:
              clear_wd = false;   
              Serial.println("Waiting for the watchdog...");
              super_print_state();
              break;
        }
        if (io_wd_is_enabled() && clear_wd)
        {
          esp_task_wdt_reset();
        }
        vTaskDelay(1000);
    }

}

void super_clr_task_cntr(super_indx_et sindx)
{
    super[sindx].cnt = 0;
}